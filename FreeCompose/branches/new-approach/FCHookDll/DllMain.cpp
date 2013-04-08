#include "stdafx.h"

#include "FCHookDll.h"
#include "Common.h"
#include "HookProc.h"

#pragma data_seg( push, ".shareddata" )

volatile static LONG cClients = 0;

#pragma data_seg( pop )

BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD ulReasonForCall, LPVOID /*lpReserved*/ ) {
	switch ( ulReasonForCall ) {
		case DLL_PROCESS_ATTACH:
			hDllInst = hModule;
			if ( 1 == InterlockedIncrement( &cClients ) ) {
				InitializeCriticalSection( &cs );
				InitializeKeyEventDispatcher( );
			}
			break;

		case DLL_PROCESS_DETACH:
			if ( 0 == InterlockedDecrement( &cClients ) ) {
				ReleaseComposeSequences( );
				DeleteCriticalSection( &cs );
			}
			break;
	}

    return TRUE;
}
