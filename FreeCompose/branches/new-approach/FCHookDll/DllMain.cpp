#include "stdafx.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

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
