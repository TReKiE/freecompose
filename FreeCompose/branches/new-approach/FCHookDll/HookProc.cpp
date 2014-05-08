#include "stdafx.h"

#include <string>

#include <Unicode.h>

#include "Key.h"

#include "HookProc.h"
#include "Stringify.h"
#include "CapsLock.h"
#include "ComposeKeyHandler.h"

using namespace std;

//==============================================================================
// Global variables
//==============================================================================

CapsLockMutator* capsLockMutator = nullptr;
CapsLockToggler* capsLockToggler = nullptr;
wstring          translationBuffer;

//==============================================================================
// Prototypes
//==============================================================================

//static int FindComposeSequence( unsigned ch1, unsigned ch2 );

static void MakeUnicodeKeyDown( INPUT& input, wchar_t ch );
static void MakeUnicodeKeyUp( INPUT& input, wchar_t ch );
//static bool SendKey( ComposeSequence* sequence );
static void RegenerateKey( KBDLLHOOKSTRUCT* pkb );
static bool TranslateKey( KBDLLHOOKSTRUCT* pkb, wstring& translation );

//==============================================================================
// Static functions
//==============================================================================

//static int FindComposeSequence( unsigned ch1, unsigned ch2 ) {
//	ComposeSequence needle1 = { ch1, ch2 };
//	ComposeSequence needle2 = { ch2, ch1 };
//	ComposeSequence* match = nullptr;
//
//	LOCK( cs ) {
//		if ( cComposeSequences < 1 )
//			break;
//
//		match = reinterpret_cast< ComposeSequence* >( bsearch( &needle1, ComposeSequences, cComposeSequences, sizeof( ComposeSequence ), CompareComposeSequences ) );
//		if ( !match ) {
//			match = reinterpret_cast< ComposeSequence* >( bsearch( &needle2, ComposeSequences, cComposeSequences, sizeof( ComposeSequence ), CompareComposeSequences ) );
//		}
//	} UNLOCK( cs );
//
//	return match ? match->chComposed : -1;
//}

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

//static bool SendKey( ComposeSequence* sequence ) {
//	UINT numInputsToSend;
//	INPUT input[4] = { 0, };
//	wchar_t ch[3]  = { 0, };
//
//	if ( sequence->chComposed < 0x10000U ) {
//		numInputsToSend = 2;
//		ch[0] = static_cast<wchar_t>( sequence->chComposed );
//	} else {
//		numInputsToSend = 4;
//		ch[0] = MakeFirstSurrogate( sequence->chComposed );
//		ch[1] = MakeSecondSurrogate( sequence->chComposed );
//	}
//	debug( L"SendKey: chComposed=U+%06x '%s' numInputsToSend=%u\n", sequence->chComposed, ch, numInputsToSend );
//
//	MakeUnicodeKeyDown( input[0], ch[0] );
//	MakeUnicodeKeyUp( input[1], ch[0] );
//	if ( ch[1] ) {
//		MakeUnicodeKeyDown( input[2], ch[1] );
//		MakeUnicodeKeyUp( input[3], ch[1] );
//	}
//
//	UINT u = SendInput( numInputsToSend, input, sizeof( INPUT ) );
//	if ( u < numInputsToSend ) {
//		debug( L"SendKey: SendInput failed? sent=%u err=%lu\n", u, GetLastError( ) );
//		return false;
//	}
//
//	::PostMessage( hwndNotifyWindow, FCM_KEY, 0, (LPARAM) sequence->chComposed );
//	return true;
//}

static void RegenerateKey( KBDLLHOOKSTRUCT* pkb ) {
	INPUT input = { 0, };

	debug( L"RegenerateKey: vk=0x%08x flags=0x%08x\n", pkb->vkCode, pkb->flags );
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = static_cast<WORD>( pkb->vkCode );
	input.ki.time = pkb->time;
	input.ki.dwExtraInfo = pkb->dwExtraInfo;

	if ( Key::isExtended( pkb ) ) input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
	if ( Key::isKeyUpEvent( pkb ) ) input.ki.dwFlags |= KEYEVENTF_KEYUP;

	UINT u = SendInput( 1, &input, sizeof(input) );
	if ( u < 1 ) {
		debug( L"RegenerateKey: SendInput failed? sent=%u err=%lu\n", u, GetLastError( ) );
	}
}

static bool TranslateKey( KBDLLHOOKSTRUCT* pkb, wstring& translation ) {
	translation.clear( );

	// We need to call GetKeyState() before we call GetKeyboardState(), or, for
	// unknown reasons, the keyboard state array will not be up-to-date.
	GetKeyState( VK_SHIFT );

	BYTE keyState[256];
	if ( !GetKeyboardState( keyState ) ) {
		debug( L"TranslateKey: GetKeyboardState: error=%lu\n", GetLastError( ) );
		return false;
	}

	HWND hwndForeground = GetForegroundWindow( );
	DWORD pid, tid = GetWindowThreadProcessId( hwndForeground, &pid );
	HKL hkl = GetKeyboardLayout( tid );
	debug( L"TranslateKey: hwndForeground: 0x%p, pid: %lu, tid: %lu, hkl: 0x%p\n", hwndForeground, pid, tid, hkl );

	wchar_t buf[65] = { 0, }; // accept up to 64 characters, plus the terminating NUL
	int rc = ToUnicodeEx( pkb->vkCode, pkb->scanCode, keyState, buf, 64, 0, hkl );
	if ( 0 == rc ) {
		debug( L"TranslateKey: ToUnicodeEx: 0 no translation\n" );
		return false;
	}

	if ( -1 == rc ) {
		debug( L"TranslateKey: ToUnicodeEx: -1 dead key\n" );
		return true;
	}

	if ( rc < -1 ) {
		debug( L"TranslateKey: ToUnicodeEx: returned %d?\n", rc );
		return false;
	}

	debug( L"TranslateKey: ToUnicodeEx: %d bytes\n", rc );

#ifdef _DEBUG
	{
		debug( L"-- data: " );
		for ( int n = 0; n < rc; n++ ) {
			debug( L"0x%04X ", buf[n] );
		}

		wchar_t printbuf[ _countof( buf ) ];
		for ( int n = 0; n < _countof( printbuf ); n++ ) {
			if ( 0 == buf[n] ) {
				printbuf[n] = 0;
				break;
			}

			printbuf[n] = iswprint( buf[n] ) ? buf[n] : '.';
		}
		debug( L" -- {%s}\n", printbuf );
	}
#endif

	translation = buf;
	return true;
}

//==============================================================================
// Global functions
//==============================================================================

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam ) {
	KBDLLHOOKSTRUCT* pkb = reinterpret_cast<KBDLLHOOKSTRUCT*>( lParam );
	wstring translated;

	//
	// If nCode is negative, we are required to immediately pass control to the
	// next handler. Also, if somebody (including us!) synthesized this key
	// event, ignore it and pass control to the next handler.
	//

	if ( nCode < 0 || Key::isInjected( pkb ) ) {
		goto acceptKey;
	}

	bool isKeyDown = Key::isKeyDownEvent( pkb );
	debug( L"LLKP|ComposeState=%s wParam=0x%04llX isKeyDown=%s pkb->vkCode=0x%02x pkb->scanCode=0x%08x pkb->flags=0x%08x\n", Stringify::from_COMPOSE_STATE( ComposeState ), wParam, Stringify::from_bool( isKeyDown ), pkb->vkCode, pkb->scanCode, pkb->flags );

	DISPOSITION dHandler = D_NOT_HANDLED;
	KeyEventHandler* keh = KeyEventHandlers[ pkb->vkCode ];
	debug( L"LLKP|Calling KeyEventHandlers[%lu](0x%p)->Key%s(pkb=0x%p)\n", pkb->vkCode, keh, isKeyDown ? L"Down" : L"Up", pkb );
	if ( keh ) {
		if ( isKeyDown ) {
			dHandler = keh->KeyDown( pkb );
		} else {
			dHandler = keh->KeyUp( pkb );
		}
	}
	debug( L"LLKP|Call result: disposition is %s\n", Stringify::from_DISPOSITION( dHandler ) );

	switch ( dHandler ) {
		case D_ACCEPT_KEY:     goto acceptKey;
		case D_REJECT_KEY:     goto rejectKey;
		case D_REGENERATE_KEY: goto regenerateKey;
		case D_NOT_HANDLED:    break; // "fall through" into next section of code, rather than next case label.

#ifdef _DEBUG
		default:
			debug( L"LLKP|Unknown DISPOSITION value %d!!\n", dHandler );
			DebugBreak( );
			break;
#endif
	}

	//
	// Code needs to go in here to translate the keystroke's key code into characters, and then use _that_ to drive the composition lookup
	//

	if ( csCOMPOSE != ComposeState ) {
		goto acceptKey;
	}

	debug( L"LLKP|Calling TranslateKey. translationBuffer is currently {%s}.\n", translationBuffer.c_str( ) );
	if ( !TranslateKey( pkb, translated ) ) {
		debug( L"LLKP|Call to TranslateKey failed. clearing translationBuffer, exiting COMPOSE state.\n" );
		translationBuffer.clear( );
		ComposeState = csNORMAL;
		goto acceptKey;
	}

	debug( L"LLKP|TranslateKey returned {%s}. Appending to translationBuffer.\n", translated.c_str( ) );
	translationBuffer += translated;
	goto rejectKey;

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
	KeyEventHandlers[ vkCompose ] = nullptr;
	KeyEventHandlers[ vkNew ] = keh;
	vkCompose = vkNew;
}

void ChangeCapsLockSwapKey( DWORD const vkNew ) {
	KeyEventHandler* keh = KeyEventHandlers[ vkCapsLockSwap ];
	KeyEventHandlers[ vkCapsLockSwap ] = nullptr;
	KeyEventHandlers[ vkNew ] = keh;
	vkCapsLockSwap = vkNew;
}

void ConfigureCapsLockHandling( void ) {
	if ( capsLockToggler ) {
		delete capsLockToggler;
		capsLockToggler = nullptr;
	}
	capsLockToggler = CapsLockTogglerFactory::Create( clToggleMode );

	if ( capsLockMutator ) {
		delete capsLockMutator;
		capsLockMutator = nullptr;
	}
	capsLockMutator = CapsLockMutatorFactory::Create( clSwapMode );

	if ( capsLockToggler || capsLockMutator ) {
		if ( !KeyEventHandlers[ VK_CAPITAL ] ) {
			KeyEventHandlers[ VK_CAPITAL ] = new CapsLockKeyHandler;
		}
	} else {
		if ( KeyEventHandlers[ VK_CAPITAL ] ) {
			KeyEventHandler* temp = KeyEventHandlers[ VK_CAPITAL ];
			KeyEventHandlers[ VK_CAPITAL ] = nullptr;
			if ( temp ) {
				delete temp;
			}
		}
	}
}
