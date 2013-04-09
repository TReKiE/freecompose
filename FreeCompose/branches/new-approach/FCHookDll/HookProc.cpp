#include "stdafx.h"

#include <KeyIsXAlnum.h>
#include <Unicode.h>

#include "FCHookDll.h"
#include "Common.h"
#include "Key.h"
#include "KeyEventHandler.h"

#include "HookProc.h"
#include "Stringify.h"
#include "KeyUpSink.h"
#include "CapsLock.h"
#include "ComposeKeyHandler.h"
#include "KeyDownUpSink.h"

//==============================================================================
// Global variables
//==============================================================================

#pragma data_seg( push, ".shareddata" )

CapsLockMutator* capsLockMutator = NULL;
CapsLockToggler* capsLockToggler = NULL;

#pragma data_seg( pop )

//==============================================================================
// Prototypes
//==============================================================================

static COMPOSE_SEQUENCE* FindComposeSequence( unsigned ch1, unsigned ch2 );

static void MakeUnicodeKeyDown( INPUT& input, wchar_t ch );
static void MakeUnicodeKeyUp( INPUT& input, wchar_t ch );
static bool SendKey( COMPOSE_SEQUENCE* sequence );
static void RegenerateKey( KBDLLHOOKSTRUCT* pkb );
static void TranslateKey( KBDLLHOOKSTRUCT* pkb );

//==============================================================================
// Static functions
//==============================================================================

static COMPOSE_SEQUENCE* FindComposeSequence( unsigned ch1, unsigned ch2 ) {
	COMPOSE_SEQUENCE needle1 = { ch1, ch2 };
	COMPOSE_SEQUENCE needle2 = { ch2, ch1 };
	COMPOSE_SEQUENCE* match = NULL;

	LOCK( cs ) {
		if ( cComposeSequences < 1 )
			break;

		match = reinterpret_cast< COMPOSE_SEQUENCE* >( bsearch( &needle1, ComposeSequences, cComposeSequences, sizeof( COMPOSE_SEQUENCE ), CompareComposeSequences ) );
		if ( !match ) {
			match = reinterpret_cast< COMPOSE_SEQUENCE* >( bsearch( &needle2, ComposeSequences, cComposeSequences, sizeof( COMPOSE_SEQUENCE ), CompareComposeSequences ) );
		}
	} UNLOCK( cs );

	return match;
}

static inline void MakeUnicodeKeyDown( INPUT& input, wchar_t ch ) {
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = ch;
	input.ki.dwFlags = KEYEVENTF_UNICODE;
}

static inline void MakeUnicodeKeyUp( INPUT& input, wchar_t ch ) {
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = ch;
	input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
}

static bool SendKey( COMPOSE_SEQUENCE* sequence ) {
	UINT numInputsToSend;
	INPUT input[4] = { 0, };
	wchar_t ch[3]  = { 0, };

	if ( sequence->chComposed < 0x10000U ) {
		numInputsToSend = 2;
		ch[0] = static_cast<wchar_t>( sequence->chComposed );
	} else {
		numInputsToSend = 4;
		ch[0] = MakeFirstSurrogate( sequence->chComposed );
		ch[1] = MakeSecondSurrogate( sequence->chComposed );
	}
	debug( L"SendKey: chComposed=U+%06x '%s' numInputsToSend=%u\n", sequence->chComposed, ch, numInputsToSend );

	MakeUnicodeKeyDown( input[0], ch[0] );
	MakeUnicodeKeyUp( input[1], ch[0] );
	if ( ch[1] ) {
		MakeUnicodeKeyDown( input[2], ch[1] );
		MakeUnicodeKeyUp( input[3], ch[1] );
	}

	UINT u = SendInput( numInputsToSend, input, sizeof( INPUT ) );
	if ( u < numInputsToSend ) {
		debug( L"SendKey: SendInput failed? sent=%u err=%u\n", u, GetLastError( ) );
		return false;
	}

	::PostMessage( hwndNotifyWindow, FCM_KEY, 0, (LPARAM) sequence->chComposed );
	return true;
}

static void RegenerateKey( KBDLLHOOKSTRUCT* pkb ) {
	INPUT input = { 0, };

	debug( L"RegenerateKey: vk=0x%08x flags=0x%08x\n", pkb->vkCode, pkb->flags );
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = (WORD) pkb->vkCode;
	input.ki.time = pkb->time;
	input.ki.dwExtraInfo = pkb->dwExtraInfo;

	if ( Key::isExtended( pkb ) ) input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
	if ( Key::isKeyUpEvent( pkb ) ) input.ki.dwFlags |= KEYEVENTF_KEYUP;

	UINT u = SendInput( 1, &input, sizeof(input) );
	if ( u < 1 ) {
		debug( L"RegenerateKey: SendInput failed? sent=%u err=%u\n", u, GetLastError( ) );
	}
}

static void TranslateKey( KBDLLHOOKSTRUCT* pkb ) {
	// We need to call GetKeyState() before we call GetKeyboardState(), or, for
	// unknown reasons, the keyboard state array will not be up-to-date.
	GetKeyState( VK_SHIFT );

	BYTE keyState[256];
	if ( GetKeyboardState( keyState ) ) {
		HWND hwndForeground = NULL;
		DWORD pid = 0;
		DWORD tid = 0;
		HKL hkl = NULL;

		hwndForeground = GetForegroundWindow( );
		tid = GetWindowThreadProcessId( hwndForeground, &pid );
		hkl = GetKeyboardLayout( tid );
		debug( L"LLKP|hwndForeground: 0x%p, pid: %d, tid: %d, hkl: 0x%p\n", hwndForeground, pid, tid, hkl );

		wchar_t buf[65]; // accept up to 64 characters, plus the terminating NUL
		int rc = ToUnicodeEx( pkb->vkCode, pkb->scanCode, keyState, buf, 65, 0, hkl );
		switch ( rc ) {
			case -1:
				debug( L"LLKP|ToUnicodeEx: -1 dead key\n" );
				break;

			case 0:
				debug( L"LLKP|ToUnicodeEx: 0 no translation\n" );
				break;

			default:
				// really, this is testing if it's less than *-1*, but, well, we need at
				// least 1 anyway, -1 and 0 have been handled, what difference does it make
				if ( rc < 1 ) {
					debug( L"LLKP|ToUnicodeEx: returned %d?\n", rc );
					break;
				}

				debug( L"LLKP|ToUnicodeEx: %d bytes: ", rc );
				for ( int n = 0; n < rc; n++ ) {
					debug( L"0x%04X ", buf[n] );
				}

				wchar_t printbuf[65];
				for ( int n = 0; n < _countof( printbuf ); n++ ) {
					if ( 0 == buf[n] ) {
						printbuf[n] = 0;
						break;
					}

					printbuf[n] = iswprint( buf[n] ) ? buf[n] : '.';
				}
				debug( L" -- {%s}\n", printbuf );
				break;
		}
	} else {
		debug( L"LLKP|GetKeyboardState: error=%u\n", GetLastError( ) );
	}
}

//==============================================================================
// Global functions
//==============================================================================

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam ) {
	KBDLLHOOKSTRUCT* pkb = (KBDLLHOOKSTRUCT*) lParam;

	//
	// If nCode is negative, we are required to immediately pass control to the
	// next handler. Also, if somebody (including us!) synthesized this key
	// event, ignore it and pass control to the next handler.
	//

	if ( nCode < 0 || Key::isInjected( pkb ) ) {
		goto acceptKey;
	}

	bool isKeyDown = Key::isKeyDownEvent( pkb );
	debug( L"LLKP|ComposeState=%s wParam=0x%04x isKeyDown=%s pkb->vkCode=0x%02x pkb->scanCode=0x%08x pkb->flags=0x%08x\n", Stringify::from_COMPOSE_STATE( ComposeState ), wParam, Stringify::from_bool( isKeyDown ), pkb->vkCode, pkb->scanCode, pkb->flags );

	if ( !isKeyDown && WantedKeys.Contains( pkb->vkCode ) ) {
		debug( L"LLKP|vkCode is in WantedKeys, rejecting\n" );
		goto rejectKey;
	}

	DISPOSITION dHandler = D_NOT_HANDLED;
	KeyEventHandler* keh = KeyEventHandlers[ pkb->vkCode ];
	debug( L"LLKP|KeyEventHandlers[%ld]=0x%p\n", pkb->vkCode, keh );
	if ( keh ) {
		if ( isKeyDown ) {
			dHandler = keh->KeyDown( pkb );
		} else {
			dHandler = keh->KeyUp( pkb );
		}
	}
	debug( L"LLKP|disposition is %s\n", Stringify::from_DISPOSITION( dHandler ) );

	switch ( dHandler ) {
		case D_REJECT_KEY:     goto rejectKey;
		case D_REGENERATE_KEY: goto regenerateKey;
		default:               goto acceptKey;
	}

	__assume( 0 ); // not reached

rejectKey:
	return 1;

regenerateKey:
	RegenerateKey( pkb );
	return 1;

acceptKey:
	return CallNextHookEx( hHook, nCode, wParam, lParam );
}

void InitializeKeyEventDispatcher( void ) {
	memset( KeyEventHandlers, 0, sizeof( KeyEventHandlers ) );
	KeyEventHandlers[ vkCompose ] = new ComposeKeyHandler;
}

void ChangeComposeKey( DWORD const vkNew ) {
	KeyEventHandler* keh = KeyEventHandlers[ vkCompose ];
	KeyEventHandlers[ vkCompose ] = NULL;
	KeyEventHandlers[ vkNew ] = keh;
	vkCompose = vkNew;
}

void ChangeCapsLockSwapKey( DWORD const vkNew ) {
	KeyEventHandler* keh = KeyEventHandlers[ vkCapsLockSwap ];
	KeyEventHandlers[ vkCapsLockSwap ] = NULL;
	KeyEventHandlers[ vkNew ] = keh;
	vkCapsLockSwap = vkNew;
}

void ConfigureCapsLockHandling( void ) {
	if ( capsLockToggler ) {
		delete capsLockToggler;
		capsLockToggler = NULL;
	}
	capsLockToggler = CapsLockTogglerFactory::Create( clToggleMode );

	if ( capsLockMutator ) {
		delete capsLockMutator;
		capsLockMutator = NULL;
	}
	capsLockMutator = CapsLockMutatorFactory::Create( clSwapMode );

	if ( capsLockToggler || capsLockMutator ) {
		if ( !KeyEventHandlers[ VK_CAPITAL ] ) {
			KeyEventHandlers[ VK_CAPITAL ] = new CapsLockKeyHandler;
		}
	} else {
		if ( KeyEventHandlers[ VK_CAPITAL ] ) {
			KeyEventHandler* temp = KeyEventHandlers[ VK_CAPITAL ];
			KeyEventHandlers[ VK_CAPITAL ] = NULL;
			if ( temp ) {
				delete temp;
			}
		}
	}
}
