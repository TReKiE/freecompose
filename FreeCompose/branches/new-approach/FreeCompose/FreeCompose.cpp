#include "stdafx.h"

#include "FreeCompose.h"
#include "MainFrm.h"

#include "Utils.h"

extern void InitializeDebug( void );
extern void TerminateDebug( void );

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
	debug( L"CFreeComposeApp::IsAlreadyRunning: exclusionName=\"%s\"\n", exclusionName );

	SetLastError( ERROR_SUCCESS );
	m_hInstanceMutex = CreateMutex( nullptr, TRUE, exclusionName );
	DWORD dwError = GetLastError( );
	debug( L"CFreeComposeApp::IsAlreadyRunning: after CreateMutex: m_hInstanceMutex=0x%p dwError=%lu\n", m_hInstanceMutex, dwError );
	if ( m_hInstanceMutex ) {
		if ( ERROR_SUCCESS == dwError ) {
			debug( L"Created locked mutex, so not running!\n" );
			return false;
		}
		if ( ERROR_ALREADY_EXISTS == dwError ) {
			debug( L"Mutex already exists, so already running =(\n" );
			return true;
		}
		debug( L"Uh oh. m_hInstanceMutex is set, but we didn't catch dwError?\n" );
	} else {
		if ( ERROR_ACCESS_DENIED == dwError ) {
			debug( L"CFreeComposeApp::InitInstance: CreateMutex returned ERROR_ACCESS_DENIED, so already running\n" );
			return true;
		}
		debug( L"CFreeComposeApp::InitInstance: Uh oh. m_hInstanceMutex is not set, but we didn't catch dwError?\n" );
	}
	return true;
}

BOOL CFreeComposeApp::InitInstance( ) {
	if ( IsAlreadyRunning( ) ) {
		::PostMessage( HWND_BROADCAST, APP_ACTIVATE, 0, 0 );
		return FALSE;
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls = { sizeof( InitCtrls ), ICC_WIN95_CLASSES };
	InitCommonControlsEx( &InitCtrls );

	debug( L"CFreeComposeApp::InitInstance: FreeCompose API version: host %lu, DLL %lu\n", FCHOOKDLL_API_VERSION, FcGetApiVersion( ) );
	if ( FCHOOKDLL_API_VERSION != FcGetApiVersion( ) ) {
		CString str;
		str.Format( CString( (LPCWSTR) IDS_MAINFRAME_MISMATCH_PROMPT ), FcGetApiVersion( ), FCHOOKDLL_API_VERSION );
		MessageBox( NULL, str, CString( (LPCWSTR) IDS_MAINFRAME_MISMATCH_TITLE ), MB_ICONHAND );
		// Hey, maybe we can use Windows Installer to try to repair the file!
		return FALSE;
	}

	HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
	if ( FAILED( hr ) ) {
		debug( L"CFreeComposeApp::InitInstance: CoInitializeEx failed, hr=0x%lX\n", hr );
		return FALSE;
	}
	CString str( (LPCWSTR) AFX_IDS_COMPANY_NAME );
	str.Append( L" DEBUG" );
	SetRegistryKey( str );
	CWinApp::InitInstance( );

	InitializeDebugLogFile( );

	CMainFrame* pFrame = new CMainFrame;
	if ( !pFrame ) {
		return FALSE;
	}
	m_pMainWnd = pFrame;
	pFrame->LoadFrame( IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL );
	pFrame->ShowWindow( SW_HIDE );
	pFrame->UpdateWindow( );

	return TRUE;
}

int CFreeComposeApp::ExitInstance( ) {
	TerminateDebugLogFile( );
	CloseHandle( m_hInstanceMutex );
	return CWinApp::ExitInstance( );
}
