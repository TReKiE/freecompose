#include "stdafx.h"

#include "FreeCompose.h"
#include "Features.h"

#include "Utils.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

const DWORD SpecialKeys[] = {
	VK_LCONTROL, VK_RCONTROL, VK_LMENU,    VK_RMENU,
	VK_LWIN,     VK_RWIN,     VK_APPS,     VK_CAPITAL,
	VK_NUMLOCK,  VK_SCROLL,   VK_ESCAPE,   VK_PRINT,
	VK_PAUSE,    VK_TAB,      VK_BACK,
};

IMPLEMENT_DYNAMIC( CFeatures, CPropertyPage )

BEGIN_MESSAGE_MAP( CFeatures, CPropertyPage )
	//{{AFX_MSG_MAP( CFeatures )
	ON_BN_CLICKED   ( IDC_F_ENABLE_ON_STARTUP,  &CFeatures::OnChanged                )
	ON_BN_CLICKED   ( IDC_F_START_WITH_WINDOWS, &CFeatures::OnChanged                )
	ON_BN_CLICKED   ( IDC_F_SWAPCAPSLOCK,       &CFeatures::OnSwapCapsLockChanged    )
	ON_CBN_SELCHANGE( IDC_F_COMPOSE_KEY,        &CFeatures::OnComposeKeyChanged      )
	ON_CBN_SELCHANGE( IDC_F_SWAPCAPSLOCK_KEY,   &CFeatures::OnSwapCapsLockKeyChanged )

	ON_CONTROL_RANGE( BN_CLICKED, IDC_F_CAPS_LOCK_TOGGLE_MODE_NORMAL, IDC_F_CAPS_LOCK_TOGGLE_MODE_DISABLED, &CFeatures::OnCapsLockToggleModeChanged )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )

CFeatures::CFeatures( COptionsData& Options ):
	CPropertyPage ( IDT_FEATURES ),
	m_Options     ( Options )
{
	debug( L"CFeatures::`ctor\n" );
}

CFeatures::~CFeatures( ) {
	debug( L"CFeatures::`dtor\n" );
}

void CFeatures::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );

	DDX_Control ( pDX, IDC_F_COMPOSE_KEY,                      m_cbComposeKey             );
	DDX_Control ( pDX, IDC_F_SWAPCAPSLOCK,                     m_btnSwapCapsLock          );
	DDX_Control ( pDX, IDC_F_SWAPCAPSLOCK_KEY,                 m_cbSwapCapsLockKey        );

	DDX_Check   ( pDX, IDC_F_ENABLE_ON_STARTUP,                m_Options.StartActive      );
	DDX_Check   ( pDX, IDC_F_START_WITH_WINDOWS,               m_Options.StartWithWindows );
	DDX_Radio   ( pDX, IDC_F_CAPS_LOCK_TOGGLE_MODE_NORMAL,     m_nCapsLockToggleMode      );
}

BOOL CFeatures::OnInitDialog( ) {
	if ( !CPropertyPage::OnInitDialog( ) ) {
		return FALSE;
	}

	int index;
	for ( int n = 0; n < _countof( SpecialKeys ); n++ ) {
		CString keyName( reinterpret_cast<LPCWSTR>( IDS_VK_NAMES_BASE + n ) );

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

	m_nCapsLockToggleMode = m_Options.CapsLockToggleMode - 1;

	UpdateData( FALSE );
	return TRUE;
}

//
// Event handlers
//

void CFeatures::OnChanged( ) {
	SetModified( );
}

void CFeatures::OnComposeKeyChanged( ) {
	int index = m_cbComposeKey.GetCurSel( );
	if ( CB_ERR == index ) {
		return;
	}

	DWORD dw = static_cast<DWORD>( m_cbComposeKey.GetItemData( index ) );
	if ( CB_ERR == dw ) {
		return;
	}

	m_Options.ComposeVk = dw;
	SetModified( );
}

void CFeatures::OnSwapCapsLockChanged( ) {
	m_cbSwapCapsLockKey.EnableWindow( BST_CHECKED == m_btnSwapCapsLock.GetCheck( ) );
	SetModified( );
}

void CFeatures::OnSwapCapsLockKeyChanged( ) {
	int index = m_cbSwapCapsLockKey.GetCurSel( );
	if ( CB_ERR == index ) {
		return;
	}

	DWORD dw = static_cast<DWORD>( m_cbSwapCapsLockKey.GetItemData( index ) );
	if ( CB_ERR == dw ) {
		return;
	}

	m_Options.SwapCapsLockVk = dw;
	SetModified( );
}

void CFeatures::OnCapsLockToggleModeChanged( UINT uID ) {
	m_Options.CapsLockToggleMode = static_cast<CAPS_LOCK_TOGGLE_MODE>( uID - IDC_F_CAPS_LOCK_TOGGLE_MODE_NORMAL + 1 );
	SetModified( );
}
