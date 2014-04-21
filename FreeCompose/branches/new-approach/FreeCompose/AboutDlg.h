#pragma once

class CAboutDlg: public CDialog {
	DECLARE_MESSAGE_MAP( )

public:
	enum { IDD = IDD_ABOUTBOX };

	CAboutDlg( );

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );

	// Implementation
protected:
	CString m_strVersion;
	CString m_strCopyright;
};
