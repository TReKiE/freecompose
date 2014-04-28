#include "stdafx.h"

#include "HookProc.h"
#include "ComposeSequenceTree.h"

using namespace std::tr1;

//==============================================================================
// Local variables
//==============================================================================

static volatile LONG cClients = 0;

//==============================================================================
// Function prototypes
//==============================================================================

static void _SetComposeSequencesImpl( ComposeSequence* pSequences, INT_PTR cSequences );

//==============================================================================
// Private functions
//==============================================================================

static void _SetComposeSequencesImpl( ComposeSequence* pSequences, INT_PTR cSequences ) {
	LOCK( cs ) {
		if ( ComposeSequences ) {
			delete[] ComposeSequences;
		}
		ComposeSequences = pSequences;
		cComposeSequences = cSequences;

		ComposeSequenceTree.BuildTree( ComposeSequences, cComposeSequences );
	} UNLOCK( cs );
}

//==============================================================================
// Library-internal functions
//==============================================================================

void ReleaseComposeSequences( void ) {
	_SetComposeSequencesImpl( nullptr, 0 );
	ComposeSequenceTree.ReleaseTree( );
}

//==============================================================================
// FCHookDll API
//==============================================================================

//==============================================================================
// Suite 0: API version
//==============================================================================

FCHOOKDLL_API DWORD FcGetApiVersion( void ) {
	return FCHOOKDLL_API_VERSION;
}

//==============================================================================
// Suite 1: Lifetime management
//==============================================================================

FCHOOKDLL_API BOOL FcInitialize( void ) {
	if ( 1 == InterlockedIncrement( &cClients ) ) {
		InitializeKeyEventDispatcher( );
	}

	return TRUE;
}

FCHOOKDLL_API BOOL FcUninitialize( void ) {
	if ( 0 == InterlockedDecrement( &cClients ) ) {
		ReleaseComposeSequences( );
	}

	return TRUE;
}

//==============================================================================
// Suite 2: Hook control
//==============================================================================

FCHOOKDLL_API BOOL FcEnableHook( void ) {
#if UI_DEBUGGING_ONLY
	return FALSE;
#else
	DWORD dwError = ERROR_SUCCESS;
	BOOL ret = FALSE;

	LOCK( cs ) {
		if ( hHook ) {
			break;
		}

		SetLastError( ERROR_SUCCESS );
		hHook = SetWindowsHookEx( WH_KEYBOARD_LL, LowLevelKeyboardProc, hDllInst, 0 );
		dwError = GetLastError( );
		ret = ( nullptr != hHook );
	} UNLOCK( cs );

	if ( !ret ) {
		if ( !dwError ) {
			debug( L"FcEnableHook: hook was already set\n" );
		} else {
			debug( L"FcEnableHook: SetWindowsHookEx failed, error %lu\n", dwError );
		}
	}
	return ret;
#endif
}

FCHOOKDLL_API BOOL FcDisableHook( void ) {
	DWORD dwError = ERROR_SUCCESS;
	BOOL ret = FALSE;

	LOCK( cs ) {
		if ( !hHook ) {
			break;
		}

		SetLastError( ERROR_SUCCESS );
		ret = UnhookWindowsHookEx( hHook );
		dwError = GetLastError( );
		hHook = nullptr;
	} UNLOCK( cs );

	if ( !ret ) {
		if ( !dwError ) {
			debug( L"FcDisableHook: hook was not set\n" );
		} else {
			debug( L"FcDisableHook: UnhookWindowsHookEx failed, error %lu\n", dwError );
		}
	}
	return ret;
}

FCHOOKDLL_API BOOL FcIsHookEnabled( void ) {
	return ( nullptr != hHook );
}


FCHOOKDLL_API BOOL FcSetNotifyWindowHandle( HWND hwndNotifyWindow_ ) {
	if ( hwndNotifyWindow ) {
		return FALSE;
	}
	hwndNotifyWindow = hwndNotifyWindow_;
	return TRUE;
}

FCHOOKDLL_API HWND FcGetNotifyWindowHandle( void ) {
	return hwndNotifyWindow;
}

//==============================================================================
// Suite 3: Compose key and sequences
//==============================================================================

FCHOOKDLL_API BOOL FcSetComposeKey( DWORD _vkCompose ) {
	ChangeComposeKey( _vkCompose );
	return TRUE;
}

FCHOOKDLL_API DWORD FcGetComposeKey( void ) {
	return vkCompose;
}


FCHOOKDLL_API BOOL FcSetComposeSequences( ComposeSequence* pInSequences, DWORD cInSequences ) {
	if ( !pInSequences || !cInSequences ) {
		ReleaseComposeSequences( );
	} else {
		ComposeSequence* pSequences = new ComposeSequence[cInSequences];
		memcpy( pSequences, pInSequences, sizeof( ComposeSequence ) * cInSequences );
		qsort( pSequences, cInSequences, sizeof( ComposeSequence ), CompareComposeSequences );
		_SetComposeSequencesImpl( pSequences, cInSequences );
	}
	return TRUE;
}

//==============================================================================
// Suite 4: Caps Lock
//==============================================================================

FCHOOKDLL_API BOOL FcSetSwapCapsLockKey( DWORD _vkCapsLockSwap ) {
	ChangeCapsLockSwapKey( _vkCapsLockSwap );
	return TRUE;
}

FCHOOKDLL_API DWORD FcGetSwapCapsLockKey( void ) {
	return vkCapsLockSwap;
}


FCHOOKDLL_API BOOL FcSetCapsLockToggleMode( CAPS_LOCK_TOGGLE_MODE mode ) {
	clToggleMode = mode;
	ConfigureCapsLockHandling( );
	return TRUE;
}

FCHOOKDLL_API CAPS_LOCK_TOGGLE_MODE FcGetCapsLockToggleMode( void ) {
	return clToggleMode;
}


FCHOOKDLL_API BOOL FcSetCapsLockSwapMode( CAPS_LOCK_SWAP_MODE mode ) {
	clSwapMode = mode;
	ConfigureCapsLockHandling( );
	return TRUE;
}

FCHOOKDLL_API CAPS_LOCK_SWAP_MODE FcGetCapsLockSwapMode( void ) {
	return clSwapMode;
}
