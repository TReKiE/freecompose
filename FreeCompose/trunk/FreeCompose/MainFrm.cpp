// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "FreeCompose.h"
#include "MainFrm.h"
#include "ComposeDefaults.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT FCM_NOTIFYICON = RegisterWindowMessage( _T("FreeCompose.FCM_NOTIFYICON") );
const UINT FCM_ENABLE     = RegisterWindowMessage( _T("FreeCompose.FCM_ENABLE") );
const UINT FCM_DISABLE    = RegisterWindowMessage( _T("FreeCompose.FCM_DISABLE") );

const UINT FCM_PIP        = RegisterWindowMessage( _T("FcHookDll.FCM_PIP") );
const UINT FCM_KEY        = RegisterWindowMessage( _T("FcHookDll.FCM_KEY") );

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_REGISTERED_MESSAGE(FCM_NOTIFYICON, &CMainFrame::OnFcmNotifyIcon)
	ON_REGISTERED_MESSAGE(FCM_ENABLE, &CMainFrame::OnFcmEnable)
	ON_REGISTERED_MESSAGE(FCM_DISABLE, &CMainFrame::OnFcmDisable)
	ON_REGISTERED_MESSAGE(FCM_PIP, &CMainFrame::OnFcmPip)
	ON_REGISTERED_MESSAGE(FCM_KEY, &CMainFrame::OnFcmKey)
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ptni = NULL;
	m_fActive = FALSE;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	debug( _T("CMainFrame::OnCreate: DLL %d host %d\n"), FcGetApiVersion( ), FCHOOKDLL_API_VERSION );
	if ( FCHOOKDLL_API_VERSION != FcGetApiVersion( ) ) {
		CString str;
		str.Format( _T(" FC API version mismatch: DLL %d vs. host %d" ), FcGetApiVersion( ), FCHOOKDLL_API_VERSION );
		MessageBox( (LPCTSTR) str, _T( "Uh oh" ), MB_ICONHAND );
		// Hey, maybe we can use Windows Installer to try to repair the file!
		return FALSE;
	}

	_FcLoadKeys( );
	if ( 0 == ComposeKeyEntries.GetCount() ) {
		LONG n = 0;
		while (
			DefaultComposeKeyEntries[n].vkFirst     != 0 &&
			DefaultComposeKeyEntries[n].vkSecond    != 0 &&
			DefaultComposeKeyEntries[n].wchComposed != 0
		) {
			ComposeKeyEntries.Add( DefaultComposeKeyEntries[n++] );
		}
		_FcSaveKeys( );
	}
	FcSetComposeKeyEntries( ComposeKeyEntries.GetData( ), (DWORD) ComposeKeyEntries.GetCount( ) );

	if ( (BOOL) theApp.GetProfileInt( _T("Startup"), _T("StartActive"), (int) TRUE ) ) {
		FcEnableHook( );
		m_fActive = TRUE;
	}

	OnTaskbarCreated(0, 0);

	return 0;
}


// CMainFrame message handlers

LRESULT CMainFrame::OnTaskbarCreated(WPARAM /*wParam*/, LPARAM /*lParam*/) {
	m_ptni = new CTrayNotifyIcon();
	m_ptni->Create( this, 1, ( m_fActive ? _T("FreeCompose is enabled.") : _T("FreeCompose is disabled.") ), m_hIcon, FCM_NOTIFYICON, IDR_POPUP_ACTIVE );

	return 0;
}

LRESULT CMainFrame::OnFcmNotifyIcon(WPARAM wParam, LPARAM lParam) {
	return m_ptni->OnTrayNotification( wParam, lParam );
}

LRESULT CMainFrame::OnFcmEnable(WPARAM /*wParam*/, LPARAM /*lParam*/) {
	if ( ! m_fActive ) {
		FcEnableHook( );
		m_fActive = TRUE;
		m_ptni->SetMenu( LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE( IDR_POPUP_ACTIVE ) ) );
		m_ptni->SetTooltipText( _T("FreeCompose is enabled.") );
		m_ptni->SetBalloonDetails( _T("FreeCompose is enabled."), _T("FreeCompose"), CTrayNotifyIcon::Info, 10 );
	}
	return 0;
}

LRESULT CMainFrame::OnFcmDisable(WPARAM /*wParam*/, LPARAM /*lParam*/) {
	if ( m_fActive ) {
		FcDisableHook();
		m_fActive = FALSE;
		m_ptni->SetMenu( LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE( IDR_POPUP_INACTIVE ) ) );
		m_ptni->SetTooltipText( _T("FreeCompose is disabled.") );
		m_ptni->SetBalloonDetails( _T("FreeCompose is disabled."), _T("FreeCompose"), CTrayNotifyIcon::Info, 10 );
	}
	return 0;
}

LRESULT CMainFrame::OnFcmPip(WPARAM wPip, LPARAM /*lParam*/) {
	switch ( wPip ) {
		case PIP_OK_1:
			::Beep( 523, 100 );
			break;

		case PIP_OK_2:
			::Beep( 523, 100 );
			break;

		case PIP_OK_3:
			::Beep( 1047, 100 );
			break;

		case PIP_ABORT:
			MessageBeep( MB_ICONASTERISK );
			break;

		case PIP_ERROR:
			MessageBeep( MB_ICONHAND );
			break;

		case PIP_FAIL:
			MessageBeep( MB_ICONHAND );
			break;

		default:
			break;
	}

	return 0;
}

LRESULT CMainFrame::OnFcmKey(WPARAM /*wKey*/, LPARAM /*lParam*/) {
	return 0;
}

void CMainFrame::OnClose() {
	OnFcmDisable( 0, 0 );
	delete m_ptni;
	m_ptni = NULL;
	CFrameWnd::OnClose();
}
