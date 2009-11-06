#pragma once

#include "OptionsData.h"

class CKeySequences: public CPropertyPage {
public:
	enum { IDD = IDD_KEYSEQUENCES };

	CKeySequences( COptionsData& Options );
	virtual ~CKeySequences( );

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );
	virtual BOOL PreTranslateMessage( MSG* pMsg );

	afx_msg void OnKeyComboListItemChanged( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnKeyComboListDoubleClick( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnBnClickedAdd( );
	afx_msg void OnBnClickedEdit( );
	afx_msg void OnBnClickedRemove( );

protected:
	DECLARE_MESSAGE_MAP( )

private:
	COptionsData& m_Options;

	int m_nColumnWidths[2];

	CListCtrl m_KeyComboList;
	CButton m_btnAdd;
	CButton m_btnEdit;
	CButton m_btnRemove;

	void _DoAddOneKeySequence( const int n );
	void _DoUpdateOneKeySequence( const int n );
	void _AdjustColumns( void );
	void _FillKeyComboList( void );
	void _AddNewKeySequence( const int n );

	DECLARE_DYNAMIC( CKeySequences )
};
