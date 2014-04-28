#include "stdafx.h"

#ifndef _AFXDLL
BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD ulReasonForCall, LPVOID /*lpReserved*/ ) {
	switch ( ulReasonForCall ) {
		case DLL_PROCESS_ATTACH:
			hDllInst = hModule;
			break;

		case DLL_PROCESS_DETACH:
			hDllInst = nullptr;
			break;
	}

    return TRUE;
}
#endif
