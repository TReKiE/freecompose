#include "stdafx.h"
#include "FCHookDll.h"
#include "HookProc.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma data_seg( push, ".shareddata" )
volatile LONG cClients = 0;
#pragma data_seg( pop )

HINSTANCE hDllInst = NULL;

static void _SetComposeKeyEntriesImpl( COMPOSE_KEY_ENTRY* rgEntries, DWORD cEntries ) {
	LOCK( cs ) {
		if ( NULL != ComposeKeyEntries ) {
			free( ComposeKeyEntries );
		}
		ComposeKeyEntries = rgEntries;
		cComposeKeyEntries = cEntries;
	} UNLOCK( cs );
}

BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD ulReasonForCall, LPVOID /*lpReserved*/ ) {
	switch ( ulReasonForCall ) {
		case DLL_PROCESS_ATTACH:
			hDllInst = hModule;
			if ( 1 == InterlockedIncrement( &cClients ) ) {
				InitializeCriticalSection( &cs );
			}
			break;

		case DLL_PROCESS_DETACH:
			if ( 0 == InterlockedDecrement( &cClients ) ) {
				_SetComposeKeyEntriesImpl( NULL, 0 );
				DeleteCriticalSection( &cs );
			}
			break;
	}

    return TRUE;
}

//
// FC API
//

FCHOOKDLL_API DWORD FcGetApiVersion( void ) {
	return FCHOOKDLL_API_VERSION;
}

FCHOOKDLL_API BOOL FcSetComposeKeyEntries( COMPOSE_KEY_ENTRY* rgEntries, DWORD cEntries ) {
	COMPOSE_KEY_ENTRY* pcke = (COMPOSE_KEY_ENTRY*) calloc( cEntries, sizeof( COMPOSE_KEY_ENTRY ) );
	memcpy( pcke, rgEntries, sizeof( COMPOSE_KEY_ENTRY ) * cEntries );
	qsort( pcke, cEntries, sizeof( COMPOSE_KEY_ENTRY ), CompareCkes );
	_SetComposeKeyEntriesImpl( pcke, cEntries );
	return TRUE;
}

FCHOOKDLL_API BOOL FcEnableHook( void ) {
	DWORD dwError = 0;
	BOOL ret = FALSE;

	LOCK( cs ) {
		SetLastError( 0 );

		if ( NULL != hHook ) {
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
	BOOL ret;

	LOCK( cs ) {
		ret = ( NULL != hHook );
	} UNLOCK( cs );
	return ret;
}

FCHOOKDLL_API BOOL FcSetCapsLockToggleMode( CAPS_LOCK_TOGGLE_MODE mode ) {
	clToggleMode = mode;
	return TRUE;
}

FCHOOKDLL_API CAPS_LOCK_TOGGLE_MODE FcGetCapsLockToggleMode( void ) {
	return clToggleMode;
}

FCHOOKDLL_API BOOL FcSetCapsLockSwapMode( CAPS_LOCK_SWAP_MODE mode ) {
	clSwapMode = mode;
	return TRUE;
}

FCHOOKDLL_API CAPS_LOCK_SWAP_MODE FcGetCapsLockSwapMode( void ) {
	return clSwapMode;
}

FCHOOKDLL_API BOOL FcEnableSwapCapsLock( void ) {
	fSwapCapsLock = true;
	return TRUE;
}

FCHOOKDLL_API BOOL FcDisableSwapCapsLock( void ) {
	fSwapCapsLock = false;
	return TRUE;
}

FCHOOKDLL_API BOOL FcIsSwapCapsLockEnabled( void ) {
	return fSwapCapsLock;
}

FCHOOKDLL_API BOOL FcSetComposeKey( DWORD _vkCompose ) {
	vkCompose = _vkCompose;
	return TRUE;
}

FCHOOKDLL_API DWORD FcGetComposeKey( void ) {
	return vkCompose;
}

FCHOOKDLL_API BOOL FcSetSwapCapsLockKey( DWORD _vkCapsLockSwap ) {
	vkCapsLockSwap = _vkCapsLockSwap;
	return TRUE;
}

FCHOOKDLL_API DWORD FcGetSwapCapsLockKey( void ) {
	return vkCapsLockSwap;
}
