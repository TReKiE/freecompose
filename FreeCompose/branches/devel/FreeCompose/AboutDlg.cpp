// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FreeCompose.h"
#include "AboutDlg.h"

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg( ):
	CDialog ( CAboutDlg::IDD )
{
}

void CAboutDlg::DoDataExchange( CDataExchange* pDX ) {
	CDialog::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_ABOUT_VERSION, m_strVersion );
	DDX_Text( pDX, IDC_ABOUT_COPYRIGHT, m_strCopyright );
}

BOOL CAboutDlg::OnInitDialog( ) {
	void* pvdata = NULL;
	CString str;
	CString strExeName;
	DWORD dwBytes, dwDummy;

	strExeName.Format( _T("%s.exe"), theApp.m_pszExeName );
	dwBytes = GetFileVersionInfoSize( strExeName, &dwDummy );
	if ( 0 == dwBytes ) {
		str.Format( _T("GetFileVersionInfoSize failed: %d\n"), GetLastError() );
		OutputDebugString( str );
		return FALSE;
	}
	pvdata = malloc( dwBytes );
	if ( ! GetFileVersionInfo( strExeName, NULL, dwBytes, pvdata ) ) {
		str.Format( _T("GetFileVersionInfo failed: %d\n"), GetLastError() );
		OutputDebugString( str );
		free( pvdata );
		return FALSE;
	}

	VS_FIXEDFILEINFO* pvsffi = NULL;
	LPVOID pvffi = NULL;
	UINT uLen;
	if ( ! VerQueryValue( pvdata, _T("\\"), &pvffi, &uLen ) ) {
		str.Format( _T("VerQueryValue failed: %d\n"), GetLastError() );
		OutputDebugString( str );
		free( pvdata );
		return FALSE;
	}
	pvsffi = (VS_FIXEDFILEINFO*) pvffi;
	
	m_strVersion.Format(
		CString( (LPCWSTR) IDS_ABOUT_VERSION ),
		CString( (LPCWSTR) AFX_IDS_APP_TITLE ),
		HIWORD( pvsffi->dwProductVersionMS ), LOWORD( pvsffi->dwProductVersionMS ),
		HIWORD( pvsffi->dwProductVersionLS ), LOWORD( pvsffi->dwProductVersionLS )
	);

	if ( NULL != pvdata ) {
		free( pvdata );
	}
	UpdateData( FALSE );
	return TRUE;
}
