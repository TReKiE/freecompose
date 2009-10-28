#pragma once

class COptionsDlg: public CDialog {
	DECLARE_DYNAMIC( COptionsDlg )

public:
	COptionsDlg( CWnd* pParent = NULL );
	virtual ~COptionsDlg( );

	enum { IDD = IDD_OPTIONS };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );

	DECLARE_MESSAGE_MAP( )

private:
	CTabCtrl m_Tabs;

	CButton m_btnOk;
	CButton m_btnApply;
	CButton m_btnClose;

	virtual BOOL OnInitDialog( );
	afx_msg void OnBnClickedOk( );
	afx_msg void OnBnClickedApply( );
	afx_msg void OnBnClickedCancel( );
};
