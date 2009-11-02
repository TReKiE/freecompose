#pragma once

#include "OptionsData.h"

class CKeySequences: public CPropertyPage {
	DECLARE_DYNAMIC( CKeySequences )

public:
	CKeySequences( COptionsData& Options );
	virtual ~CKeySequences( );

	// Dialog Data
	enum { IDD = IDD_KEYSEQUENCES };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );

	DECLARE_MESSAGE_MAP( )

private:
	COptionsData& m_Options;

	int m_nColumnWidths[3];

	CListCtrl m_KeyComboList;
	CButton m_btnEdit;
	CButton m_btnRemove;

	void _FillKeyComboList( void );

	virtual BOOL OnInitDialog( );

	afx_msg void OnKeyComboListItemChanged( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnBnClickedEdit( );
	afx_msg void OnBnClickedRemove( );
};
