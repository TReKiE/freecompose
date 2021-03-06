#include "stdafx.h"

#include "FreeCompose.h"
#include "AboutDlg.h"
#include "Utils.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

IMPLEMENT_DYNAMIC( CAboutDlg, CDialogEx )

BEGIN_MESSAGE_MAP( CAboutDlg, CDialogEx )
	ON_NOTIFY( NM_CLICK,  IDC_A_COPYRIGHT, &CAboutDlg::OnClickCopyright )
	ON_NOTIFY( NM_RETURN, IDC_A_COPYRIGHT, &CAboutDlg::OnClickCopyright )
	ON_NOTIFY( NM_CLICK,  IDC_A_WEBSITE,   &CAboutDlg::OnClickWebsite   )
	ON_NOTIFY( NM_RETURN, IDC_A_WEBSITE,   &CAboutDlg::OnClickWebsite   )
	ON_NOTIFY( NM_CLICK,  IDC_A_LICENSE,   &CAboutDlg::OnClickLicense   )
	ON_NOTIFY( NM_RETURN, IDC_A_LICENSE,   &CAboutDlg::OnClickLicense   )
END_MESSAGE_MAP( )

CAboutDlg::CAboutDlg( ):
	CDialogEx ( IDD_ABOUT ),
	m_pFont   ( nullptr )
{

}

CAboutDlg::~CAboutDlg() {
	if ( m_pFont ) {
		delete m_pFont;
		m_pFont = nullptr;
	}
}

void CAboutDlg::_GetVersionInfo( ) {
	CString strExeName( theApp.m_pszExeName );
	strExeName += L".exe";

	SetLastError( ERROR_SUCCESS );
	DWORD dwBytes = GetFileVersionInfoSize( strExeName, nullptr );
	if ( 0 == dwBytes ) {
		debug( L"GetFileVersionInfoSize failed: %lu\n", GetLastError( ) );
		return;
	}

	SetLastError( ERROR_SUCCESS );
	void* pvdata = new byte[dwBytes];
	if ( !pvdata ) {
		debug( L"CAboutDlg::InitDialog: new byte[%lu] failed, errno: %d, Win32 error: %lu\n", dwBytes, errno, GetLastError( ) );
		return;
	}

	SetLastError( ERROR_SUCCESS );
	if ( !GetFileVersionInfo( strExeName, 0, dwBytes, pvdata ) ) {
		debug( L"GetFileVersionInfo failed: %lu\n", GetLastError( ) );
		delete[] pvdata;
		return;
	}

	VS_FIXEDFILEINFO* pvsffi = nullptr;
	UINT uLen = 0;
	SetLastError( ERROR_SUCCESS );
	if ( !VerQueryValue( pvdata, L"\\", reinterpret_cast<LPVOID*>( &pvsffi ), &uLen ) ) {
		debug( L"VerQueryValue failed: %lu\n", GetLastError( ) );
		delete[] pvdata;
		return;
	}

	int v1 = static_cast<int>( static_cast<unsigned>( HIWORD( pvsffi->dwProductVersionMS ) ) );
	int v2 = static_cast<int>( static_cast<unsigned>( LOWORD( pvsffi->dwProductVersionMS ) ) );
	int v3 = static_cast<int>( static_cast<unsigned>( HIWORD( pvsffi->dwProductVersionLS ) ) );
	int v4 = static_cast<int>( static_cast<unsigned>( LOWORD( pvsffi->dwProductVersionLS ) ) );
	delete[] pvdata;

	m_strName = LoadFromStringTable( AFX_IDS_APP_TITLE );
	m_strVersion.Format( LoadFromStringTable( IDC_A_VERSION_FORMAT ), v1, v2, v3, v4 );
}

void CAboutDlg::_LaunchUrl( wchar_t const* pwzUrl ) {
	HINSTANCE hinst = ShellExecute( nullptr, L"open", pwzUrl, nullptr, nullptr, SW_SHOW );
	if ( reinterpret_cast<unsigned>( hinst ) > 32 ) {
		debug( L"CAboutDlg::_LaunchUrl: ShellExecute('%s') succeeded (hinst=0x%08X)\n", pwzUrl, hinst );
	} else {
		debug( L"CAboutDlg::_LaunchUrl: ShellExecute('%s') failed, hinst=%d\n", pwzUrl, hinst );
	}
}

void CAboutDlg::DoDataExchange( CDataExchange* pDX ) {
	CDialogEx::DoDataExchange( pDX );

	DDX_Text    ( pDX, IDC_A_NAME,      m_strName       );
	DDX_Text    ( pDX, IDC_A_VERSION,   m_strVersion    );

	DDX_Control ( pDX, IDC_A_NAME,      m_staticName    );
	DDX_Control ( pDX, IDC_A_COPYRIGHT, m_linkCopyright );
	DDX_Control ( pDX, IDC_A_WEBSITE,   m_linkWebsite   );
	DDX_Control ( pDX, IDC_A_LICENSE,   m_linkLicense   );
}

BOOL CAboutDlg::OnInitDialog( ) {
	if ( !CDialogEx::OnInitDialog( ) ) {
		return FALSE;
	}

	_GetVersionInfo( );

	int nHeight = 0;
	{
		CDC dc;
		if ( dc.CreateCompatibleDC( nullptr ) ) {
			nHeight = -MulDiv( 14, GetDeviceCaps( dc, LOGPIXELSY ), 72 );
		} else {
			debug( L"CAboutDlg::OnInitDialog: CreateCompatibleDC failed, can't get font height\n" );
		} 
	}

#pragma push_macro( "new" )
#undef new
	m_pFont = new CFont( );
#pragma pop_macro( "new" )
	if ( nHeight && m_pFont->CreateFont( nHeight, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY, FF_SWISS | VARIABLE_PITCH, L"MS Shell Dlg 2" ) ) {
		m_staticName.SetFont( m_pFont, TRUE );
	} else {
		debug( L"CAboutDlg::OnInitDialog: couldn't create font for control IDC_A_NAME\n" );
	}

	UpdateData( FALSE );
	return TRUE;
}

// for when the syslink controls are activated:
// ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);

void CAboutDlg::OnClickCopyright( NMHDR* pNMHDR, LRESULT* pResult ) {
	NMLINK* pnml = reinterpret_cast<NMLINK*>( pNMHDR );
	LITEM& item = pnml->item;

	debug(
		L"CAboutDlg::OnClickCopyright: LITEM structure contains:\n"
		L"+ UINT  mask       0x%08X\n"
		L"+ int   iLink      %d\n"
		L"+ UINT  state      0x%08X\n"
		L"+ UINT  stateMask  0x%08X\n"
		L"+ WCHAR szID[]     '%s'\n"
		L"+ WCHAR szUrl[]    '%s'\n",
		item.mask,
		item.iLink,
		item.state,
		item.stateMask,
		item.szID,
		item.szUrl
		);

	_LaunchUrl( item.szUrl );
	*pResult = 0;
}

void CAboutDlg::OnClickWebsite( NMHDR* pNMHDR, LRESULT* pResult ) {
	NMLINK* pnml = reinterpret_cast<NMLINK*>( pNMHDR );
	LITEM& item = pnml->item;

	debug(
		L"CAboutDlg::OnClickWebsite: LITEM structure contains:\n"
		L"+ UINT  mask       0x%08X\n"
		L"+ int   iLink      %d\n"
		L"+ UINT  state      0x%08X\n"
		L"+ UINT  stateMask  0x%08X\n"
		L"+ WCHAR szID[]     '%s'\n"
		L"+ WCHAR szUrl[]    '%s'\n",
		item.mask,
		item.iLink,
		item.state,
		item.stateMask,
		item.szID,
		item.szUrl
		);

	_LaunchUrl( item.szUrl );
	*pResult = 0;
}

void CAboutDlg::OnClickLicense( NMHDR* pNMHDR, LRESULT* pResult ) {
	NMLINK* pnml = reinterpret_cast<NMLINK*>( pNMHDR );
	LITEM& item = pnml->item;

	debug(
		L"CAboutDlg::OnClickLicense: LITEM structure contains:\n"
		L"+ UINT  mask       0x%08X\n"
		L"+ int   iLink      %d\n"
		L"+ UINT  state      0x%08X\n"
		L"+ UINT  stateMask  0x%08X\n"
		L"+ WCHAR szID[]     '%s'\n"
		L"+ WCHAR szUrl[]    '%s'\n",
		item.mask,
		item.iLink,
		item.state,
		item.stateMask,
		item.szID,
		item.szUrl
		);

	_LaunchUrl( item.szUrl );
	*pResult = 0;
}
