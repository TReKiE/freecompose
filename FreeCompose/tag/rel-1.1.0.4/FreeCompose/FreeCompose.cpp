#include "stdafx.h"
#include "FreeCompose.h"
#include "MainFrm.h"
#include "Utils.h"

#include <initguid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern void InitializeDebug( void );
extern void TerminateDebug( void );

BEGIN_MESSAGE_MAP( CFreeComposeApp, CWinApp )
	//{{AFX_MSG_MAP( COptionsPropSheet )
	ON_COMMAND(ID_APP_EXIT, &CFreeComposeApp::OnAppExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )

CFreeComposeApp theApp;

CFreeComposeApp::CFreeComposeApp( ) {
}

BOOL CFreeComposeApp::InitInstance( ) {
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx( &InitCtrls );

	debug( _T("CFreeComposeApp::InitInstance: DLL %d host %d\n"), FcGetApiVersion( ), FCHOOKDLL_API_VERSION );
	if ( FCHOOKDLL_API_VERSION != FcGetApiVersion( ) ) {
		CString str;
		str.Format( CString( (LPCWSTR) IDS_MAINFRAME_MISMATCH_PROMPT ), FcGetApiVersion( ), FCHOOKDLL_API_VERSION );
		MessageBox( NULL, str, CString( (LPCWSTR) IDS_MAINFRAME_MISMATCH_TITLE ), MB_ICONHAND );
		// Hey, maybe we can use Windows Installer to try to repair the file!
		return FALSE;
	}

	CoInitializeEx( NULL, COINIT_MULTITHREADED );
	SetRegistryKey( AFX_IDS_COMPANY_NAME );
	InitializeDebug( );

	CWinApp::InitInstance( );

	CMainFrame* pFrame = new CMainFrame;
	if ( ! pFrame ) {
		return FALSE;
	}
	m_pMainWnd = pFrame;
	pFrame->LoadFrame( IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL );
	pFrame->ShowWindow( SW_HIDE );
	pFrame->UpdateWindow( );

	return TRUE;
}

int CFreeComposeApp::ExitInstance( ) {
	TerminateDebug( );
	return CWinApp::ExitInstance( );
}
