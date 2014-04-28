#pragma once

#include "OptionsData.h"

class CKeySequences: public CPropertyPage {
public:
	enum { IDD = IDD_KEYSEQUENCES };

	CKeySequences( COptionsData& Options );
	virtual ~CKeySequences( );

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );

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

	CString _FormatResultString( ComposeSequence const& sequence );
	void _AddOneKeySequence( const INT_PTR n );
	void _UpdateOneKeySequence( const INT_PTR n );
	void _AdjustColumns( void );
	void _FillKeyComboList( void );
	void _AddNewKeySequence( const INT_PTR n );

	DECLARE_DYNAMIC( CKeySequences )
};
