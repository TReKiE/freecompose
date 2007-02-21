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
				EnterCriticalSection( &cs );
				if ( NULL != ComposeKeyEntries )
					free( ComposeKeyEntries );
				ComposeKeyEntries = NULL;
				cComposeKeyEntries = 0;
				LeaveCriticalSection( &cs );
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
	EnterCriticalSection( &cs );
	if ( NULL != ComposeKeyEntries )
		free( ComposeKeyEntries );
	ComposeKeyEntries = (COMPOSE_KEY_ENTRY*) calloc( cEntries, sizeof( COMPOSE_KEY_ENTRY ) );
	memcpy( ComposeKeyEntries, rgEntries, sizeof( COMPOSE_KEY_ENTRY ) * cEntries );
	cComposeKeyEntries = cEntries;
	LeaveCriticalSection( &cs );
	return TRUE;
}

FCHOOKDLL_API BOOL FcEnableHook( void ) {
	debug( _T( "EnableHook\n" ) );

	if ( NULL != hHook ) {
		debug( _T( "EnableHook: hook already registered!!\n" ) );
		return FALSE;
	}
	hHook = SetWindowsHookEx( WH_KEYBOARD_LL, LowLevelKeyboardProc, hDllInst, 0 );
	if ( NULL == hHook ) {
		debug( _T( "EnableHook: SetWindowsHookEx failed, %d\n" ), GetLastError( ) );
		return FALSE;
	}
	return TRUE;
}

FCHOOKDLL_API BOOL FcDisableHook( void ) {
	debug( _T( "DisableHook\n" ) );

	if ( NULL == hHook ) {
		debug( _T( "DisableHook: hook not registered!!\n" ) );
		return FALSE;
	}
	if ( ! UnhookWindowsHookEx( hHook ) ) {
		debug( _T( "DisableHook: UnhookWindowsHookEx failed, %d\n" ), GetLastError( ) );
		return FALSE;
	}
	hHook = NULL;
	return TRUE;
}

FCHOOKDLL_API DWORD FcGetApiVersion( void ) {
	return FCHOOKDLL_API_VERSION;
}
