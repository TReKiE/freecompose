#include "stdafx.h"

#include <Bitset.h>
#include <KeyIsXAlnum.h>
#include <Unicode.h>

#include "FCHookDll.h"
#include "HookProc.h"
#include "Common.h"

#define KEY_DOWN()     ( 0 == ( pkb->flags & LLKHF_UP       ) )
#define KEY_UP()       ( 0 != ( pkb->flags & LLKHF_UP       ) )
#define KEY_INJECTED() ( 0 != ( pkb->flags & LLKHF_INJECTED ) )
#define KEY_ALTDOWN()  ( 0 != ( pkb->flags & LLKHF_ALTDOWN  ) )

#define KEY_XALNUM() ( KeyIsXAlnum::Test( pkb->vkCode ) )
#define KEY_SHIFT() ( ( VK_LSHIFT == pkb->vkCode ) || ( VK_RSHIFT == pkb->vkCode ) )
#define KEY_COMPOSE() ( vkCompose == pkb->vkCode )
#define KEY_CAPSLOCK() ( VK_CAPITAL == pkb->vkCode )

#pragma data_seg( push, ".shareddata" )
int ComposeState = 0;
DWORD key1 = 0;
DWORD key2 = 0;
bool shift = false;

zive::bitset< 256, DWORD > WantedKeys;
#pragma data_seg( pop )

const UINT FCM_PIP = RegisterWindowMessage( L"FcHookDll.FCM_PIP" );
const UINT FCM_KEY = RegisterWindowMessage( L"FcHookDll.FCM_KEY" );

inline void makeUnicodeKeyDown( INPUT& input, wchar_t ch ) {
	memset( &input, 0, sizeof( input ) );
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = 0;
	input.ki.wScan = ch;
	input.ki.dwFlags = KEYEVENTF_UNICODE;
}

inline void makeUnicodeKeyUp( INPUT& input, wchar_t ch ) {
	memset( &input, 0, sizeof( input ) );
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = 0;
	input.ki.wScan = ch;
	input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
}

bool TranslateKey( DWORD vk1, DWORD vk2, OUT COMPOSE_SEQUENCE& match ) {
	COMPOSE_SEQUENCE needle1 = { vk1, vk2 };
	COMPOSE_SEQUENCE needle2 = { vk2, vk1 };
	COMPOSE_SEQUENCE* pmatch = NULL;

	LOCK( cs ) {
		if ( NULL == cComposeSequences || cComposeSequences < 1 )
			break;
		
		pmatch = (COMPOSE_SEQUENCE*) bsearch( &needle1, ComposeSequences, cComposeSequences, sizeof( COMPOSE_SEQUENCE ), CompareComposeSequences );
		if ( NULL != pmatch )
			break;
		
		pmatch = (COMPOSE_SEQUENCE*) bsearch( &needle2, ComposeSequences, cComposeSequences, sizeof( COMPOSE_SEQUENCE ), CompareComposeSequences );
	} UNLOCK( cs );

	if ( NULL == pmatch ) {
		return false;
	}

	match = *pmatch;
	return true;
}

bool SendKey( COMPOSE_SEQUENCE& sequence ) {
	UINT numInputsToSend;
	INPUT input[4];
	wchar_t ch[3];

	if ( sequence.chComposed < 0x10000 ) {
		numInputsToSend = 2;
		ch[0] = (wchar_t) sequence.chComposed;
		ch[1] = 0;
	} else {
		numInputsToSend = 4;
		ch[0] = MakeFirstSurrogate( sequence.chComposed );
		ch[1] = MakeSecondSurrogate( sequence.chComposed );
	}
	ch[2] = 0;
	debug( L"SendKey: chComposed=U+%06x '%s' numInputsToSend=%u\n", sequence.chComposed, ch, numInputsToSend );

	makeUnicodeKeyDown( input[0], ch[0] );
	makeUnicodeKeyUp( input[1], ch[0] );
	if ( ch[1] ) {
		makeUnicodeKeyDown( input[2], ch[1] );
		makeUnicodeKeyUp( input[3], ch[1] );
	}

	UINT u = SendInput( numInputsToSend, input, sizeof( INPUT ) );
	if ( u < numInputsToSend ) {
		debug( L"SendKey: SendInput failed? sent=%u err=%u\n", u, GetLastError( ) );
		return false;
	}

	::PostMessage( HWND_BROADCAST, FCM_KEY, 0, (LPARAM) sequence.chComposed );
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
		debug( L"RegenerateKey: SendInput failed? sent=%u err=%u\n", u, GetLastError( ) );
	}
}

bool HandleCLToggleMode( void ) {
	if ( CLTM_DISABLED == clToggleMode ) {
		debug( L"HandleCLToggleMode: disabled, eating\n" );
		return true;
	}
	if ( CLTM_PRESSTWICE == clToggleMode ) {
		// first press, don't let through
		debug( L"HandleCLToggleMode: press-twice mode, first press\n" );
		WantedKeys.Add( VK_CAPITAL );
		ComposeState = 3;
		return true;
	}
	return false;
}

bool HandleCLSwapMode( KBDLLHOOKSTRUCT* pkb ) {
	if ( CLSM_SWAP == clSwapMode ) {
		if ( VK_CAPITAL == pkb->vkCode ) {
			debug( L"HandleCLSwapMode: swapping VK_CAPITAL with 0x%02x\n", vkCapsLockSwap );
			pkb->vkCode = vkCapsLockSwap;
			return true;
		} else if ( vkCapsLockSwap == pkb->vkCode ) {
			debug( L"HandleCLSwapMode: swapping 0x%02x with VK_CAPITAL\n", pkb->vkCode );
			pkb->vkCode = VK_CAPITAL;
			return true;
		}
	} else if ( CLSM_REPLACE == clSwapMode ) {
		if ( VK_CAPITAL == pkb->vkCode ) {
			debug( L"HandleCLSwapMode: replacing VK_CAPITAL with 0x%02x\n", vkCapsLockSwap );
			pkb->vkCode = vkCapsLockSwap;
			return true;
		}
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
		debug( L"LLKP|eating wanted key %u\n", pkb->vkCode );
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
				} else if ( KEY_CAPSLOCK() && HandleCLToggleMode( ) ) {
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
					if ( KEY_CAPSLOCK() && HandleCLToggleMode( ) ) {
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

					COMPOSE_SEQUENCE sequence;
					WPARAM pip = PIP_FAIL;
					debug(L"LLKP|2=>0 keys %08x %08x\n", key1, key2);
					if ( ! TranslateKey( key1, key2, sequence ) ) {
						debug(L"LLKP|2=>0 translate failed\n");
					} else {
						if ( ! SendKey( sequence ) ) {
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
