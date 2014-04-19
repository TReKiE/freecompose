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

bool CFreeComposeApp::IsAlreadyRunning( ) {
	SetLastError( 0 );
	m_hInstanceMutex = CreateMutex( nullptr, TRUE, MakeInstanceExclusionName( CString( L"ca.zive.FreeCompose.instanceMutex" ), UNIQUE_TO_SESSION ) );
	DWORD dwError = GetLastError( );
	if ( ERROR_ALREADY_EXISTS == dwError ) {
		debug( L"CFreeComposeApp::InitInstance: CreateMutex(\"ca.zive.FreeCompose.instanceMutex\") returned ERROR_ALREADY_EXISTS, exiting\n" );
		return true;
	} else if ( ERROR_ACCESS_DENIED == dwError ) {
		debug( L"CFreeComposeApp::InitInstance: CreateMutex(\"ca.zive.FreeCompose.instanceMutex\") returned ERROR_ACCESS_DENIED, exiting\n" );
		return true;
	}
	return false;
}

BOOL CFreeComposeApp::InitInstance( ) {
	if ( IsAlreadyRunning( ) ) {
		return FALSE;
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls = { sizeof( InitCtrls ), ICC_WIN95_CLASSES };
	InitCommonControlsEx( &InitCtrls );

	debug( L"CFreeComposeApp::InitInstance: FreeCompose API version: host %d, DLL %d\n", FCHOOKDLL_API_VERSION, FcGetApiVersion( ) );
	if ( FCHOOKDLL_API_VERSION != FcGetApiVersion( ) ) {
		CString str;
		str.Format( CString( (LPCWSTR) IDS_MAINFRAME_MISMATCH_PROMPT ), FcGetApiVersion( ), FCHOOKDLL_API_VERSION );
		MessageBox( NULL, str, CString( (LPCWSTR) IDS_MAINFRAME_MISMATCH_TITLE ), MB_ICONHAND );
		// Hey, maybe we can use Windows Installer to try to repair the file!
		return FALSE;
	}

	CoInitializeEx( NULL, COINIT_MULTITHREADED );
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
