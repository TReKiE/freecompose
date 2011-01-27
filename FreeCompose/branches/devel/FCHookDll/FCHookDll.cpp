// FCHookDll.cpp : Defines the entry point for the DLL application.

#include "stdafx.h"
#include "FCHookDll.h"
#include "HookProc.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma data_seg( push, ".shareddata" )
int cClients = 0;
#pragma data_seg( pop )

HINSTANCE hDllInst = NULL;

BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD ulReasonForCall, LPVOID /*lpReserved*/ ) {
	debug( _T( "DllMain: hModule=%08x ulReasonForCall=%d\n" ), hModule, ulReasonForCall );

	switch ( ulReasonForCall ) {
		case DLL_PROCESS_ATTACH:
			hDllInst = hModule;
			cClients++;
			if ( 1 == cClients )
				InitializeCriticalSection( &cs );
			break;

		case DLL_PROCESS_DETACH:
			cClients--;
			if ( 0 == cClients ) {
				LOCK( cs ) {
					if ( NULL != ComposeKeyEntries )
						free( ComposeKeyEntries );
					ComposeKeyEntries = NULL;
					cComposeKeyEntries = 0;
				} UNLOCK( cs );
				DeleteCriticalSection( &cs );
			}
			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}

    return TRUE;
}

FCHOOKDLL_API BOOL FcSetComposeKeyEntries( COMPOSE_KEY_ENTRY* rgEntries, DWORD cEntries ) {
	COMPOSE_KEY_ENTRY* pcke = (COMPOSE_KEY_ENTRY*) calloc( cEntries, sizeof( COMPOSE_KEY_ENTRY ) );
	memcpy( pcke, rgEntries, sizeof( COMPOSE_KEY_ENTRY ) * cEntries );
	qsort( pcke, cEntries, sizeof( COMPOSE_KEY_ENTRY ), CompareCkes );

	LOCK( cs ) {
		if ( NULL != ComposeKeyEntries )
			free( ComposeKeyEntries );
		ComposeKeyEntries = pcke;
		cComposeKeyEntries = cEntries;
	} UNLOCK( cs );
	return TRUE;
}

FCHOOKDLL_API BOOL FcEnableHook( void ) {
	BOOL ret = TRUE;

	debug( _T( "EnableHook\n" ) );
	LOCK( cs ) {
		if ( NULL != hHook ) {
			debug( _T( "EnableHook: hook already registered!!\n" ) );
			ret = FALSE;
			goto out;
		}
		hHook = SetWindowsHookEx( WH_KEYBOARD_LL, LowLevelKeyboardProc, hDllInst, 0 );
		if ( NULL == hHook ) {
			debug( _T( "EnableHook: SetWindowsHookEx failed, %d\n" ), GetLastError( ) );
			ret = FALSE;
			goto out;
		}
out:	;
	} UNLOCK( cs );
	return ret;
}

FCHOOKDLL_API BOOL FcDisableHook( void ) {
	BOOL ret = TRUE;

	debug( _T( "DisableHook\n" ) );
	LOCK( cs ) {
		if ( NULL == hHook ) {
			debug( _T( "DisableHook: hook not registered!!\n" ) );
			ret = FALSE;
			goto out;
		}
		if ( ! UnhookWindowsHookEx( hHook ) ) {
			debug( _T( "DisableHook: UnhookWindowsHookEx failed, %d\n" ), GetLastError( ) );
			ret = FALSE;
			goto out;
		}
		hHook = NULL;
out:	;
	} UNLOCK( cs );
	return ret;
}

FCHOOKDLL_API DWORD FcGetApiVersion( void ) {
	return FCHOOKDLL_API_VERSION;
}

FCHOOKDLL_API BOOL FcEnableCapsLock( void ) {
	//LOCK( cs ) {
		fDisableCapsLock = false;
	//} UNLOCK( cs );
	return TRUE;
}

FCHOOKDLL_API BOOL FcDisableCapsLock( void ) {
	//LOCK( cs ) {
		fDisableCapsLock = true;
	//} UNLOCK( cs );
	return TRUE;
}
