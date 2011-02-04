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


// CFreeComposeApp

BEGIN_MESSAGE_MAP( CFreeComposeApp, CWinApp )
	//{{AFX_MSG_MAP( COptionsPropSheet )
	ON_COMMAND(ID_APP_EXIT, &CFreeComposeApp::OnAppExit)
	//}}AFX_MSG_MAP
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
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	SetRegistryKey(_T("Zive Technology Research"));

	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

	pFrame->ShowWindow(SW_HIDE);
	pFrame->UpdateWindow();

	return TRUE;
}
