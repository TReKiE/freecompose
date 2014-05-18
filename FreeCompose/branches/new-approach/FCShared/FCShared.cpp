#include "stdafx.h"
#include "FCShared.h"
#include "Internal.h"

//BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD ulReasonForCall, LPVOID /*lpReserved*/ ) {
//	switch ( ulReasonForCall ) {
//		case DLL_PROCESS_ATTACH:
//			//hDllInst = hModule;
//			break;
//	}
//
//    return TRUE;
//}

LPWSTR _CoTaskMemDuplicateString( LPCWSTR lpwszSource ) {
	size_t cch = wcslen( lpwszSource ) + 1;
	size_t cb = sizeof( wchar_t ) * cch;
	LPWSTR lpwsz = static_cast<LPWSTR>( CoTaskMemAlloc( cb ) );
	if ( lpwsz ) {
		wcscpy_s( lpwsz, cch, lpwszSource );
	}
	return lpwsz;
}
