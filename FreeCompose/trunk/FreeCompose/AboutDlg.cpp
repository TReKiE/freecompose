// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FreeCompose.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_strVersion(_T(""))
, m_strCopyright(_T(""))
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ABOUT_VERSION, m_strVersion);
	DDX_Text(pDX, IDC_ABOUT_COPYRIGHT, m_strCopyright);
}

BOOL CAboutDlg::OnInitDialog() {
	void* pvdata;
	//VS_VERSIONINFO* pvi;
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
	//pvi = (VS_VERSIONINFO*) pvdata;
	if ( ! GetFileVersionInfo( strExeName, NULL, dwBytes, pvdata ) ) {
		str.Format( _T("GetFileVersionInfo failed: %d\n"), GetLastError() );
		OutputDebugString( str );
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
	
	// XXX
	m_strVersion.Format( _T("%s version %d.%d.%d.%d\n"), _T("FreeCompose"), HIWORD(pvsffi->dwProductVersionMS), LOWORD(pvsffi->dwProductVersionMS), HIWORD(pvsffi->dwProductVersionLS), LOWORD(pvsffi->dwProductVersionLS) );

	free( pvdata );
	UpdateData( FALSE );
	return TRUE;
}
