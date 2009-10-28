// FreeComposeDlg.h : header file

#pragma once

class CAboutDlg: public CDialog {
public:
	CAboutDlg( );

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );

// Implementation
protected:
	CString m_strVersion;
	CString m_strCopyright;

	DECLARE_MESSAGE_MAP( )
};
