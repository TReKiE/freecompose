#include "stdafx.h"

#include "FreeCompose.h"
#include "AppSoundsRegistry.h"
#include "Utils.h"

#include "Sounds.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

//==============================================================================
// Definitions for CSounds
//==============================================================================

IMPLEMENT_DYNAMIC( CSounds, CPropertyPage )

BEGIN_MESSAGE_MAP( CSounds, CPropertyPage )
	//{{AFX_MSG_MAP( CSounds )
	ON_BN_CLICKED( IDADD,    CSounds::OnAdd    )
	ON_BN_CLICKED( IDEDIT,   CSounds::OnRename )
	ON_BN_CLICKED( IDREMOVE, CSounds::OnRemove )
	ON_BN_CLICKED( IDBROWSE, CSounds::OnBrowse )
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

	for ( auto& soundId : ApplicationSoundDisplayNameIds ) {
		m_comboEvent.AddString( LoadFromStringTable( soundId ) );
	}

	return TRUE;
}

void CSounds::OnAdd( ) {
	// TODO
	// pop up Add/Rename dialog for new name
}

void CSounds::OnRename( ) {
	// TODO
	// pop up Add/Rename dialog for new name
}

void CSounds::OnRemove( ) {
	// TODO
	// confirm deletion, then delete
}

void CSounds::OnBrowse( ) {
	// TODO
	// init m_soundEventFileName

	CString m_soundEventFileName;

	CFileDialog dlg( TRUE, L"wav", m_soundEventFileName, dwOfnFlags, L"Wave Files (*.wav)|*.wav|All Files (*.*)|*.*||", this, TRUE );
	if ( !strLastDirectory.IsEmpty( ) ) {
		dlg.m_ofn.lpstrInitialDir = strLastDirectory;
	} else {
		dlg.m_ofn.lpstrInitialDir = GetPathFromFileName( m_soundEventFileName );
	}

	INT_PTR ret = dlg.DoModal( );

}
