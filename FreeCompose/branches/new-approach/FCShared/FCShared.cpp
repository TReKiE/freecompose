#include "stdafx.h"
#include "FCShared.h"
#include "Internal.h"

HINSTANCE hDllInst = NULL;

BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD ulReasonForCall, LPVOID /*lpReserved*/ ) {
	switch ( ulReasonForCall ) {
		case DLL_PROCESS_ATTACH:
			hDllInst = hModule;
			break;
	}

    return TRUE;
}
