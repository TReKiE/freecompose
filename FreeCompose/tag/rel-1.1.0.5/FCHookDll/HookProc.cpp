#include "stdafx.h"
#include "FCHookDll.h"
#include "HookProc.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define KEY_DOWN()     ( 0 == ( pkb->flags & LLKHF_UP       ) )
#define KEY_UP()       ( 0 != ( pkb->flags & LLKHF_UP       ) )
#define KEY_INJECTED() ( 0 != ( pkb->flags & LLKHF_INJECTED ) )
#define KEY_ALTDOWN()  ( 0 != ( pkb->flags & LLKHF_ALTDOWN  ) )

#define KEY_XALNUM() ( keyIsXAlNum.Test( pkb->vkCode ) )
#define KEY_SHIFT() ( VK_LSHIFT == pkb->vkCode || VK_RSHIFT == pkb->vkCode )
#define KEY_COMPOSE() ( vkCompose == pkb->vkCode )
#define KEY_CAPSLOCK() ( VK_CAPITAL == pkb->vkCode )

class KeyIsXAlNum {
public:
	KeyIsXAlNum( ) {
		for ( int n = '0'; n <= '9'; n++ )
			map[n] = true;
		for ( int n = 'A'; n <= 'Z'; n++ )
			map[n] = true;
		for ( int n = VK_OEM_1; n <= VK_OEM_3; n++ )
			map[n] = true;
		for ( int n = VK_OEM_4; n <= VK_OEM_7; n++ )
			map[n] = true;
		map[VK_OEM_102] = true;
	}

	inline bool Test( const DWORD dw ) const {
		if ( dw >= 256 )
			return false;
		return map[dw];
	}

private:
	bool map[256];
};

#pragma data_seg( push, ".shareddata" )
int ComposeState = 0;
DWORD key1 = 0;
DWORD key2 = 0;
bool shift = false;

ByteSet WantedKeys;
const KeyIsXAlNum keyIsXAlNum;
#pragma data_seg( pop )

const UINT FCM_PIP = RegisterWindowMessage( L"FcHookDll.FCM_PIP" );
const UINT FCM_KEY = RegisterWindowMessage( L"FcHookDll.FCM_KEY" );

inline void makeUnicodeKeyDown( INPUT& input, wchar_t ch ) {
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = 0;
	input.ki.wScan = ch;
	input.ki.dwFlags = KEYEVENTF_UNICODE;
}

inline void makeUnicodeKeyUp( INPUT& input, wchar_t ch ) {
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = 0;
	input.ki.wScan = ch;
	input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
}

inline wchar_t makeFirstSurrogate( unsigned ch ) {
	return (wchar_t) ( 0xD800 + ( ( ch - 0x10000 ) >> 10 ) );
}

inline wchar_t makeSecondSurrogate( unsigned ch ) {
	return (wchar_t) ( 0xDC00 + ( ( ch - 0x10000 ) & 0x3FF ) );
}

bool TranslateKey( DWORD vk1, DWORD vk2, COMPOSE_KEY_ENTRY& cke ) {
	COMPOSE_KEY_ENTRY dummy1 = { vk1, vk2 };
	COMPOSE_KEY_ENTRY dummy2 = { vk2, vk1 };
	COMPOSE_KEY_ENTRY* pkey = NULL;

	LOCK( cs ) {
		if ( NULL == cComposeKeyEntries || cComposeKeyEntries < 1 )
			break;
		
		pkey = (COMPOSE_KEY_ENTRY*) bsearch( &dummy1, ComposeKeyEntries, cComposeKeyEntries, sizeof( COMPOSE_KEY_ENTRY ), CompareCkes );
		if ( NULL != pkey )
			break;
		
		pkey = (COMPOSE_KEY_ENTRY*) bsearch( &dummy2, ComposeKeyEntries, cComposeKeyEntries, sizeof( COMPOSE_KEY_ENTRY ), CompareCkes );
	} UNLOCK( cs );

	if ( NULL == pkey ) {
		return false;
	}

	cke = *pkey;
	return true;
}

bool SendKey( COMPOSE_KEY_ENTRY& cke ) {
	UINT numInputsToSend;
	INPUT input[4];
	wchar_t ch[4];

	memset( input, 0, sizeof( input ) );
	memset( ch, 0, sizeof( ch ) );
	if ( cke.u32Composed < 0x10000 ) {
		numInputsToSend = 2;
		ch[0] = (wchar_t) cke.u32Composed;
	} else {
		numInputsToSend = 4;
		ch[0] = makeFirstSurrogate( cke.u32Composed );
		ch[1] = makeSecondSurrogate( cke.u32Composed );
	}
	debug( L"SendKey: u32Composed=U+%08x '%s' numInputsToSend=%d\n", cke.u32Composed, ch, numInputsToSend );
	makeUnicodeKeyDown( input[0], ch[0] );
	makeUnicodeKeyUp( input[1], ch[0] );
	if ( ch[1] ) {
		makeUnicodeKeyDown( input[2], ch[1] );
		makeUnicodeKeyUp( input[3], ch[1] );
	}

	UINT u = SendInput( numInputsToSend, input, sizeof( INPUT ) );
	if ( u < numInputsToSend ) {
		debug( L"SendKey: SendInput failed? sent=%d err=%d\n", u, GetLastError( ) );
		return false;
	}

	::PostMessage( HWND_BROADCAST, FCM_KEY, 0, (LPARAM) cke.u32Composed );
	return true;
}

void RegenerateKey( KBDLLHOOKSTRUCT* pkb ) {
	INPUT input;

	debug( L"RegenerateKey: vk=0x%08x flags=0x%08x\n", pkb->vkCode, pkb->flags );
	memset( &input, 0, sizeof( input ) );
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = (WORD) pkb->vkCode;
	input.ki.dwFlags = 0;
	input.ki.time = pkb->time;
	input.ki.dwExtraInfo = pkb->dwExtraInfo;

	if ( pkb->flags & LLKHF_EXTENDED ) input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
	if ( pkb->flags & LLKHF_UP )       input.ki.dwFlags |= KEYEVENTF_KEYUP;

	UINT u = SendInput( 1, &input, sizeof(input) );
	if ( u < 1 ) {
		debug( L"RegenerateKey: SendInput failed? sent=%d err=%d\n", u, GetLastError( ) );
	}
}

bool HandleCapsLock( void ) {
	if ( CLM_DISABLED == clmCapsLockMode ) {
		debug( L"HandleCapsLock: disabled, eating\n" );
		return true;
	}
	if ( CLM_PRESSTWICE == clmCapsLockMode ) {
		// first press, don't let through
		debug( L"HandleCapsLock: press-twice mode, first press\n" );
		WantedKeys.Add( VK_CAPITAL );
		ComposeState = 3;
		return true;
	}
	return false;
}

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam ) {
	KBDLLHOOKSTRUCT* pkb = (KBDLLHOOKSTRUCT*) lParam;
	bool fRegenerate = false;

	if ( nCode < 0 ) {
		debug( L"LLKP|passthrough, ignoring (nCode=%d)\n", nCode );
		goto acceptKey;
	}

	if ( KEY_INJECTED() ) {
		debug( L"LLKP|injected event, ignoring (CS=%d)\n", ComposeState );
		goto acceptKey;
	}

	//debug( L"LLKP|ComposeState=%d nCode=%d wParam=%04x vk=%02x scan=%08x flags=%08x\n", ComposeState, nCode, wParam, pkb->vkCode, pkb->scanCode, pkb->flags );

	if ( fSwapCapsLock ) {
		if ( VK_CAPITAL == pkb->vkCode ) {
			debug( L"LLKP|Caps Lock: replacing VK_CAPITAL with 0x%02x\n", vkCapsLockSwap );
			pkb->vkCode = vkCapsLockSwap;
			fRegenerate = true;
		} else if ( vkCapsLockSwap == pkb->vkCode ) {
			debug( L"LLKP|Caps Lock: replacing 0x%02x with VK_CAPITAL\n", pkb->vkCode );
			pkb->vkCode = VK_CAPITAL;
			fRegenerate = true;
		}
	}

	if ( KEY_ALTDOWN() ) {
		if ( ComposeState > 0 ) {
			debug( L"LLKP|ALT key down seen, aborting compose\n" );
		}
		ComposeState = 0;
		goto acceptKey;
	}
	if ( KEY_SHIFT() ) {
		shift = KEY_DOWN();
		goto acceptKey;
	}
	if ( KEY_UP() && WantedKeys.Contains( pkb->vkCode ) ) {
		debug( L"LLKP|eating wanted key %d\n", pkb->vkCode );
		WantedKeys.Remove( pkb->vkCode );
		return 1;
	}

	switch ( ComposeState ) {
		case 0:
			if ( KEY_DOWN() ) {
				if ( KEY_COMPOSE() ) {
					debug(L"LLKP|0=>1 Apps down\n");
					ComposeState = 1;
					WantedKeys.Add( vkCompose );
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_OK_1, 0 );
					return 1;
				} else if ( KEY_CAPSLOCK() && HandleCapsLock( ) ) {
					return 1;
				}
			}

			goto acceptKey;

		case 1:
			if ( KEY_DOWN() ) {
				if ( KEY_XALNUM() ) {
					debug(L"LLKP|1=>2 Xalnum down\n");
					ComposeState = 2;
					WantedKeys.Add( pkb->vkCode );
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_OK_2, 0 );
					key1 = ( (DWORD) shift << 31 ) | pkb->vkCode;
					return 1;
				} else if ( KEY_COMPOSE() && ! WantedKeys.Contains( vkCompose ) ) {
					debug(L"LLKP|1=>0: Apps down abort\n");
					ComposeState = 0;
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_ABORT, 0 );
					if ( KEY_CAPSLOCK() && HandleCapsLock( ) ) {
						return 1;
					}
					goto acceptKey;
				} else {
					debug(L"LLKP|1=>0 rejecting\n");
					WantedKeys.Add( pkb->vkCode );
					goto rejectKey;
				}
			}

			debug(L"LLKP|1=>0 what?\n");
			ComposeState = 0;
			::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_ERROR, 0 );
			goto acceptKey;

		case 2:
			if ( KEY_DOWN() ) {
				if ( KEY_XALNUM() ) {
					debug(L"LLKP|2=>0 Xalnum down\n");
					ComposeState = 0;
					WantedKeys.Add( pkb->vkCode );
					key2 = ( (DWORD) shift << 31 ) | pkb->vkCode;

					COMPOSE_KEY_ENTRY cke;
					WPARAM pip = PIP_FAIL;
					debug(L"LLKP|2=>0 keys %08x %08x\n", key1, key2);
					if ( ! TranslateKey( key1, key2, cke ) ) {
						debug(L"LLKP|2=>0 translate failed\n");
					} else {
						if ( ! SendKey( cke ) ) {
							debug(L"LLKP|2=>0 send failed\n");
						} else {
							pip = PIP_OK_3;
						}
					}
					::PostMessage( HWND_BROADCAST, FCM_PIP, pip, 0 );
					return 1;
				} else if ( KEY_COMPOSE() && ! WantedKeys.Contains( vkCompose ) ) {
					debug(L"LLKP|2=>0: Apps down abort\n");
					ComposeState = 0;
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_ABORT, 0 );
					return 1;
				} else {
					debug(L"LLKP|2=>0 rejecting\n");
					WantedKeys.Add( pkb->vkCode );
					goto rejectKey;
				}
			}

			debug(L"LLKP|2=>0 what?\n");
			ComposeState = 0;
			::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_ERROR, 0 );
			goto acceptKey;

		case 3:
			if ( KEY_DOWN() ) {
				if ( VK_CAPITAL == pkb->vkCode ) {
					// second press, allow
					debug( L"LLKP|CapsLock: press-twice mode, second press\n" );
				}
			}

			ComposeState = 0;
			goto acceptKey;

		default:
			debug( L"LLKP|ComposeState=%d, wtf?\n", ComposeState );
			ComposeState = 0;
			::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_ERROR, 0 );
			goto acceptKey;
	}

rejectKey:
	debug(L"LLKP|rejectKey\n");
	ComposeState = 0;
	::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_ERROR, 0 );
	return 1;

acceptKey:
	if ( fRegenerate ) {
		RegenerateKey( pkb );
		return 1;
	}

	return CallNextHookEx( hHook, nCode, wParam, lParam );
}
