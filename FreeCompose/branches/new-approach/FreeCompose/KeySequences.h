#pragma once

#include "OptionsData.h"

enum SORTSTATE {
	ssUnsorted,
	ssAscending,
	ssDescending,
	ssMaximumValue,
};

class CKeySequences: public CPropertyPage {

	DECLARE_DYNAMIC( CKeySequences )
	DECLARE_MESSAGE_MAP( )

public:
	enum { IDD = IDD_KEYSEQUENCES };

	CKeySequences( COptionsData& Options );
	virtual ~CKeySequences( );

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );

	afx_msg void OnListColumnClick( NMHDR* pnmhdr, LRESULT* pResult );
	afx_msg void OnListDoubleClick( NMHDR* pnmhdr, LRESULT* pResult );
	afx_msg void OnListItemChanged( NMHDR* pnmhdr, LRESULT* pResult );

	afx_msg void OnBnClickedAdd( );
	afx_msg void OnBnClickedEdit( );
	afx_msg void OnBnClickedRemove( );

private:
	static int const ResultCodePointColumn = 0;
	static int const ResultCharacterColumn = 1;
	static int const SequenceColumn = 2;
	static int const NumberOfColumns = 3;

	using sortcallbackfunc = int CALLBACK ( LPARAM, LPARAM, LPARAM );
	static sortcallbackfunc* ResultColumnsSortFuncMap[ssMaximumValue];
	static sortcallbackfunc* SequenceColumnSortFuncMap[ssMaximumValue];
	static sortcallbackfunc** ColumnSortFuncMap[NumberOfColumns];

private:
	COptionsData& m_Options;

	int m_nColumnWidths[NumberOfColumns];

	CListCtrl m_List;
	CHeaderCtrl* m_pListHeader;
	CButton m_btnAdd;
	CButton m_btnEdit;
	CButton m_btnRemove;

	SORTSTATE m_SortState;
	int m_nSortColumn;
	CArray<unsigned> m_ListIndexMap;

private:
	ComposeSequence& _GetComposeSequence( unsigned const uKey );
	ComposeSequence& _GetComposeSequenceFromListIndex( int const nItemIndex );
	CString _FormatResultString( ComposeSequence const& sequence );
	int _MeasureListItemText( CString const& str );
	void _MeasureListItemStringsAndUpdate( CString const& strCodePoint, CString const& strCharacter, CString const& strSequence );
	int _AddOneKeySequence( ComposeSequence const& composeSequence, unsigned const csgkey );
	void _UpdateOneKeySequence( int const nItemIndex, ComposeSequence const& sequence );
	void _FillKeyComboList( void );
	void _SetColumnWidths( void );
	void _SetColumnSortState( int nColumn, SORTSTATE state );

private:
	static int CALLBACK _ListComparer_Unsorted( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Ascending_Result( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Descending_Result( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Ascending_Sequence( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Descending_Sequence( LPARAM index1, LPARAM index2, LPARAM );

};
