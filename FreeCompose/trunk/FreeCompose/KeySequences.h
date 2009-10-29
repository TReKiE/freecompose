#pragma once

class CKeySequences: public CPropertyPage {
	DECLARE_DYNAMIC( CKeySequences )

public:
	CKeySequences( );
	virtual ~CKeySequences( );

	// Dialog Data
	enum { IDD = IDD_KEYSEQUENCES };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );

	DECLARE_MESSAGE_MAP( )

private:
	int m_nColumnWidths[3];

	CListCtrl m_KeyComboList;
	CButton m_btnEdit;
	CButton m_btnRemove;

	void _FillKeyComboList( void );
	CString _VkToString( DWORD dw );

	virtual BOOL OnInitDialog( );
	afx_msg void OnKeyComboListItemChanged( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnBnClickedEdit( );
	afx_msg void OnBnClickedRemove( );
};
