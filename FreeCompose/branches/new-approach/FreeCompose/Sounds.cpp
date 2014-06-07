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

	ON_BN_CLICKED( IDC_S_NO_SOUND,          CSounds::OnNoSoundClicked          )
	ON_BN_CLICKED( IDC_S_APPLICATION_SOUND, CSounds::OnApplicationSoundClicked )
	ON_BN_CLICKED( IDC_S_TONE_SOUND,        CSounds::OnToneSoundClicked        )

	ON_CBN_SELCHANGE( IDC_S_SCHEME, CSounds::OnSchemeChanged )
	ON_CBN_SELCHANGE( IDC_S_EVENT,  CSounds::OnEventChanged  )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//==============================================================================
// Constants
//==============================================================================

CSounds::MethodPtr const CSounds::_EventGroupDispatchTable[] {
	&CSounds::_UpdateNoSound,
	&CSounds::_UpdateApplicationSound,
	&CSounds::_UpdateToneSound,
};

DWORD const dwOfnFlags =
	OFN_DONTADDTORECENT  |
	OFN_ENABLESIZING     |
	OFN_FILEMUSTEXIST    |
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
// CSounds helper members
//==============================================================================

inline SoundScheme& CSounds::_GetCurrentScheme( void ) const {
	return m_Schemes[m_nCurrentScheme];
}

inline CString CSounds::_GetCurrentSchemeId( void ) const {
	return _GetCurrentScheme( ).ID;
}

inline SoundEvent* CSounds::_GetCurrentEvent( void ) const {
	return _GetCurrentScheme( ).Events[_GetCurrentEventName( )];
}

inline CString CSounds::_GetCurrentEventName( void ) const {
	return CString( ApplicationSoundNames[m_nCurrentEvent] );
}

inline void CSounds::_SetCurrentEvent( SoundEvent* pEvent ) {
	SoundEventMap& events = _GetCurrentScheme( ).Events;
	CString name = _GetCurrentEventName( );
	if ( events[name] ) {
		delete events[name];
	}
	events[name] = pEvent;
}

void CSounds::_UpdateNoSound( void ) {
	m_nRadioIndex = 0;

	m_editFileName.EnableWindow( FALSE );
	m_editFileName.SetWindowText( L"" );
	m_buttonBrowse.EnableWindow( FALSE );

	m_editFrequency.EnableWindow( FALSE );
	m_editFrequency.SetWindowText( L"" );
	m_editDuration.EnableWindow( FALSE );
	m_editDuration.SetWindowText( L"" );
}

void CSounds::_UpdateApplicationSound( void ) {
	m_nRadioIndex = 1;

	m_editFileName.EnableWindow( TRUE );
	m_editFileName.SetWindowText( CFcAppSoundsRegistry::GetEventFileName( _GetCurrentSchemeId( ), _GetCurrentEventName( ) ) );
	m_buttonBrowse.EnableWindow( TRUE );

	m_editFrequency.EnableWindow( FALSE );
	m_editFrequency.SetWindowText( L"" );
	m_editDuration.EnableWindow( FALSE );
	m_editDuration.SetWindowText( L"" );
}

void CSounds::_UpdateToneSound( void ) {
	m_nRadioIndex = 2;

	m_editFileName.EnableWindow( FALSE );
	m_editFileName.SetWindowText( L"" );
	m_buttonBrowse.EnableWindow( FALSE );

	m_editFrequency.EnableWindow( TRUE );
	m_editDuration.EnableWindow( TRUE );

	ToneSoundEvent* pTone = dynamic_cast<ToneSoundEvent*>( _GetCurrentEvent( ) );
	if ( pTone ) {
		CString tmp;

		tmp.Format( L"%u", pTone->Frequency );
		m_editFrequency.SetWindowText( tmp );

		tmp.Format( L"%u", pTone->Duration );
		m_editDuration.SetWindowText( tmp );
	} else {
		m_editFrequency.SetWindowText( L"" );
		m_editDuration.SetWindowText( L"" );
	}
}

void CSounds::_UpdateEventGroup( void ) {
	if ( m_nRadioIndex >= 0 && m_nRadioIndex < 3 ) {
		( this->*_EventGroupDispatchTable[m_nRadioIndex] )( );
	} else {
		debug( L"CSounds::_UpdateEventGroup: bad value for m_nRadioIndex %d\n", m_nRadioIndex );
	}
}

//==============================================================================
// CSounds implementation
//==============================================================================

void CSounds::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );

	DDX_Control ( pDX, IDC_S_SCHEME,            m_comboScheme           );
	DDX_Control ( pDX, IDEDIT,                  m_buttonRename          );
	DDX_Control ( pDX, IDREMOVE,                m_buttonRemove          );
	DDX_Control ( pDX, IDC_S_EVENT,             m_comboEvent            );
	DDX_Control ( pDX, IDC_S_NO_SOUND,          m_radioNoSound          );
	DDX_Control ( pDX, IDC_S_APPLICATION_SOUND, m_radioApplicationSound );
	DDX_Control ( pDX, IDC_S_TONE_SOUND,        m_radioToneSound        );
	DDX_Control ( pDX, IDC_S_FILE_NAME,         m_editFileName          );
	DDX_Control ( pDX, IDBROWSE,                m_buttonBrowse          );
	DDX_Control ( pDX, IDC_S_FREQUENCY,         m_editFrequency         );
	DDX_Control ( pDX, IDC_S_DURATION,          m_editDuration          );

	DDX_CBIndex ( pDX, IDC_S_SCHEME,            m_nCurrentScheme        );
	DDX_CBIndex ( pDX, IDC_S_EVENT,             m_nCurrentEvent         );
	DDX_Radio   ( pDX, IDC_S_NO_SOUND,          m_nRadioIndex           );
	DDX_Text    ( pDX, IDC_S_FILE_NAME,         m_strFileName           );
	DDX_Text    ( pDX, IDC_S_FREQUENCY,         m_nFrequency            );
	DDX_Text    ( pDX, IDC_S_DURATION,          m_nDuration             );

	DDV_MinMaxInt( pDX, m_nFrequency, 37, 32767 );
}

CSounds::CSounds( COptionsData& Options ):
	CPropertyPage    ( IDT_SOUNDS ),
	m_Options        ( Options ),
	m_Schemes        ( Options.Sounds.Schemes ),
	m_nCurrentScheme ( 0 ),
	m_nCurrentEvent  ( 0 ),
	m_nRadioIndex    ( 0 ),
	m_nFrequency     ( 0 ),
	m_nDuration      ( 0 )
{
	debug( L"CSounds::`ctor\n" );
}

CSounds::~CSounds( )
{
	debug( L"CSounds::`dtor\n" );
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

	_UpdateEventGroup( );

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
	CString m_soundEventFileName;
	m_editFileName.GetWindowText( m_soundEventFileName );

	debug( L"CSounds::OnBrowse: about to show common file dialog\n" );
	CFileDialog dlg( TRUE, L"wav", m_soundEventFileName, dwOfnFlags, L"Wave Files (*.wav)|*.wav|All Files (*.*)|*.*||", this, TRUE );
	if ( !m_soundEventFileName.IsEmpty( ) ) {
		dlg.m_ofn.lpstrInitialDir = GetPathFromFileName( m_soundEventFileName );
	} else {
		dlg.m_ofn.lpstrInitialDir = strLastDirectory;
	}

	INT_PTR ret = dlg.DoModal( );
	if ( IDCANCEL == ret ) {
		debug( L"CSounds::OnBrowse: user clicked Cancel\n" );
		return;
	}
	if ( IDOK != ret ) {
		debug( L"CSounds::OnBrowse: unknown return value from DoModal: %d\n", static_cast<int>( ret ) );
		return;
	}

	_SetCurrentEvent( new ApplicationSoundEvent( ) );
	CFcAppSoundsRegistry::SetEventFileName( _GetCurrentSchemeId( ), _GetCurrentEventName( ), dlg.GetPathName( ) );
	m_editFileName.SetWindowText( m_soundEventFileName );
}

void CSounds::OnSchemeChanged( ) {
	debug( L"CSounds::OnSchemeChanged\n" );
	m_nCurrentScheme = m_comboScheme.GetCurSel( );
	BOOL fEnable = FALSE;
	if ( CB_ERR != m_nCurrentScheme ) {
		CString strSchemeId = _GetCurrentSchemeId( );
		fEnable = ( ( 0 != strSchemeId.CompareNoCase( L".Default" ) ) && ( 0 != strSchemeId.CompareNoCase( L".None" ) ) );
	}
	
	m_buttonRename.EnableWindow( fEnable );
	m_buttonRemove.EnableWindow( fEnable );
	m_comboEvent.SetCurSel( 0 );
	_UpdateEventGroup( );
}

void CSounds::OnEventChanged( ) {
	debug( L"CSounds::OnEventChanged\n" );
	m_nCurrentEvent = m_comboEvent.GetCurSel( );
	if ( CB_ERR != m_nCurrentEvent ) {
		m_nRadioIndex = +( _GetCurrentEvent( )->GetType( ) );
	}

	m_radioNoSound         .SetCheck( ( 0 == m_nRadioIndex ) ? BST_CHECKED : BST_UNCHECKED );
	m_radioApplicationSound.SetCheck( ( 1 == m_nRadioIndex ) ? BST_CHECKED : BST_UNCHECKED );
	m_radioToneSound       .SetCheck( ( 2 == m_nRadioIndex ) ? BST_CHECKED : BST_UNCHECKED );

	_UpdateEventGroup( );
}

void CSounds::OnNoSoundClicked( ) {
	debug( L"CSounds::OnNoSoundClicked\n" );
	_UpdateNoSound( );
}

void CSounds::OnApplicationSoundClicked( ) {
	debug( L"CSounds::OnApplicationSoundClicked\n" );
	_UpdateApplicationSound( );
}

void CSounds::OnToneSoundClicked( ) {
	debug( L"CSounds::OnToneSoundClicked\n" );
	_UpdateToneSound( );
}
