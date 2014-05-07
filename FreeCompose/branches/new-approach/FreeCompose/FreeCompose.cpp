#include "stdafx.h"

#include "FreeCompose.h"
#include "MainFrm.h"

#include "AppSoundsRegistry.h"
#include "Utils.h"

BEGIN_MESSAGE_MAP( CFreeComposeApp, CWinApp )
	//{{AFX_MSG_MAP( CFreeComposeApp )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )

CFreeComposeApp theApp;

CFreeComposeApp::CFreeComposeApp( ):
	m_hInstanceMutex( nullptr )
{

}

bool CFreeComposeApp::IsAlreadyRunning( ) {
	CString exclusionName( MakeInstanceExclusionName( CString( L"ca.zive.FreeCompose.instanceMutex" ), UNIQUE_TO_SESSION ) );

	SetLastError( ERROR_SUCCESS );
	m_hInstanceMutex = CreateMutex( nullptr, TRUE, exclusionName );
	DWORD dwError = GetLastError( );
	if ( m_hInstanceMutex ) {
		if ( ERROR_SUCCESS == dwError ) {
			return false;
		}
		if ( ERROR_ALREADY_EXISTS == dwError ) {
			return true;
		}
	} else {
		if ( ERROR_ACCESS_DENIED == dwError ) {
			return true;
		}
	}
	debug( L"CFreeComposeApp::IsAlreadyRunning: Something went wrong. m_hInstanceMutex=0x%p dwError=%lu\n", m_hInstanceMutex, dwError );
	return true;
}

BOOL CFreeComposeApp::InitInstance( ) {
	if ( !IsWindowsVersionOrGreater( WindowsMajorVersion, WindowsMinorVersion, WindowsServicePackMajor ) ) {
		debug( L"CFreeComposeApp::InitInstance: Windows version too low\n" );
		MessageBox( nullptr, LoadFromStringTable( IDS_APP_WINVER_TOO_LOW ), LoadFromStringTable( AFX_IDS_APP_TITLE ), MB_ICONERROR | MB_OK );
		return FALSE;
	}

	// If there's already another instance running within this session, activate it instead.
	if ( IsAlreadyRunning( ) ) {
		::PostMessage( HWND_BROADCAST, APP_ACTIVATE, 0, 0 );
		return FALSE;
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls = { sizeof( InitCtrls ), ICC_WIN95_CLASSES };
	InitCommonControlsEx( &InitCtrls );

	// Initialize COM
	HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
	if ( FAILED( hr ) ) {
		debug( L"CFreeComposeApp::InitInstance: CoInitializeEx failed, hr=0x%lX\n", hr );
		return FALSE;
	}

	// Store Common Controls (ComCtl32.dll) version for later use
	GetComCtl32Version( );

	// Configure MFC
	SetRegistryKey( LoadFromStringTable( AFX_IDS_COMPANY_NAME ) );

	// Initialize MFC
	CWinApp::InitInstance( );

	// Initialize the debug log
	InitializeDebugLogFile( );

	// Check FCHookDLL's API version against the version we were built with
	debug( L"CFreeComposeApp::InitInstance: FreeCompose API version: host %lu, DLL %lu\n", FCHOOKDLL_API_VERSION, FcGetApiVersion( ) );
	if ( FCHOOKDLL_API_VERSION != FcGetApiVersion( ) ) {
		CString str;
		str.Format( LoadFromStringTable( IDS_MAINFRAME_MISMATCH_PROMPT ), FcGetApiVersion( ), FCHOOKDLL_API_VERSION );
		MessageBox( nullptr, str, LoadFromStringTable( IDS_MAINFRAME_MISMATCH_TITLE ), MB_ICONHAND );
		// Hey, maybe we can use Windows Installer to try to repair the file!
		return FALSE;
	}

	// Initialize FCHookDLL
	if ( !FcInitialize( ) ) {
		debug( L"CFreeComposeApp::InitInstance: FcInitialize failed\n" );
		return FALSE;
	}

	CAppSoundsRegistry::RegisterFcAppSounds( );

	// Create and (not) show our hidden window
	CMainFrame* pFrame = new CMainFrame;
	if ( !pFrame ) {
		return FALSE;
	}

	m_pMainWnd = pFrame;
	pFrame->LoadFrame( IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr, nullptr );
	pFrame->ShowWindow( SW_HIDE );
	pFrame->UpdateWindow( );

	// DONE
	return TRUE;
}

int CFreeComposeApp::ExitInstance( ) {
	FcUninitialize( );
	TerminateDebugLogFile( );

	return CWinApp::ExitInstance( );
}
