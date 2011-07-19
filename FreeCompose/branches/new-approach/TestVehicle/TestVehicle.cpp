#include "stdafx.h"
#include "TestVehicle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;
HMODULE hModule;

using namespace std;

extern void BitsetTests( void );

int wmain( void ) {
	hModule = ::GetModuleHandle( NULL );
	if ( ! hModule || ! AfxWinInit( hModule, NULL, ::GetCommandLine( ), 0 ) ) {
		wcerr << L"Initialization failed!" << endl;
		return 1;
	}

	BitsetTests( );

	if ( IsDebuggerPresent( ) ) {
		wcout << L"Press ENTER...";
		wcout.flush( );
		wchar_t junk[5];
		wcin.getline( junk, 5 );
	}

	return 0;
}
