#include "stdafx.h"

#include "HookProc.h"
#include "ComposeSequenceTree.h"

using namespace std::tr1;

//==============================================================================
// Function prototypes
//==============================================================================

static void _SetComposeSequencesImpl( COMPOSE_SEQUENCE* pSequences, INT_PTR cSequences );

//==============================================================================
// Private functions
//==============================================================================

static void _SetComposeSequencesImpl( COMPOSE_SEQUENCE* pSequences, INT_PTR cSequences ) {
	LOCK( cs ) {
		if ( NULL != ComposeSequences ) {
			delete[] ComposeSequences;
		}
		ComposeSequences = pSequences;
		cComposeSequences = cSequences;
	} UNLOCK( cs );

	ComposeSequenceTree = COMPOSE_SEQUENCE_TREE( pSequences, cSequences );
}

//==============================================================================
// Library-internal functions
//==============================================================================

void ReleaseComposeSequences( void ) {
	_SetComposeSequencesImpl( NULL, 0 );

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
// Suite 1: Hook control
//==============================================================================

FCHOOKDLL_API BOOL FcEnableHook( void ) {
#if UI_DEBUGGING_ONLY
	return FALSE;
#else
	DWORD dwError = 0;
	BOOL ret = FALSE;

	LOCK( cs ) {
		SetLastError( 0 );

		if ( hHook ) {
			break;
		}

		hHook = SetWindowsHookEx( WH_KEYBOARD_LL, LowLevelKeyboardProc, hDllInst, 0 );
		ret = ( NULL != hHook );
		dwError = GetLastError( );
	} UNLOCK( cs );

	if ( ! ret ) {
		if ( ! dwError ) {
			debug( L"FcEnableHook: hook was already set\n" );
		} else {
			debug( L"FcEnableHook: SetWindowsHookEx failed, error %u\n", dwError );
		}
	}
	return ret;
#endif
}

FCHOOKDLL_API BOOL FcDisableHook( void ) {
	DWORD dwError = 0;
	BOOL ret = FALSE;

	LOCK( cs ) {
		SetLastError( 0 );

		if ( NULL == hHook ) {
			break;
		}

		ret = UnhookWindowsHookEx( hHook );
		dwError = GetLastError( );
		hHook = NULL;
	} UNLOCK( cs );

	if ( ! ret ) {
		if ( ! dwError ) {
			debug( L"FcDisableHook: hook was not set\n" );
		} else {
			debug( L"FcDisableHook: UnhookWindowsHookEx failed, error %u\n", dwError );
		}
	}
	return ret;
}

FCHOOKDLL_API BOOL FcIsHookEnabled( void ) {
	return ( NULL != hHook );
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
// Suite 2: Compose key and sequences
//==============================================================================

FCHOOKDLL_API BOOL FcSetComposeKey( DWORD _vkCompose ) {
	ChangeComposeKey( _vkCompose );
	return TRUE;
}

FCHOOKDLL_API DWORD FcGetComposeKey( void ) {
	return vkCompose;
}


FCHOOKDLL_API BOOL FcSetComposeSequences( COMPOSE_SEQUENCE* pInSequences, DWORD cInSequences ) {
	if ( NULL == pInSequences || 0 == cInSequences ) {
		ReleaseComposeSequences( );
	} else {
		COMPOSE_SEQUENCE* pSequences = new COMPOSE_SEQUENCE[cInSequences];
		memcpy( pSequences, pInSequences, sizeof( COMPOSE_SEQUENCE ) * cInSequences );
		qsort( pSequences, cInSequences, sizeof( COMPOSE_SEQUENCE ), CompareComposeSequences );
		_SetComposeSequencesImpl( pSequences, cInSequences );
	}
	return TRUE;
}

//==============================================================================
// Suite 3: Caps Lock
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
