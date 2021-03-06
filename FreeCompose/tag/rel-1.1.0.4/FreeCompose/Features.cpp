// Features.cpp : implementation file

#include "stdafx.h"

#include "FreeCompose.h"
#include "Features.h"

#include "Utils.h"

const DWORD SpecialKeys[] = {
	VK_LCONTROL, VK_RCONTROL, VK_LMENU,    VK_RMENU,
	VK_LWIN,     VK_RWIN,     VK_APPS,     VK_CAPITAL,
	VK_NUMLOCK,  VK_SCROLL,   VK_ESCAPE,   VK_PRINT,
	VK_PAUSE,
};

IMPLEMENT_DYNAMIC( CFeatures, CPropertyPage )

BEGIN_MESSAGE_MAP( CFeatures, CPropertyPage )
	ON_BN_CLICKED( IDC_ENABLEONSTARTUP, &CFeatures::OnChanged )
	ON_BN_CLICKED( IDC_STARTWITHWINDOWS, &CFeatures::OnChanged )
	ON_CBN_SELCHANGE( IDC_COMPOSE_KEY, &CFeatures::OnComposeKeyChanged )
	ON_BN_CLICKED( IDC_SWAPCAPSLOCK, &CFeatures::OnSwapCapsLockChanged )
	ON_CBN_SELCHANGE( IDC_SWAPCAPSLOCK_KEY, &CFeatures::OnSwapCapsLockKeyChanged )
	ON_CONTROL_RANGE( BN_CLICKED, IDC_CAPSLOCKMODE_NORMAL, IDC_CAPSLOCKMODE_DISABLED, &CFeatures::OnCapsLockModeChanged )
END_MESSAGE_MAP( )

CFeatures::CFeatures( COptionsData& Options ):
	CPropertyPage   ( CFeatures::IDD ),
	m_Options       ( Options )
{
}

CFeatures::~CFeatures( ) {
}

void CFeatures::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );

	DDX_Check( pDX, IDC_ENABLEONSTARTUP,  m_Options.m_fStartActive );
	DDX_Check( pDX, IDC_STARTWITHWINDOWS, m_Options.m_fStartWithWindows );
	DDX_Check( pDX, IDC_SWAPCAPSLOCK,     m_Options.m_fSwapCapsLock );

	DDX_Control( pDX, IDC_COMPOSE_KEY,             m_cbComposeKey );
	DDX_Control( pDX, IDC_SWAPCAPSLOCK,            m_btnSwapCapsLock );
	DDX_Control( pDX, IDC_SWAPCAPSLOCK_KEY,        m_cbSwapCapsLockKey );
	DDX_Control( pDX, IDC_CAPSLOCKMODE_NORMAL,     m_btnClmNormal );
	DDX_Control( pDX, IDC_CAPSLOCKMODE_PRESSTWICE, m_btnClmPressTwice );
	DDX_Control( pDX, IDC_CAPSLOCKMODE_DISABLED,   m_btnClmDisabled );
}

BOOL CFeatures::OnInitDialog( ) {
	if ( ! CPropertyPage::OnInitDialog( ) )
		return FALSE;

	debug( L"CFeatures::OnInitDialog: %c%c%c %d 0x%02x 0x%02x\n",
		m_Options.m_fStartActive      ? 'T' : 'f',
		m_Options.m_fStartWithWindows ? 'T' : 'f',
		m_Options.m_fSwapCapsLock     ? 'T' : 'f',
		m_Options.m_CapsLockMode,
		m_Options.m_vkCompose,
		m_Options.m_vkSwapCapsLock
	);

	CString keyName;
	int index;
	for ( int n = 0; n < countof( SpecialKeys ); n++ ) {
		keyName.LoadString( IDS_VK_NAMES_BASE + n );

		index = m_cbComposeKey.AddString( keyName );
		if ( index >= 0 ) {
			m_cbComposeKey.SetItemData( index, SpecialKeys[n] );
			if ( m_Options.m_vkCompose == SpecialKeys[n] ) {
				m_cbComposeKey.SetCurSel( index );
			}
		}

		if ( VK_CAPITAL == SpecialKeys[n] ) {
			continue;
		}

		index = m_cbSwapCapsLockKey.AddString( keyName );
		if ( index >= 0 ) {
			m_cbSwapCapsLockKey.SetItemData( index, SpecialKeys[n] );
			if ( m_Options.m_vkSwapCapsLock == SpecialKeys[n] ) {
				m_cbSwapCapsLockKey.SetCurSel( index );
			}
		}
	}

	m_cbSwapCapsLockKey.EnableWindow( m_Options.m_fSwapCapsLock ? TRUE : FALSE );

	switch ( m_Options.m_CapsLockMode ) {
		case CLM_NORMAL:     m_btnClmNormal    .SetCheck( BST_CHECKED ); break;
		case CLM_PRESSTWICE: m_btnClmPressTwice.SetCheck( BST_CHECKED ); break;
		case CLM_DISABLED:   m_btnClmDisabled  .SetCheck( BST_CHECKED ); break;
	}

	UpdateData( FALSE );
	return TRUE;
}

BOOL CFeatures::PreTranslateMessage( MSG* pMsg ) {
	if ( IsDialogMessage( pMsg ) )
		return TRUE;

	return CPropertyPage::PreTranslateMessage( pMsg );
}

//
// Event handlers
//

void CFeatures::OnChanged( ) {
	debug( L"CFeatures::OnChanged\n" );

	SetModified( );
}

void CFeatures::OnComposeKeyChanged( ) {
	debug( L"CFeatures::OnComposeKeyChanged\n" );

	int index = m_cbComposeKey.GetCurSel( );
	if ( CB_ERR == index )
		return;

	DWORD_PTR dw = m_cbComposeKey.GetItemData( index );
	if ( CB_ERR == dw )
		return;

	m_Options.m_vkCompose = (DWORD) dw;
	SetModified( );
}

void CFeatures::OnSwapCapsLockChanged( ) {
	debug( L"CFeatures::OnSwapCapsLockChanged\n" );

	SetModified( );
	m_cbSwapCapsLockKey.EnableWindow( BST_CHECKED == m_btnSwapCapsLock.GetCheck( ) );
}

void CFeatures::OnSwapCapsLockKeyChanged( ) {
	debug( L"CFeatures::OnSwapCapsLockKeyChanged\n" );

	int index = m_cbSwapCapsLockKey.GetCurSel( );
	if ( CB_ERR == index )
		return;

	DWORD_PTR dw = m_cbSwapCapsLockKey.GetItemData( index );
	if ( CB_ERR == dw )
		return;

	m_Options.m_vkSwapCapsLock = (DWORD) dw;
	SetModified( );
}

void CFeatures::OnCapsLockModeChanged( UINT uID ) {
	debug( L"CFeatures::OnCapsLockModeChanged %d\n", uID );

	SetModified( );
	switch ( uID ) {
		case IDC_CAPSLOCKMODE_NORMAL:
			m_Options.m_CapsLockMode = CLM_NORMAL;
			break;

		case IDC_CAPSLOCKMODE_PRESSTWICE:
			m_Options.m_CapsLockMode = CLM_PRESSTWICE;
			break;

		case IDC_CAPSLOCKMODE_DISABLED:
			m_Options.m_CapsLockMode = CLM_DISABLED;
			break;
	}
}
