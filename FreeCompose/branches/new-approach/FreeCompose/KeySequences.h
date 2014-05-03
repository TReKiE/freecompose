#pragma once

#include "OptionsData.h"

enum SORTSTATE { ssUnsorted, ssAscending, ssDescending };

class CKeySequences: public CPropertyPage {
	DECLARE_DYNAMIC( CKeySequences )
	DECLARE_MESSAGE_MAP( )

public:
	enum { IDD = IDD_KEYSEQUENCES };

	CKeySequences( COptionsData& Options );
	virtual ~CKeySequences( );

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );

	afx_msg void OnKeyComboListColumnClick( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnKeyComboListDoubleClick( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnKeyComboListItemChanged( NMHDR* pNMHDR, LRESULT* pResult );

	afx_msg void OnBnClickedAdd( );
	afx_msg void OnBnClickedEdit( );
	afx_msg void OnBnClickedRemove( );

private:
	COptionsData& m_Options;

	int m_nColumnWidths[2];

	CListCtrl m_KeyComboList;
	CButton m_btnAdd;
	CButton m_btnEdit;
	CButton m_btnRemove;

	SORTSTATE m_SortState;
	int m_nSortColumn;

	CString _FormatResultString( ComposeSequence const& sequence );
	void _AddOneKeySequence( const INT_PTR n );
	void _UpdateOneKeySequence( const INT_PTR n );
	void _AdjustColumns( void );
	void _FillKeyComboList( void );
	void _AddNewKeySequence( const INT_PTR n );

	static int CALLBACK _ListComparer_Unsorted( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Ascending_Result( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Descending_Result( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Ascending_Sequence( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Descending_Sequence( LPARAM index1, LPARAM index2, LPARAM );

	using sortcallbackfunc = int CALLBACK ( LPARAM, LPARAM, LPARAM );
	static sortcallbackfunc* ResultColumnSortFuncMap[3];
	static sortcallbackfunc* SequenceColumnSortFuncMap[3];

	static int const ResultColumn = 0;
	static int const SequenceColumn = 1;

};
