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
	// Constants
	static int const ResultCodePointColumn   = 0;
	static int const ResultCharacterColumn   = 1;
	static int const SequenceColumn          = 2;
	static int const EnabledColumn           = 3;
	static int const CaseInsensitiveColumn   = 4;
	static int const ReversibleColumn        = 5;
	static int const NumberOfColumns         = 6;

	using sortcallbackfunc = int CALLBACK ( LPARAM, LPARAM, LPARAM );
	static sortcallbackfunc* ResultColumnsSortFuncMap[ssMaximumValue];
	static sortcallbackfunc* SequenceColumnSortFuncMap[ssMaximumValue];
	static sortcallbackfunc* EnabledColumnSortFuncMap[ssMaximumValue];
	static sortcallbackfunc* CaseInsensitiveColumnSortFuncMap[ssMaximumValue];
	static sortcallbackfunc* ReversibleColumnSortFuncMap[ssMaximumValue];
	static sortcallbackfunc** ColumnSortFuncMap[NumberOfColumns];
	static int const ColumnHeaderFormatFlagsMap[3];

	// Member variables
	COptionsData& m_Options;

	int m_nColumnWidths[NumberOfColumns];

	CListCtrl m_List;
	CButton m_btnAdd;
	CButton m_btnEdit;
	CButton m_btnRemove;

	SORTSTATE m_SortState;
	int m_nSortColumn;
	CArray<unsigned> m_ListIndexMap;

	// Member functions
	unsigned _MakeComposeSequenceGroupKey( INT_PTR const groupIndex, INT_PTR const sequenceIndex );
	int _GroupIndex( unsigned const key );
	int _SequenceIndex( unsigned const key );
	ComposeSequence& _GetComposeSequence( unsigned const uKey );
	ComposeSequence& _GetComposeSequenceFromListIndex( INT_PTR const nItemIndex );
	int _MeasureListItemText( CString const& str );
	void _MeasureListItemStringsAndUpdate( CString const& strCodePoint, CString const& strCharacter, CString const& strSequence, CString const& strEnabled, CString const& strCaseInsensitive, CString const& strReversible );
	void _AddGroup( int const groupIndex );
	void _UpdateGroup( int const groupIndex );
	int _AddComposeSequence( ComposeSequence const& composeSequence, unsigned const csgkey );
	void _UpdateComposeSequence( int const nItemIndex, ComposeSequence const& sequence );
	void _FillList( void );
	void _SetColumnWidths( void );
	void _SetColumnSortState( int nColumn, SORTSTATE state );
	void _DoWithRedrawDisabled( std::function<void( void )> func );
	void _RefreshListIndexMap( void );

	// Sorting functions
	static int CALLBACK _ListComparer_Unsorted( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Ascending_Result( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Descending_Result( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Ascending_Sequence( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Descending_Sequence( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Ascending_Enabled( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Descending_Enabled( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Ascending_CaseInsensitive( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Descending_CaseInsensitive( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Ascending_Reversible( LPARAM index1, LPARAM index2, LPARAM );
	static int CALLBACK _ListComparer_Descending_Reversible( LPARAM index1, LPARAM index2, LPARAM );

};
