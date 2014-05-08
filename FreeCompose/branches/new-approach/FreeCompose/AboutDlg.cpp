// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FreeCompose.h"
#include "AboutDlg.h"
#include "Utils.h"

BEGIN_MESSAGE_MAP( CAboutDlg, CDialog )
END_MESSAGE_MAP( )

CAboutDlg::CAboutDlg( ):
	CDialog( IDD )
{

}

void CAboutDlg::DoDataExchange( CDataExchange* pDX ) {
	CDialog::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_ABOUT_VERSION,   m_strVersion   );
	DDX_Text( pDX, IDC_ABOUT_COPYRIGHT, m_strCopyright );
}

BOOL CAboutDlg::OnInitDialog( ) {
	CString strExeName;
	strExeName.Format( L"%s.exe", theApp.m_pszExeName );

	SetLastError( ERROR_SUCCESS );
	DWORD dwBytes = GetFileVersionInfoSize( strExeName, nullptr/*&dwDummy*/ );
	if ( 0 == dwBytes ) {
		DWORD dwError = GetLastError( );
		debug( L"GetFileVersionInfoSize failed: %lu\n", dwError );
		return FALSE;
	}

	SetLastError( ERROR_SUCCESS );
	void* pvdata = malloc( dwBytes );
	if ( !pvdata ) {
		DWORD dwError = GetLastError( );
		debug( L"CAboutDlg::InitDialog: malloc(%lu) failed, errno: %d, Win32 error: %lu\n", dwBytes, errno, dwError );
		return FALSE;
	}

	SetLastError( ERROR_SUCCESS );
	if ( !GetFileVersionInfo( strExeName, 0, dwBytes, pvdata ) ) {
		DWORD dwError = GetLastError( );
		debug( L"GetFileVersionInfo failed: %lu\n", dwError );
		free( pvdata );
		return FALSE;
	}

	VS_FIXEDFILEINFO* pvsffi = nullptr;
	UINT uLen = 0;
	SetLastError( ERROR_SUCCESS );
	if ( !VerQueryValue( pvdata, L"\\", reinterpret_cast<LPVOID*>( &pvsffi ), &uLen ) ) {
		DWORD dwError = GetLastError( );
		debug( L"VerQueryValue failed: %lu\n", dwError );
		free( pvdata );
		return FALSE;
	}

	m_strVersion.Format(
		LoadFromStringTable( IDS_ABOUT_VERSION ),
		LoadFromStringTable( AFX_IDS_APP_TITLE ),
		HIWORD( pvsffi->dwProductVersionMS ), LOWORD( pvsffi->dwProductVersionMS ),
		HIWORD( pvsffi->dwProductVersionLS ), LOWORD( pvsffi->dwProductVersionLS )
	);

	free( pvdata );
	UpdateData( FALSE );
	return TRUE;
}
