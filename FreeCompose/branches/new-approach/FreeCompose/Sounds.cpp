#include "stdafx.h"

#include "FreeCompose.h"
#include "FcAppSoundsRegistry.h"
#include "Utils.h"

#include "Sounds.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

//==============================================================================
//
// Guide to this dialog's controls
// ===============================
//
// Type    Resource ID
// ------  --------------------------
// Combo   IDC_S_SCHEME
// Button  IDADD
// Button  IDEDIT
// Button  IDREMOVE
//
// Combo   IDC_S_EVENT
// Radio   IDC_S_NO_SOUND
// Radio   IDC_S_APPLICATION_SOUND
// Radio   IDC_S_TONE_SOUND
//
// Edit    IDC_S_FILE_NAME
// Button  IDBROWSE
//
// Edit    IDC_S_FREQUENCY
// Edit    IDC_S_DURATION
//
//==============================================================================

//==============================================================================
// Definitions for CSounds
//==============================================================================

IMPLEMENT_DYNAMIC( CSounds, CPropertyPage )

BEGIN_MESSAGE_MAP( CSounds, CPropertyPage )
	//{{AFX_MSG_MAP( CSounds )
	ON_COMMAND( IDADD,    CSounds::OnAdd    )
	ON_COMMAND( IDEDIT,   CSounds::OnRename )
	ON_COMMAND( IDREMOVE, CSounds::OnRemove )
	ON_COMMAND( IDBROWSE, CSounds::OnBrowse )

	ON_CONTROL_RANGE( BN_CLICKED, IDC_S_NO_SOUND, IDC_S_TONE_SOUND, &CSounds::OnRadioGroupClicked )

	ON_CBN_SELCHANGE( IDC_S_SCHEME, CSounds::OnSchemeChanged )
	ON_CBN_SELCHANGE( IDC_S_EVENT,  CSounds::OnEventChanged )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//==============================================================================
// Constants
//==============================================================================

DWORD const dwOfnFlags =
	OFN_DONTADDTORECENT  |
	OFN_ENABLESIZING     |
	OFN_FILEMUSTEXIST    |
	OFN_HIDEREADONLY     |
	OFN_LONGNAMES        |
	OFN_NOCHANGEDIR      |
	OFN_NOTESTFILECREATE |
	OFN_PATHMUSTEXIST    |
	OFN_SHAREAWARE;

//==============================================================================
// Local variables
//==============================================================================

static CString strLastDirectory;

//==============================================================================
// Local functions
//==============================================================================

static CString GetPathFromFileName( CString const& strFileName ) {
	errno_t err;

	wchar_t wzDrive[_MAX_DRIVE] = { };
	wchar_t wzDirectory[_MAX_DIR] = { };
	err = _wsplitpath_s( strFileName, wzDrive, _MAX_DRIVE, wzDirectory, _MAX_DIR, nullptr, 0, nullptr, 0 );
	if ( err ) {
		debug( L"GetPathFromFileName: splitpath failed: %d\n", err );
	}

	wchar_t wzPathName[_MAX_PATH] = { };
	err = _wmakepath_s( wzPathName, wzDrive, wzDirectory, nullptr, nullptr );
	if ( err ) {
		debug( L"GetPathFromFileName: makepath failed: %d\n", err );
	}

	return CString( wzPathName );
}

//==============================================================================
// CSounds
//==============================================================================

void CSounds::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );

	DDX_Control ( pDX, IDC_S_SCHEME,            m_comboScheme           );
	DDX_Control ( pDX, IDREMOVE,                m_buttonRemove          );
	DDX_Control ( pDX, IDC_S_EVENT,             m_comboEvent            );
	DDX_Control ( pDX, IDC_S_NO_SOUND,          m_radioNoSound          );
	DDX_Control ( pDX, IDC_S_APPLICATION_SOUND, m_radioApplicationSound );
	DDX_Control ( pDX, IDC_S_TONE_SOUND,        m_radioToneSound        );
	DDX_Control ( pDX, IDC_S_FILE_NAME,         m_editFileName          );
	DDX_Control ( pDX, IDBROWSE,                m_buttonBrowse          );
	DDX_Control ( pDX, IDC_S_FREQUENCY,         m_editFrequency         );
	DDX_Control ( pDX, IDC_S_DURATION,          m_editDuration          );

	DDX_CBIndex ( pDX, IDC_S_SCHEME,            m_nSchemeIndex          );
	DDX_CBIndex ( pDX, IDC_S_EVENT,             m_nEventIndex           );
	DDX_Radio   ( pDX, IDC_S_NO_SOUND,          m_nRadioIndex           );
	DDX_Text    ( pDX, IDC_S_FILE_NAME,         m_strFileName           );
	DDX_Text    ( pDX, IDC_S_FREQUENCY,         m_nFrequency            );
	DDX_Text    ( pDX, IDC_S_DURATION,          m_nDuration             );

	DDV_MinMaxInt( pDX, m_nFrequency, 37, 32767 );
}

BOOL CSounds::OnInitDialog( ) {
	if ( !CPropertyPage::OnInitDialog( ) ) {
		return FALSE;
	}

	for ( auto& scheme : m_Schemes ) {
		m_comboScheme.AddString( scheme.Name );
	}

	for ( auto soundId : ApplicationSoundDisplayNameIds ) {
		m_comboEvent.AddString( LoadFromStringTable( soundId ) );
	}

	return TRUE;
}

void CSounds::OnAdd( ) {
	// TODO
	// pop up Add/Rename dialog for name
}

void CSounds::OnRename( ) {
	// TODO
	// pop up Add/Rename dialog for name
}

void CSounds::OnRemove( ) {
	// TODO
	// confirm deletion, then delete
}

void CSounds::OnBrowse( ) {
	debug( L"CSounds::OnBrowse: about to show common file dialog\n" );

	UpdateData( TRUE );

	CString m_soundEventFileName = CFcAppSoundsRegistry::GetEventFileName( m_Schemes[m_nSchemeIndex].ID, ApplicationSoundNames[m_nEventIndex] );

	CFileDialog dlg( TRUE, L"wav", m_soundEventFileName, dwOfnFlags, L"Wave Files (*.wav)|*.wav|All Files (*.*)|*.*||", this, TRUE );
	if ( !m_soundEventFileName.IsEmpty( ) ) {
		dlg.m_ofn.lpstrInitialDir = GetPathFromFileName( m_soundEventFileName );
	} else {
		dlg.m_ofn.lpstrInitialDir = strLastDirectory;
	}

	INT_PTR ret = dlg.DoModal( );
	if ( IDCANCEL == ret ) {
		debug( L"CSounds::OnBrowse: user clicked Cancel\n" );
	}
	if ( IDOK != ret ) {
		debug( L"CSounds::OnBrowse: unknown return value from DoModal: %d\n", static_cast<int>( ret ) );
	}

	SoundEvent* pSoundEvent = m_Schemes[m_nSchemeIndex].Events[ApplicationSoundNames[m_nEventIndex]];
	if ( typeid( ApplicationSoundEvent ) != typeid( *pSoundEvent ) ) {
		m_Schemes[m_nSchemeIndex].Events[ApplicationSoundNames[m_nEventIndex]] = new ApplicationSoundEvent( );
		delete pSoundEvent;
	}
}

void CSounds::OnRadioGroupClicked( UINT uID ) {
	m_nRadioIndex = uID - IDC_S_NO_SOUND;

	BOOL fApplicationSound = ( IDC_S_APPLICATION_SOUND == uID );
	BOOL fToneSound        = ( IDC_S_TONE_SOUND        == uID );

	m_editFileName.EnableWindow( fApplicationSound );
	m_buttonBrowse.EnableWindow( fApplicationSound );

	m_editFrequency.EnableWindow( fToneSound );
	m_editDuration .EnableWindow( fToneSound );
}

void CSounds::OnSchemeChanged( ) {
	m_nSchemeIndex = m_comboScheme.GetCurSel( );
	if ( CB_ERR != m_nSchemeIndex ) {
		CString strSchemeId = m_Schemes[m_nSchemeIndex].ID;
		m_buttonRemove.EnableWindow( ( 0 != strSchemeId.CompareNoCase( L".Default" ) ) && ( 0 != strSchemeId.CompareNoCase( L".None" ) ) );
	} else {
		m_buttonRemove.EnableWindow( FALSE );
	}
}

void CSounds::OnEventChanged( ) {
	m_nEventIndex = m_comboEvent.GetCurSel( );
	if ( CB_ERR != m_nEventIndex ) {
		SoundEvent* pSoundEvent = m_Schemes[m_nSchemeIndex].Events[ApplicationSoundNames[m_nEventIndex]];
		if ( typeid( NoSoundEvent ) == typeid( *pSoundEvent ) ) {
			m_nRadioIndex = 0;
		} else if ( typeid( ApplicationSoundEvent ) == typeid( *pSoundEvent ) ) {
			m_nRadioIndex = 1;
		} else if ( typeid( ToneSoundEvent ) == typeid( *pSoundEvent ) ) {
			m_nRadioIndex = 2;
		} else {
			debug( L"CSounds::OnEventChanged: unknown type of sound event '%hs'?\n", typeid( *pSoundEvent ).name( ) );
			m_nRadioIndex = 0;
		}
	} else {
		m_nRadioIndex = 0;
	}

	m_radioNoSound         .SetCheck( ( 0 == m_nRadioIndex ) ? BST_CHECKED : BST_UNCHECKED );
	m_radioApplicationSound.SetCheck( ( 1 == m_nRadioIndex ) ? BST_CHECKED : BST_UNCHECKED );
	m_radioToneSound       .SetCheck( ( 2 == m_nRadioIndex ) ? BST_CHECKED : BST_UNCHECKED );
}
