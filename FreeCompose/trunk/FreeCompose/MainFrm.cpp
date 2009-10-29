// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"

#include "FreeCompose.h"
#include "MainFrm.h"
#include "AboutDlg.h"
#include "OptionsPropSheet.h"

#include "ComposeDefaults.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT FCM_NOTIFYICON = RegisterWindowMessage( _T("FreeCompose.FCM_NOTIFYICON") );
const UINT FCM_PIP        = RegisterWindowMessage( _T("FcHookDll.FCM_PIP") );

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP( COptionsPropSheet )
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_REGISTERED_MESSAGE (FCM_NOTIFYICON,   &CMainFrame::OnNotifyIcon)
	ON_REGISTERED_MESSAGE (FCM_PIP,          &CMainFrame::OnFcmPip)
	ON_COMMAND            (ID_APP_ABOUT,     &CMainFrame::OnAppAbout)
	ON_COMMAND            (ID_APP_DISABLE,   &CMainFrame::OnAppDisable)
	ON_COMMAND            (ID_APP_ENABLE,    &CMainFrame::OnAppEnable)
	ON_COMMAND            (ID_APP_CAPSLOCK,  &CMainFrame::OnAppCapsLock)
	ON_COMMAND            (ID_APP_CONFIGURE, &CMainFrame::OnAppConfigure)
	ON_UPDATE_COMMAND_UI  (ID_APP_DISABLE,   &CMainFrame::OnUpdateAppEnable)
	ON_UPDATE_COMMAND_UI  (ID_APP_ENABLE,    &CMainFrame::OnUpdateAppEnable)
	ON_UPDATE_COMMAND_UI  (ID_APP_CAPSLOCK,  &CMainFrame::OnUpdateAppCapsLock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame( ):
	m_ptni    ( NULL  ),
	m_fActive ( FALSE )
{
}

CMainFrame::~CMainFrame( ) {
}

void CMainFrame::_InitializeHook( void ) {
	_FcLoadKeys( );
	if ( 0 == ComposeKeyEntries.GetCount() ) {
		for ( int n = 0; n < countof( DefaultComposeKeyEntries ); n++ ) {
			ComposeKeyEntries.Add( DefaultComposeKeyEntries[n] );
		}
		_FcSaveKeys( );
	}
	FcSetComposeKeyEntries( ComposeKeyEntries.GetData( ), (DWORD) ComposeKeyEntries.GetCount( ) );

	if ( (BOOL) theApp.GetProfileInt( _T("Startup"), _T("StartActive"), (int) TRUE ) ) {
		FcEnableHook( );
		m_fActive = TRUE;
	}
	if ( (BOOL) theApp.GetProfileInt( _T("Features"), _T("DisableCapsLock"), (int) FALSE ) ) {
		FcDisableCapsLock( );
	}
}

void CMainFrame::_SetupTrayIcon( void ) {
	m_ptni = new CTrayNotifyIcon();
	m_ptni->Create(
		this,
		1,
		m_fActive ? _T("FreeCompose is enabled.") : _T("FreeCompose is disabled."),
		AfxGetApp( )->LoadIcon( IDR_MAINFRAME ),
		FCM_NOTIFYICON,
		IDR_POPUP
	);
}

// CMainFrame message handlers

int CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct ) {
	if ( -1 == CFrameWnd::OnCreate( lpCreateStruct ) ) {
		return -1;
	}

	debug( _T("CMainFrame::OnCreate: DLL %d host %d\n"), FcGetApiVersion( ), FCHOOKDLL_API_VERSION );
	if ( FCHOOKDLL_API_VERSION != FcGetApiVersion( ) ) {
		CString str;
		str.Format( _T(" FC API version mismatch: DLL %d vs. host %d" ), FcGetApiVersion( ), FCHOOKDLL_API_VERSION );
		MessageBox( (LPCTSTR) str, _T( "Uh oh" ), MB_ICONHAND );
		// Hey, maybe we can use Windows Installer to try to repair the file!
		return FALSE;
	}

	_InitializeHook( );
	_SetupTrayIcon( );

	return 0;
}

void CMainFrame::OnClose( ) {
	if ( m_fActive ) {
		FcDisableHook( );
		m_fActive = FALSE;
	}
	delete m_ptni;
	CFrameWnd::OnClose( );
}

LRESULT CMainFrame::OnNotifyIcon(WPARAM wParam, LPARAM lParam) {
	return m_ptni->OnTrayNotification( wParam, lParam );
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

void CMainFrame::OnAppAbout(void) {
	CAboutDlg aboutDlg;
	aboutDlg.DoModal( );
}

void CMainFrame::OnAppEnable( void ) {
	m_fActive = TRUE;
	FcEnableHook( );
	m_ptni->SetTooltipText( _T("FreeCompose is enabled.") );
	m_ptni->SetBalloonDetails( _T("FreeCompose is enabled."), _T("FreeCompose"), CTrayNotifyIcon::Info, 10 );
}

void CMainFrame::OnAppDisable( void ) {
	m_fActive = FALSE;
	FcDisableHook();
	m_ptni->SetTooltipText( _T("FreeCompose is disabled.") );
	m_ptni->SetBalloonDetails( _T("FreeCompose is disabled."), _T("FreeCompose"), CTrayNotifyIcon::Info, 10 );
}

void CMainFrame::OnAppCapsLock( void ) {
	bool fCaps = ! IsCapsLock( );

	BYTE keys[256];
	GetKeyboardState( keys );
	keys[VK_CAPITAL] = ( keys[VK_CAPITAL] & ~1 ) | ( fCaps ? 1 : 0 );
	SetKeyboardState( keys );
}

void CMainFrame::OnAppConfigure( ) {
	COptionsPropSheet options( this );
	options.DoModal( );
}

void CMainFrame::OnUpdateAppEnable( CCmdUI* pui ) {
	if ( m_fActive ) {
		pui->SetText( _T("&Disable") );
	} else {
		pui->SetText( _T("&Enable") );
	}
}

void CMainFrame::OnUpdateAppCapsLock( CCmdUI* pui ) {
	pui->SetCheck( IsCapsLock( ) ? 1 : 0 );
	pui->Enable( m_fActive && (BOOL) theApp.GetProfileInt( _T("Features"), _T("DisableCapsLock"), (int) FALSE ) );
}
