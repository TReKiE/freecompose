#include "stdafx.h"
#include "FCHookDll.h"
#include "HookProc.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef set<DWORD> DwordSet;

#define VK_COMPOSE VK_SCROLL

#define KEY_DOWN()     ( 0 == ( pkb->flags & LLKHF_UP       ) )
#define KEY_UP()       ( 0 != ( pkb->flags & LLKHF_UP       ) )
#define KEY_INJECTED() ( 0 != ( pkb->flags & LLKHF_INJECTED ) )
#define KEY_ALTDOWN()  ( 0 != ( pkb->flags & LLKHF_ALTDOWN  ) )

#define KEY_XALNUM() ( ( pkb->vkCode >= '0'      && pkb->vkCode <= '9'      ) || \
					   ( pkb->vkCode >= 'A'      && pkb->vkCode <= 'Z'      ) || \
					   ( pkb->vkCode >= VK_OEM_1 && pkb->vkCode <= VK_OEM_3 ) || \
					   ( pkb->vkCode >= VK_OEM_4 && pkb->vkCode <= VK_OEM_7 )    )

#define KEY_SHIFT() ( VK_LSHIFT == pkb->vkCode || VK_RSHIFT == pkb->vkCode )
#define KEY_COMPOSE() ( VK_COMPOSE == pkb->vkCode )

#pragma data_seg( push, ".shareddata" )
int ComposeState = 0;
DWORD key1 = 0;
DWORD key2 = 0;
bool shift = false;

DwordSet WantedKeys;
#pragma data_seg( pop )

const UINT FCM_PIP = RegisterWindowMessage( _T("FcHookDll.FCM_PIP") );
const UINT FCM_KEY = RegisterWindowMessage( _T("FcHookDll.FCM_KEY") );

bool TranslateKey( DWORD vk1, DWORD vk2 ) {
	bool f = false;

//	debug( _T( "TranslateKey: vk1=%08x vk2=%08x\n" ), vk1, vk2 );
	EnterCriticalSection( &cs );
		for ( int n = 0; n < cComposeKeyEntries; n++ ) {
			if ( ! ComposeKeyEntries[n].fValid )
				continue;

			if ( 
				( vk1 == ComposeKeyEntries[n].vkFirst && vk2 == ComposeKeyEntries[n].vkSecond ) ||
				( vk2 == ComposeKeyEntries[n].vkFirst && vk1 == ComposeKeyEntries[n].vkSecond )
			) {
				INPUT input;
				memset( &input, 0, sizeof( INPUT ) );
				input.type = INPUT_KEYBOARD;
				input.ki.wVk = 0;
				input.ki.wScan = ComposeKeyEntries[n].wchComposed;
				input.ki.dwFlags = KEYEVENTF_UNICODE;

				UINT u = SendInput( 1, &input, sizeof( INPUT ) );
				if ( u < 1 ) {
					debug( _T( "TranslateKey: SendInput failed? u=%d %d\n" ), u, GetLastError( ) );
				}

				::PostMessage( HWND_BROADCAST, FCM_KEY, (WPARAM) ComposeKeyEntries[n].wchComposed, 0 );

				f = true;
				break;
			}
		}
	LeaveCriticalSection( &cs );
//	debug( _T( "TranslateKey: done: %d\n" ), (int) f );

	return f;
}

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam ) {
	KBDLLHOOKSTRUCT* pkb = (KBDLLHOOKSTRUCT*) lParam;

	if ( nCode < 0 )
		goto acceptKey;

	if ( KEY_INJECTED() ) {
		debug( _T( "LowLevelKeyboardProc: injected event, ignoring (CS=%d)\n" ), ComposeState );
//		ComposeState = 0;
		goto acceptKey;
	}
	if ( KEY_ALTDOWN() ) {
		debug( _T( "LowLevelKeyboardProc: ALT key down, ignoring\n" ) );
		ComposeState = 0;
		goto acceptKey;
	}
	if ( KEY_SHIFT() ) {
		shift = KEY_DOWN();
		goto acceptKey;
	}
	if ( KEY_UP() ) {
		if ( ! WantedKeys.empty() ) {
			DwordSet::iterator it = WantedKeys.find( pkb->vkCode );
			if ( WantedKeys.end() != it ) {
				debug(_T("LLKP: WK eating wanted key\n"));
				WantedKeys.erase( it );
				return 1;
			}
		}
	}

#ifdef _DEBUG
	if ( ComposeState > 0 )
		debug( _T( "LowLevelKeyboardProc: nCode=%d wParam=%04x vk=%08x scan=%08x flags=%08x\n" ), nCode, wParam, pkb->vkCode, pkb->scanCode, pkb->flags );
#endif

	switch ( ComposeState ) {
		case 0:
			if ( KEY_DOWN() ) {
				if ( KEY_COMPOSE() ) {
					debug(_T("LLKP: 0=>1 Apps down\n"));
					ComposeState = 1;
					WantedKeys.insert( VK_COMPOSE );
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_OK_1, 0 );
					return 1;
				}
			}

			goto acceptKey;

		case 1:
			if ( KEY_DOWN() ) {
				if ( KEY_XALNUM() ) {
					debug(_T("LLKP: 1=>2 Xalnum down\n"));
					ComposeState = 2;
					WantedKeys.insert( pkb->vkCode );
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_OK_2, 0 );

					key1 = ( (DWORD) shift << 31 ) | pkb->vkCode;
					return 1;
				} else if ( KEY_COMPOSE() && WantedKeys.end() == WantedKeys.find( VK_COMPOSE ) ) {
					debug(_T("LLKP: 1=>0: Apps down abort\n"));
					ComposeState = 0;
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_ABORT, 0 );
					goto acceptKey;
				} else {
					debug(_T("LLKP: 1=>0 rejecting\n"));
					goto rejectKey;
				}
			}

			debug(_T("LLKP: 1=>0 what?\n"));
			ComposeState = 0;
			goto acceptKey;

		case 2:
			if ( KEY_DOWN() ) {
				if ( KEY_XALNUM() ) {
					debug(_T("LLKP: 2=>0 Xalnum down\n"));
					WantedKeys.insert( pkb->vkCode );
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_OK_3, 0 );

					key2 = ( (DWORD) shift << 31 ) | pkb->vkCode;
					debug(_T("LLKP: 2=>0 keys %08x %08x\n"), key1, key2);
					if ( ! TranslateKey( key1, key2 ) ) {
						debug(_T("LLKP: 2=>0 translate failed\n"));
						::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_FAIL, 0 );
					} else {
						debug(_T("LLKP: 2=>0 translate succeeded\n"));
					}
					ComposeState = 0;
					return 1;
				} else if ( KEY_COMPOSE() && WantedKeys.end() == WantedKeys.find( VK_COMPOSE ) ) {
					debug(_T("LLKP: 2=>0: Apps down abort\n"));
					ComposeState = 0;
					WantedKeys.insert( VK_COMPOSE );
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_ABORT, 0 );
					return 1;
				} else {
					debug(_T("LLKP: 2=>0 rejecting\n"));
					goto rejectKey;
				}
			}

			debug(_T("LLKP: 2=>0 what?\n"));
			ComposeState = 0;
			goto acceptKey;

		default:
			debug( _T("LLKP: ComposeState=%d, wtf?\n"), ComposeState );
			ComposeState = 0;
			goto acceptKey;
	}

rejectKey:
	ComposeState = 0;
	debug(_T("LLKP: rejectKey\n"));
	::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_ERROR, 0 );
	WantedKeys.insert( VK_COMPOSE );
	return 1;

acceptKey:
	return CallNextHookEx( hHook, nCode, wParam, lParam );
}
