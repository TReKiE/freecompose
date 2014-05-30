#pragma once

class CAboutDlg: public CDialogEx {
	DECLARE_DYNAMIC( CAboutDlg )
	DECLARE_MESSAGE_MAP( )

public:
	enum { IDD = IDD_ABOUT };

	CAboutDlg( );
	virtual ~CAboutDlg( );

private:
	// Member variables
	CString m_strVersion;
	CString m_strName;

	CStatic m_staticName;
	CLinkCtrl m_linkCopyright;
	CLinkCtrl m_linkWebsite;
	CLinkCtrl m_linkLicense;

	CFont* m_pFont;

	// Methods
	void _GetVersionInfo( void );
	void _LaunchUrl( wchar_t const* pwzUrl );

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );

	afx_msg void OnClickCopyright( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnClickWebsite( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnClickLicense( NMHDR* pNMHDR, LRESULT* pResult );

};
