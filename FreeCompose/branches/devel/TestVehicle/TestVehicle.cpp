#include "stdafx.h"
#include "TestVehicle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;
HMODULE hModule;

using namespace std;

int wmain( int /*argc*/, wchar_t* /*argv*/[], wchar_t* /*envp*/[] ) {
	hModule = ::GetModuleHandle( NULL );
	if ( !hModule || !AfxWinInit( hModule, NULL, ::GetCommandLine( ), 0 ) ) {
		fwprintf( stderr, L"Initialization failed\n" );
		return 1;
	}

	return 0;
}
