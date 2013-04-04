#include "stdafx.h"

#include "FreeCompose.h"
#include "MainFrm.h"
#include "AboutDlg.h"
#include "OptionsPropSheet.h"
#include "NTray.h"

#include "Utils.h"

//
// Registered window messages
//

const UINT APP_NOTIFYICON = RegisterWindowMessage( L"FreeCompose.APP_NOTIFYICON" );
const UINT FCM_PIP        = RegisterWindowMessage( L"FcHookDll.FCM_PIP" );
const UINT FCM_KEY        = RegisterWindowMessage( L"FcHookDll.FCM_KEY" );

//
// Message map for CMainFrame
//

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP( CMainFrame )
	ON_WM_CREATE()
	ON_WM_CLOSE()
#ifdef USE_TIMER
	ON_WM_TIMER()
#endif
	ON_REGISTERED_MESSAGE (APP_NOTIFYICON,   &CMainFrame::OnNotifyIcon)
	ON_REGISTERED_MESSAGE (APP_RECONFIGURE,  &CMainFrame::OnReconfigure)
	ON_REGISTERED_MESSAGE (FCM_PIP,          &CMainFrame::OnFcmPip)
	ON_COMMAND            (ID_APP_ABOUT,     &CMainFrame::OnAppAbout)
	ON_COMMAND            (ID_APP_TOGGLE,    &CMainFrame::OnAppToggle)
	ON_COMMAND            (ID_APP_CAPSLOCK,  &CMainFrame::OnAppCapsLock)
	ON_COMMAND            (ID_APP_CONFIGURE, &CMainFrame::OnAppConfigure)
	ON_COMMAND            (ID_APP_EXIT,      &CMainFrame::OnAppExit)
	ON_UPDATE_COMMAND_UI  (ID_APP_TOGGLE,    &CMainFrame::OnUpdateAppToggle)
	ON_UPDATE_COMMAND_UI  (ID_APP_CAPSLOCK,  &CMainFrame::OnUpdateAppCapsLock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
// Local functions
//

static bool IsCapsLock( void ) {
	return ( GetKeyState( VK_CAPITAL ) & 1 ) != 0;
}

//
// CMainFrame implementatin
//

CMainFrame::CMainFrame( ):
#ifdef USE_TIMER
	m_uTimerId    ( 0 ),
#endif
	m_pTrayIcon   ( NULL ),
	m_pOptions    ( NULL ),
	m_strTitle    ( (LPCWSTR) AFX_IDS_APP_TITLE ),
	m_strEnabled  ( (LPCWSTR) IDS_MAINFRAME_ENABLED ),
	m_strDisabled ( (LPCWSTR) IDS_MAINFRAME_DISABLED ),
	m_pPropSheet  ( NULL )
{
	InitializeCriticalSection( &m_csPropSheet );
}

CMainFrame::~CMainFrame( ) {
	if ( m_pOptions ) {
		delete m_pOptions;
	}
	DeleteCriticalSection( &m_csPropSheet );
}

void CMainFrame::_Initialize( void ) {
#ifdef USE_TIMER
	m_uTimerId = SetTimer( 1, 1000, NULL );
	debug( L"CMainFrame::_Initialize: new timer ID is %u\n", m_uTimerId );
#endif

	m_pOptions = new COptionsData;
	_Reconfigure( );

	_SetupTrayIcon( );
}

void CMainFrame::_Reconfigure( void ) {
	FcSetComposeSequences( m_pOptions->m_ComposeSequences.GetData( ), m_pOptions->m_ComposeSequences.GetCount( ) );

	FcSetCapsLockToggleMode( m_pOptions->m_CapsLockToggleMode );

	FcSetCapsLockSwapMode( m_pOptions->m_CapsLockSwapMode );
	FcSetSwapCapsLockKey( m_pOptions->m_vkSwapCapsLock );

	FcSetComposeKey( m_pOptions->m_vkCompose );

	if ( m_pOptions->m_fStartActive ) {
		FcEnableHook( );
	} else {
		FcDisableHook( );
	}
}

void CMainFrame::_SetupTrayIcon( void ) {
	m_pTrayIcon = new CTrayNotifyIcon;
	m_pTrayIcon->Create(
		this,
		1,
		FcIsHookEnabled( ) ? m_strEnabled : m_strDisabled,
		AfxGetApp( )->LoadIcon( IDR_MAINFRAME ),
		APP_NOTIFYICON,
		IDM_TRAY_MENU
	);
}

// CMainFrame message handlers

void CMainFrame::OnAppExit( ) {
	bool fPropSheetOpen;
	LOCK( m_csPropSheet ) {
		MemoryBarrier( );
		fPropSheetOpen = ( NULL != m_pPropSheet );
	} UNLOCK( m_csPropSheet );
	if ( fPropSheetOpen ) {
		MessageBox( L"Please close the FreeCompose options dialog before exiting FreeCompose.", L"FreeCompose", MB_ICONWARNING|MB_OK );
	} else {
		OnClose( );
	}
}

int CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct ) {
	if ( -1 == CFrameWnd::OnCreate( lpCreateStruct ) ) {
		return -1;
	}

	_Initialize( );

	return 0;
}

void CMainFrame::OnClose( ) {
#ifdef USE_TIMER
	KillTimer( m_uTimerId );
#endif

	if ( FcIsHookEnabled( ) ) {
		FcDisableHook( );
	}
	
	delete m_pTrayIcon;

	CFrameWnd::OnClose( );
}

LRESULT CMainFrame::OnNotifyIcon(WPARAM wParam, LPARAM lParam) {
	return m_pTrayIcon->OnTrayNotification( wParam, lParam );
}

LRESULT CMainFrame::OnReconfigure( WPARAM, LPARAM lparamOptionsPropSheet ) {
	COptionsPropSheet* poptions = reinterpret_cast< COptionsPropSheet* >( lparamOptionsPropSheet );

	*m_pOptions = poptions->GetNewOptions( );
	m_pOptions->Save( );
	_Reconfigure( );

	return 0;
}

LRESULT CMainFrame::OnFcmPip(WPARAM wPip, LPARAM /*lParam*/) {
	debug( L"CMainFrame::OnFcmPip: wPip=%d\n", wPip );

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

void CMainFrame::OnAppToggle( void ) {
	if ( FcIsHookEnabled( ) ) {
		FcDisableHook();
		m_pTrayIcon->SetTooltipText( m_strDisabled );
		m_pTrayIcon->SetBalloonDetails( m_strDisabled, m_strTitle, CTrayNotifyIcon::Info, 10 );
	} else {
		FcEnableHook( );
		m_pTrayIcon->SetTooltipText( m_strEnabled );
		m_pTrayIcon->SetBalloonDetails( m_strEnabled, m_strTitle, CTrayNotifyIcon::Info, 10 );
	}
}

void CMainFrame::OnAppCapsLock( void ) {
	INPUT inputs[2];
	memset( inputs, 0, 2 * sizeof( INPUT ) );

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_CAPITAL;
	inputs[1].ki.dwFlags = 0;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_CAPITAL;
	inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

	UINT u = SendInput( 2, inputs, sizeof( INPUT ) );
	if ( u < 2 ) {
		debug( L"CMainFrame::OnAppCapsLock: SendInput failed: sent=%d err=%d\n", u, GetLastError( ) );
	}
}

void CMainFrame::OnAppConfigure( ) {
	bool reused = false;

	LOCK( m_csPropSheet ) {
		MemoryBarrier( );
		if ( NULL != m_pPropSheet ) {
			debug( L"CMainFrame::OnAppConfigure: reusing existing prop sheet\n" );
			m_pPropSheet->ShowWindow( SW_SHOW );
			reused = true;
		}
	} UNLOCK( m_csPropSheet );
	if ( reused )
		return;

	COptionsPropSheet options( *m_pOptions, this );
	LOCK( m_csPropSheet ) { m_pPropSheet = &options; MemoryBarrier( ); } UNLOCK( m_csPropSheet );
	INT_PTR rc = options.DoModal( );
	LOCK( m_csPropSheet ) { m_pPropSheet = NULL;     MemoryBarrier( ); } UNLOCK( m_csPropSheet );

	if ( IDOK != rc ) {
		return;
	}

	const COptionsData& newoptions = options.GetNewOptions( );
	if ( *m_pOptions != newoptions ) {
		OnReconfigure( 0, (LPARAM) &options );
	}
}

void CMainFrame::OnUpdateAppToggle( CCmdUI* pui ) {
	if ( FcIsHookEnabled( ) ) {
		pui->SetText( CString( (LPCWSTR) IDS_MAINFRAME_MENU_DISABLE ) );
	} else {
		pui->SetText( CString( (LPCWSTR) IDS_MAINFRAME_MENU_ENABLE ) );
	}
}

void CMainFrame::OnUpdateAppCapsLock( CCmdUI* pui ) {
	pui->SetCheck( IsCapsLock( ) ? 1 : 0 );
}

BOOL CMainFrame::PreTranslateMessage( MSG* pMsg ) {
	if ( IsDialogMessage( pMsg ) ) {
		return TRUE;
	}
	
	return CFrameWnd::PreTranslateMessage( pMsg );
}

#ifdef USE_TIMER
void CMainFrame::OnTimer( UINT_PTR uId ) {
	// put code here

	CFrameWnd::OnTimer( uId );
}
#endif
