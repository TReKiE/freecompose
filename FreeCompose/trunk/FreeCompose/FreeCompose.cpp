// FreeCompose.cpp : Defines the class behaviors for the application.

#include "stdafx.h"
#include "FreeCompose.h"
#include "MainFrm.h"

#include <initguid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only CFreeComposeApp object

CFreeComposeApp theApp;
CArray< COMPOSE_KEY_ENTRY > ComposeKeyEntries;


// CFreeComposeApp

BEGIN_MESSAGE_MAP( CFreeComposeApp, CWinApp )
	ON_COMMAND(ID_APP_EXIT, &CFreeComposeApp::OnAppExit)
END_MESSAGE_MAP( )

// CFreeComposeApp construction

CFreeComposeApp::CFreeComposeApp() {
}

// CFreeComposeApp initialization

BOOL CFreeComposeApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// App was launched with /Embedding or /Automation switch.
	// Run app as automation server.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Don't show the main window
		return TRUE;
	}
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Zive Technology Research"));
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_HIDE);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

// CFreeComposeApp message handlers

void _FcLoadKeys( void ) {
	int lim = theApp.GetProfileInt( _T("Mapping"), _T("Count"), 0 );

	ComposeKeyEntries.RemoveAll( );
	ComposeKeyEntries.SetSize( lim );

	COMPOSE_KEY_ENTRY cke;
	CString section;
	for ( int n = 0; n < lim; n++ ) {
		section.Format( _T("Mapping\\%d"), n );
		cke.vkFirst     = (DWORD)   theApp.GetProfileInt( section, _T("First"),    0 );
		cke.vkSecond    = (DWORD)   theApp.GetProfileInt( section, _T("Second"),   0 );
		cke.wchComposed = (wchar_t) theApp.GetProfileInt( section, _T("Composed"), 0 );
		ComposeKeyEntries[n] = cke;
	}
}

void _FcSaveKeys( void ) {
	TCHAR tszSection[32];

	theApp.DelRegTree( theApp.GetAppRegistryKey( ), CString( _T("Mapping") ) );
	theApp.WriteProfileInt( _T("Mapping"), _T("Count"), ComposeKeyEntries.GetCount( ) );
	for ( LONG n = 0; n < ComposeKeyEntries.GetCount( ); n++ ) {
		_sntprintf_s( tszSection, 32, _TRUNCATE, _T("Mapping\\%d"), n );
		theApp.WriteProfileInt( tszSection, _T("First"),    (int) ComposeKeyEntries[n].vkFirst     );
		theApp.WriteProfileInt( tszSection, _T("Second"),   (int) ComposeKeyEntries[n].vkSecond    );
		theApp.WriteProfileInt( tszSection, _T("Composed"), (int) ComposeKeyEntries[n].wchComposed );
	}
}
