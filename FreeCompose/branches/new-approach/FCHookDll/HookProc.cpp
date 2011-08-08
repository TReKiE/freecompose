#include "stdafx.h"

#include <Bitset.h>
#include <KeyIsXAlnum.h>
#include <Unicode.h>

#include "FCHookDll.h"
#include "HookProc.h"
#include "Common.h"

#include "Key.h"
#include "KeyEventHandler.h"
#include "CapsLock.h"

//==============================================================================
// Constants
//==============================================================================

const UINT FCM_PIP = RegisterWindowMessage( L"FcHookDll.FCM_PIP" );
const UINT FCM_KEY = RegisterWindowMessage( L"FcHookDll.FCM_KEY" );

//==============================================================================
// Variables
//==============================================================================

#pragma data_seg( push, ".shareddata" )

int ComposeState = 0;
DWORD key1 = 0;
DWORD key2 = 0;

KeyEventHandler* keyEventHandler[256];

zive::bitset< 256, DWORD > WantedKeys;

CapsLockMutator* capsLockMutator = NULL;
CapsLockToggler* capsLockToggler = NULL;
class ComposeKeyHandler* composeKeyHandler = NULL;

#pragma data_seg( pop )

//==============================================================================
// Prototypes
//==============================================================================

static COMPOSE_SEQUENCE* FindKey( COMPOSE_SEQUENCE const& needle );
static COMPOSE_SEQUENCE* TranslateKey( unsigned ch1, unsigned ch2 );

static void MakeUnicodeKeyDown( INPUT& input, wchar_t ch );
static void MakeUnicodeKeyUp( INPUT& input, wchar_t ch );
static bool SendKey( COMPOSE_SEQUENCE* sequence );

static void RegenerateKey( KBDLLHOOKSTRUCT* pkb );

//==============================================================================
// Classes and functions
//==============================================================================

//
// Compose key handler.
//

class ComposeKeyHandler: public KeyEventHandler {
public:
	ComposeKeyHandler( ) {
	}

	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* pkb ) {
		return D_NOT_HANDLED;
	}

	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* pkb ) {
		return D_NOT_HANDLED;
	}
};


static inline COMPOSE_SEQUENCE* FindKey( COMPOSE_SEQUENCE const& needle ) {
	return reinterpret_cast< COMPOSE_SEQUENCE* >( bsearch( &needle, ComposeSequences, cComposeSequences, sizeof( COMPOSE_SEQUENCE ), CompareComposeSequences ) );
}

static COMPOSE_SEQUENCE* TranslateKey( unsigned ch1, unsigned ch2 ) {
	COMPOSE_SEQUENCE* match = NULL;

	LOCK( cs ) {
		if ( ! cComposeSequences || cComposeSequences < 1 )
			break;

		COMPOSE_SEQUENCE needle = { ch1, ch2 };
		match = FindKey( needle );
		if ( ! match ) {
			needle.chFirst  = ch2;
			needle.chSecond = ch1;
			match = FindKey( needle );
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

	::PostMessage( HWND_BROADCAST, FCM_KEY, 0, (LPARAM) sequence->chComposed );
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

//==============================================================================
// External entry points
//==============================================================================

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam ) {
	KBDLLHOOKSTRUCT* pkb = (KBDLLHOOKSTRUCT*) lParam;

	//
	// If nCode is negative, we are required to immediately pass control to the
	// next handler. Otherwise, if somebody (including us!) synthesized this
	// key event, ignore it and pass control to the next handler.
	//

	if ( nCode < 0 || Key::isInjected( pkb ) ) {
		goto acceptKey;
	}

	//debug( L"LLKP|ComposeState=%d nCode=%d wParam=%04x vk=%02x scan=%08x flags=%08x\n", ComposeState, nCode, wParam, pkb->vkCode, pkb->scanCode, pkb->flags );

	//
	// Caps Lock processing.
	//

	DISPOSITION dMutator = D_NOT_HANDLED;
	if ( capsLockMutator ) {
		if ( Key::isKeyDownEvent( pkb ) ) {
			dMutator = capsLockMutator->KeyDown( pkb );
		} else {
			dMutator = capsLockMutator->KeyUp( pkb );
		}
	}

	DISPOSITION dToggler = D_NOT_HANDLED;
	if ( capsLockToggler ) {
		if ( Key::isKeyDownEvent( pkb ) ) {
			dToggler = capsLockToggler->KeyDown( pkb );
		} else {
			dToggler = capsLockToggler->KeyUp( pkb );
		}
	}

	// Result of CapsLockToggler takes precedence over CapsLockMutator.
	switch ( dToggler ) {
		case D_NOT_HANDLED:
			break;
	
		case D_ACCEPT_KEY:
			goto acceptKey;
	
		case D_REJECT_KEY:
			goto rejectKey;
	
		case D_REGENERATE_KEY:
			debug( L"LLKP|CapsLockToggler returned D_REGENERATE_KEY??" );
			DebugBreak( );
			break;
	}

	switch ( dMutator ) {
		case D_NOT_HANDLED:
			break;
	
		case D_ACCEPT_KEY:
			debug( L"LLKP|CapsLockMutator returned D_ACCEPT_KEY??" );
			DebugBreak( );
			break;
	
		case D_REJECT_KEY:
			debug( L"LLKP|CapsLockMutator returned D_REJECT_KEY??" );
			DebugBreak( );
			break;
	
		case D_REGENERATE_KEY:
			goto regenerateKey;
	}


#if 0
	//
	// If this is a key-up event for a key-down event we swallowed, then swallow it as well.
	//

	if ( Key::isKeyUpEvent( pkb ) && WantedKeys.Contains( pkb->vkCode ) ) {
		debug( L"LLKP|eating wanted key %u\n", pkb->vkCode );
		WantedKeys.Remove( pkb->vkCode );
		return 1;
	}

	//
	// Main state machine.
	//

	switch ( ComposeState ) {
		case 0:
			if ( Key::isKeyDownEvent( pkb ) ) {
				if ( Key::isCompose( pkb ) ) {
					debug(L"LLKP|0=>1 Apps down\n");
					ComposeState = 1;
					WantedKeys.Add( vkCompose );
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_OK_1, 0 );
					return 1;
//				} else if ( Key::isCapsLock( pkb ) && HandleCLToggleMode( ) ) {
//					return 1;
				}
			}

			goto acceptKey;

		case 1:
			if ( Key::isKeyDownEvent( pkb ) ) {
				if ( Key::isXAlNum( pkb ) ) {
					debug(L"LLKP|1=>2 Xalnum down\n");
					ComposeState = 2;
					WantedKeys.Add( pkb->vkCode );
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_OK_2, 0 );
					key1 = /*( (DWORD) shift << 31 ) | */pkb->vkCode;
					return 1;
				} else if ( Key::isCompose( pkb ) && ! WantedKeys.Contains( vkCompose ) ) {
					debug(L"LLKP|1=>0: Apps down abort\n");
					ComposeState = 0;
					::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_ABORT, 0 );
//					if ( Key::isCapsLock( pkb ) && HandleCLToggleMode( ) ) {
//						return 1;
//					}
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
			if ( Key::isKeyDownEvent( pkb ) ) {
				if ( Key::isXAlNum( pkb ) ) {
					debug(L"LLKP|2=>0 Xalnum down\n");
					ComposeState = 0;
					WantedKeys.Add( pkb->vkCode );
					key2 = /*( (DWORD) shift << 31 ) | */pkb->vkCode;

					WPARAM pip = PIP_FAIL;
					debug(L"LLKP|2=>0 keys %08x %08x\n", key1, key2);
					COMPOSE_SEQUENCE* sequence = TranslateKey( key1, key2 );
					if ( ! sequence ) {
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
				} else if ( Key::isCompose( pkb ) && ! WantedKeys.Contains( vkCompose ) ) {
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
			if ( Key::isKeyDownEvent( pkb ) ) {
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
#endif

rejectKey:
	debug( L"LLKP|rejectKey\n" );
	ComposeState = 0;
	::PostMessage( HWND_BROADCAST, FCM_PIP, PIP_ERROR, 0 );
	return 1;

regenerateKey:
	RegenerateKey( pkb );
	return 1;

acceptKey:
	return CallNextHookEx( hHook, nCode, wParam, lParam );
}


void InitializeKeyEventDispatcher( void ) {
	memset( keyEventHandler, 0, sizeof( keyEventHandler ) );
	composeKeyHandler = new ComposeKeyHandler;
	keyEventHandler[ vkCompose ] = composeKeyHandler;
}

void ChangeComposeKey( DWORD const vkNew ) {
	keyEventHandler[ vkCompose ] = NULL;
	keyEventHandler[ vkNew ] = composeKeyHandler;
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
}
