#include "stdafx.h"

#include "FreeCompose.h"
#include "Features.h"

#include "Utils.h"

const DWORD SpecialKeys[] = {
	VK_LCONTROL, VK_RCONTROL, VK_LMENU,    VK_RMENU,
	VK_LWIN,     VK_RWIN,     VK_APPS,     VK_CAPITAL,
	VK_NUMLOCK,  VK_SCROLL,   VK_ESCAPE,   VK_PRINT,
	VK_PAUSE,    VK_TAB,      VK_BACK,
};

IMPLEMENT_DYNAMIC( CFeatures, CPropertyPage )
BEGIN_MESSAGE_MAP( CFeatures, CPropertyPage )
	ON_BN_CLICKED( IDC_ENABLEONSTARTUP, &CFeatures::OnChanged )
	ON_BN_CLICKED( IDC_STARTWITHWINDOWS, &CFeatures::OnChanged )
	ON_CBN_SELCHANGE( IDC_COMPOSE_KEY, &CFeatures::OnComposeKeyChanged )
	ON_BN_CLICKED( IDC_SWAPCAPSLOCK, &CFeatures::OnSwapCapsLockChanged )
	ON_CBN_SELCHANGE( IDC_SWAPCAPSLOCK_KEY, &CFeatures::OnSwapCapsLockKeyChanged )
	ON_CONTROL_RANGE( BN_CLICKED, IDC_CAPSLOCKTOGGLEMODE_NORMAL, IDC_CAPSLOCKTOGGLEMODE_DISABLED, &CFeatures::OnCapsLockModeChanged )
END_MESSAGE_MAP( )

CFeatures::CFeatures( COptionsData& Options ):
	CPropertyPage ( CFeatures::IDD ),
	m_Options     ( Options )
{
}

CFeatures::~CFeatures( ) {
}

void CFeatures::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );

	DDX_Check( pDX, IDC_ENABLEONSTARTUP,  m_Options.StartActive );
	DDX_Check( pDX, IDC_STARTWITHWINDOWS, m_Options.StartWithWindows );
	// XXX //DDX_Check( pDX, IDC_SWAPCAPSLOCK,     m_Options.m_fSwapCapsLock );

	DDX_Control( pDX, IDC_COMPOSE_KEY,                   m_cbComposeKey );
	DDX_Control( pDX, IDC_SWAPCAPSLOCK,                  m_btnSwapCapsLock );
	DDX_Control( pDX, IDC_SWAPCAPSLOCK_KEY,              m_cbSwapCapsLockKey );
	DDX_Control( pDX, IDC_CAPSLOCKTOGGLEMODE_NORMAL,     m_btnCltmNormal );
	DDX_Control( pDX, IDC_CAPSLOCKTOGGLEMODE_PRESSTWICE, m_btnCltmPressTwice );
	DDX_Control( pDX, IDC_CAPSLOCKTOGGLEMODE_DISABLED,   m_btnCltmDisabled );
}

BOOL CFeatures::OnInitDialog( ) {
	if ( ! CPropertyPage::OnInitDialog( ) )
		return FALSE;

	debug( L"CFeatures::OnInitDialog: SA:%c SWW:%c CLM:%d CLS:%d VC:0x%02x VSCL:0x%02x\n",
		m_Options.StartActive      ? L'T' : L'f',
		m_Options.StartWithWindows ? L'T' : L'f',
		m_Options.CapsLockToggleMode,
		m_Options.CapsLockSwapMode,
		m_Options.ComposeVk,
		m_Options.SwapCapsLockVk
	);

	CString keyName;
	int index;
	for ( int n = 0; n < _countof( SpecialKeys ); n++ ) {
		SetLastError( ERROR_SUCCESS );
		if ( !keyName.LoadString( IDS_VK_NAMES_BASE + n ) ) {
			DWORD dwError = GetLastError( );
			debug( L"CFeatures::OnInitDialog: LoadString(%lu) failed, error=%lu\n", IDS_VK_NAMES_BASE + n, dwError );
			continue;
		}

		index = m_cbComposeKey.AddString( keyName );
		if ( index >= 0 ) {
			m_cbComposeKey.SetItemData( index, SpecialKeys[n] );
			if ( m_Options.ComposeVk == SpecialKeys[n] ) {
				m_cbComposeKey.SetCurSel( index );
			}
		}

		if ( VK_CAPITAL == SpecialKeys[n] ) {
			continue;
		}

		index = m_cbSwapCapsLockKey.AddString( keyName );
		if ( index >= 0 ) {
			m_cbSwapCapsLockKey.SetItemData( index, SpecialKeys[n] );
			if ( m_Options.SwapCapsLockVk == SpecialKeys[n] ) {
				m_cbSwapCapsLockKey.SetCurSel( index );
			}
		}
	}

	// XXX //m_cbSwapCapsLockKey.EnableWindow( m_Options.m_fSwapCapsLock ? TRUE : FALSE );

	switch ( m_Options.CapsLockToggleMode ) {
		case CLTM_NORMAL:     m_btnCltmNormal    .SetCheck( BST_CHECKED ); break;
		case CLTM_PRESSTWICE: m_btnCltmPressTwice.SetCheck( BST_CHECKED ); break;
		case CLTM_DISABLED:   m_btnCltmDisabled  .SetCheck( BST_CHECKED ); break;
	}

	UpdateData( FALSE );
	return TRUE;
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

	m_Options.ComposeVk = (DWORD) dw;
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

	m_Options.SwapCapsLockVk = (DWORD) dw;
	SetModified( );
}

void CFeatures::OnCapsLockModeChanged( UINT uID ) {
	debug( L"CFeatures::OnCapsLockModeChanged %d\n", uID );

	SetModified( );
	switch ( uID ) {
		case IDC_CAPSLOCKTOGGLEMODE_NORMAL:
			m_Options.CapsLockToggleMode = CLTM_NORMAL;
			break;

		case IDC_CAPSLOCKTOGGLEMODE_PRESSTWICE:
			m_Options.CapsLockToggleMode = CLTM_PRESSTWICE;
			break;

		case IDC_CAPSLOCKTOGGLEMODE_DISABLED:
			m_Options.CapsLockToggleMode = CLTM_DISABLED;
			break;
	}
}
