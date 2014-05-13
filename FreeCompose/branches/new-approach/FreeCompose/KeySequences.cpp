#include "stdafx.h"

#include <Unicode.h>

#include "FreeCompose.h"
#include "KeySequences.h"
#include "ComposeSequenceEditor.h"

#include "Utils.h"

#if defined( _DEBUG ) && defined( TEST )

//==============================================================================
// TEST CODE

#include <ComposeSequenceTree.h>

#pragma warning( push )
#pragma warning( disable: 4189 4239 )

static class TestComposeSequenceTree {
public:
	TestComposeSequenceTree( ) {
		ComposeSequence composeSequences[] = {
			ComposeSequence( CString( L"A-" ),   CString( L"\u0100" ),       false, false, true  ),
			ComposeSequence( CString( L"a-" ),   CString( L"\u0101" ),       false, false, true  ),
			ComposeSequence( CString( L"a:-" ),  CString( L"\u00E4\u0304" ), false, false, false ),
			ComposeSequence( CString( L"a-:" ),  CString( L"\u0101\u0308" ), false, false, false ),
			ComposeSequence( CString( L"abcd" ), CString( L"\u0950" ),       false, true,  false ),
		};
		CComposeSequenceTree tree;
		tree.BuildTree( composeSequences, _countof( composeSequences ) );

		CString sequence, result;
		sequence = L"-A";
		bool f = tree.LookUp( sequence, result );
	}
} x_TestComposeSequenceTree;

#pragma warning( pop )

// END TEST CODE
//==============================================================================

#endif

//
// Constants
//

const int ITEM_FUDGE_FACTOR   = 12;
const int HEADER_FUDGE_FACTOR = ITEM_FUDGE_FACTOR + 3;

//
// CKeySequences declarations
//

BEGIN_MESSAGE_MAP( CKeySequences, CPropertyPage )
	ON_NOTIFY( NM_DBLCLK,       IDC_KEYCOMBOLIST, &CKeySequences::OnListDoubleClick )
	ON_NOTIFY( LVN_COLUMNCLICK, IDC_KEYCOMBOLIST, &CKeySequences::OnListColumnClick )
	ON_NOTIFY( LVN_ITEMCHANGED, IDC_KEYCOMBOLIST, &CKeySequences::OnListItemChanged )

	ON_BN_CLICKED( IDADD,    &CKeySequences::OnBnClickedAdd    )
	ON_BN_CLICKED( IDEDIT,   &CKeySequences::OnBnClickedEdit   )
	ON_BN_CLICKED( IDREMOVE, &CKeySequences::OnBnClickedRemove )
END_MESSAGE_MAP( )
IMPLEMENT_DYNAMIC( CKeySequences, CPropertyPage )

//
// CKeySequences static member variables
//

CKeySequences::sortcallbackfunc* CKeySequences::ResultColumnsSortFuncMap[] = {
	&CKeySequences::_ListComparer_Unsorted,
	&CKeySequences::_ListComparer_Ascending_Result,
	&CKeySequences::_ListComparer_Descending_Result,
};

CKeySequences::sortcallbackfunc* CKeySequences::SequenceColumnSortFuncMap[] = {
	&CKeySequences::_ListComparer_Unsorted,
	&CKeySequences::_ListComparer_Ascending_Sequence,
	&CKeySequences::_ListComparer_Descending_Sequence,
};

CKeySequences::sortcallbackfunc** CKeySequences::ColumnSortFuncMap[] = {
	ResultColumnsSortFuncMap,
	ResultColumnsSortFuncMap,
	SequenceColumnSortFuncMap,
};

int const ColumnHeaderFormatFlagsMap[] = {
	0,
	HDF_SORTUP,
	HDF_SORTDOWN
};

//
// CKeySequences constructors and destructors
//

CKeySequences::CKeySequences( COptionsData& Options ):
	CPropertyPage ( IDD ),
	m_Options     ( Options ),
	m_nSortColumn ( -1 ),
	m_SortState   ( ssUnsorted )
{

}

CKeySequences::~CKeySequences( ) {

}

//
// CKeySequences private methods
//

inline unsigned CKeySequences::_MakeComposeSequenceGroupKey( int const groupIndex, int const sequenceIndex ) {
	return ( ( groupIndex & 0xFFFF ) << 16 ) | ( sequenceIndex & 0xFFFF );
}

inline int CKeySequences::_GroupIndex( unsigned const key ) {
	return ( key >> 16 ) & 0xFFFF;
}

inline int CKeySequences::_SequenceIndex( unsigned const key ) {
	return key & 0xFFFF;
}

inline ComposeSequence& CKeySequences::_GetComposeSequence( unsigned const uKey ) {
	return m_Options.ComposeSequenceGroups[_GroupIndex( uKey )].ComposeSequences[_SequenceIndex( uKey )];
}

inline ComposeSequence& CKeySequences::_GetComposeSequenceFromListIndex( int const nItemIndex ) {
	return _GetComposeSequence( m_ListIndexMap[nItemIndex] );
}

inline CString CKeySequences::_FormatResultString( ComposeSequence const& sequence ) {
	CString strResult;
	unsigned chResult;
	Utf16ToUtf32( sequence.Result, chResult );
	strResult.Format( L"U+%06X", chResult );
	return strResult;
}

inline int CKeySequences::_MeasureListItemText( CString const& str ) {
	return m_List.GetStringWidth( str ) + ITEM_FUDGE_FACTOR;
}

inline void CKeySequences::_MeasureListItemStringsAndUpdate( CString const& strCodePoint, CString const& strCharacter, CString const& strSequence ) {
	m_nColumnWidths[0] = std::max( _MeasureListItemText( strCodePoint ), m_nColumnWidths[0] );
	m_nColumnWidths[1] = std::max( _MeasureListItemText( strCharacter ), m_nColumnWidths[1] );
	m_nColumnWidths[2] = std::max( _MeasureListItemText( strSequence  ), m_nColumnWidths[2] );
}

int CKeySequences::_AddOneKeySequence( ComposeSequence const& sequence, unsigned const csgKey ) {
	CString strResult( _FormatResultString( sequence ) );

	_MeasureListItemStringsAndUpdate( strResult, sequence.Result, sequence.Sequence );

	LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM | LVIF_GROUPID };
	lvItem.iItem = m_List.GetItemCount( );
	lvItem.iSubItem = 0;
	lvItem.pszText = const_cast<LPWSTR>( static_cast<LPCWSTR>( strResult ) );
	lvItem.lParam = static_cast<LPARAM>( csgKey );
	lvItem.iGroupId = _GroupIndex( csgKey );
	int nItemIndex = m_List.InsertItem( &lvItem );
	m_List.SetItem( nItemIndex, 1, LVIF_TEXT, sequence.Result,   0, 0, 0, 0 );
	m_List.SetItem( nItemIndex, 2, LVIF_TEXT, sequence.Sequence, 0, 0, 0, 0 );
	return nItemIndex;
}

void CKeySequences::_UpdateOneKeySequence( int const nItemIndex, ComposeSequence const& sequence ) {
	CString strResult( _FormatResultString( sequence ) );

	_MeasureListItemStringsAndUpdate( strResult, sequence.Result, sequence.Sequence );

	LVITEM lvItem = { LVIF_TEXT | LVIF_GROUPID };
	lvItem.iItem = nItemIndex;
	lvItem.pszText = const_cast<LPWSTR>( static_cast<LPCWSTR>( strResult ) );
	lvItem.iGroupId = _GroupIndex( m_ListIndexMap[nItemIndex] );
	m_List.SetItem( &lvItem );
	m_List.SetItem( nItemIndex, 1, LVIF_TEXT, sequence.Result,   0, 0, 0, 0 );
	m_List.SetItem( nItemIndex, 2, LVIF_TEXT, sequence.Sequence, 0, 0, 0, 0 );
}

void CKeySequences::_FillList( void ) {
	SetRedraw( FALSE );

	m_List.DeleteAllItems( );
	m_ListIndexMap.RemoveAll( );

	int groupCount = m_Options.ComposeSequenceGroups.GetCount( );
	for ( int groupIndex = 0; groupIndex < groupCount; groupIndex++ ) {
		int sequenceCount = m_Options.ComposeSequenceGroups[groupIndex].ComposeSequences.GetCount( );
		for ( int sequenceIndex = 0; sequenceIndex < sequenceCount; sequenceIndex++ ) {
			unsigned csgKey = _MakeComposeSequenceGroupKey( groupIndex, sequenceIndex );
			int nListIndex = _AddOneKeySequence( _GetComposeSequence( csgKey ), csgKey );
			m_ListIndexMap.SetAtGrow( nListIndex, csgKey );
		}
	}
	_SetColumnWidths( );

	SetRedraw( TRUE );
	Invalidate( );
	UpdateWindow( );
}

void CKeySequences::_SetColumnWidths( void ) {
	m_List.SetColumnWidth( 0, m_nColumnWidths[0] );
	m_List.SetColumnWidth( 1, m_nColumnWidths[1] );
	m_List.SetColumnWidth( 2, m_nColumnWidths[2] );
}

void CKeySequences::_SetColumnSortState( int nColumn, SORTSTATE state ) {
	HDITEM hdItem = { HDI_FORMAT, };
	if ( !m_pListHeader->GetItem( nColumn, &hdItem ) ) {
		debug( L"CKeySequences::_SetColumnSortState: GetItem failed?\n" );
		return;
	}
	hdItem.fmt = ( hdItem.fmt & ~( HDF_SORTUP | HDF_SORTDOWN ) ) | ColumnHeaderFormatFlagsMap[state];
	if ( !m_pListHeader->SetItem( nColumn, &hdItem ) ) {
		debug( L"CKeySequences::_SetColumnSortState: SetItem failed?\n" );
		return;
	}
}

//
// CKeySequences comparison functions, for the list control.
// Huh. Subtraction is so much easier, for certain kinds of comparisons.
//

int CKeySequences::_ListComparer_Unsorted( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	index1 = self->m_ListIndexMap[index1];
	index2 = self->m_ListIndexMap[index2];

	return static_cast<int>( index1 - index2 );
}

int CKeySequences::_ListComparer_Ascending_Result( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	CString& result1 = self->_GetComposeSequenceFromListIndex( index1 ).Result;
	CString& result2 = self->_GetComposeSequenceFromListIndex( index2 ).Result;

	return result1.Compare( result2 );
}

int CKeySequences::_ListComparer_Descending_Result( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	CString& result1 = self->_GetComposeSequenceFromListIndex( index1 ).Result;
	CString& result2 = self->_GetComposeSequenceFromListIndex( index2 ).Result;

	return result2.Compare( result1 );
}

int CKeySequences::_ListComparer_Ascending_Sequence( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	CString& sequence1 = self->_GetComposeSequenceFromListIndex( index1 ).Sequence;
	CString& sequence2 = self->_GetComposeSequenceFromListIndex( index2 ).Sequence;

	return sequence1.CompareNoCase( sequence2 );
}

int CKeySequences::_ListComparer_Descending_Sequence( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	CString& sequence1 = self->_GetComposeSequenceFromListIndex( index1 ).Sequence;
	CString& sequence2 = self->_GetComposeSequenceFromListIndex( index2 ).Sequence;

	return sequence2.CompareNoCase( sequence1 );
}

//
// CKeySequences implementation
//

void CKeySequences::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );

	DDX_Control( pDX, IDC_KEYCOMBOLIST, m_List      );
	DDX_Control( pDX, IDADD,            m_btnAdd    );
	DDX_Control( pDX, IDEDIT,           m_btnEdit   );
	DDX_Control( pDX, IDREMOVE,         m_btnRemove );
}

BOOL CKeySequences::OnInitDialog( ) {
	if ( !CPropertyPage::OnInitDialog( ) ) {
		return FALSE;
	}

	//
	// Store pointer to list's header control for later use
	//

	m_pListHeader = m_List.GetHeaderCtrl( );

	//
	// Configure the list's appearance and behavior
	//

	m_List.ModifyStyle( 0, LVS_ALIGNTOP, 0 );

	DWORD dwExtendedStyles = LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP | LVS_EX_TWOCLICKACTIVATE;
	if ( g_CommonControlsApiVersion >= COMCTL32APIVER_WINXP ) {
		dwExtendedStyles |= LVS_EX_DOUBLEBUFFER;
	}
	if ( g_CommonControlsApiVersion >= COMCTL32APIVER_VISTA ) {
		dwExtendedStyles |= LVS_EX_COLUMNSNAPPOINTS;
	}
	m_List.SetExtendedStyle( m_List.GetExtendedStyle( ) | dwExtendedStyles );

	//
	// Configure our three columns
	//

	CString strLabels[] = {
		LoadFromStringTable( IDS_KEYSEQUENCES_RESULT_CODEPOINT ),
		LoadFromStringTable( IDS_KEYSEQUENCES_RESULT_CHARACTER ),
		LoadFromStringTable( IDS_KEYSEQUENCES_SEQUENCE ),
	};

	int widths[3];
	for ( int n = 0; n < 3; n++ ) {
		widths[n] = m_List.GetStringWidth( strLabels[n] ) + HEADER_FUDGE_FACTOR;
		m_List.InsertColumn( n, strLabels[n], LVCFMT_LEFT, widths[n] );
	}
	m_nColumnWidths[0] = widths[0];
	m_nColumnWidths[1] = widths[1];
	m_nColumnWidths[2] = widths[2];

	//
	// Configure groups
	//

	if ( g_CommonControlsApiVersion >= COMCTL32APIVER_WINXP ) {
		LVGROUP lv = { sizeof( LVGROUP ), LVGF_HEADER | LVGF_FOOTER | LVGF_GROUPID | LVGF_ALIGN };
		CString tmp;

		int groupCount = m_Options.ComposeSequenceGroups.GetCount( );
		for ( int groupIndex = 0; groupIndex < groupCount; groupIndex++ ) {
			tmp.Format( L"Top of compose sequence group #%d: '%s'", groupIndex, m_Options.ComposeSequenceGroups[groupIndex] );
			wcscpy_s( lv.pszHeader, lv.cchHeader, static_cast<LPCWSTR>( tmp ) );

			tmp.Format( L"Bottom of Compose sequence group #%d: '%s'", groupIndex, m_Options.ComposeSequenceGroups[groupIndex] );
			wcscpy_s( lv.pszFooter, lv.cchFooter, static_cast<LPCWSTR>( tmp ) );

			lv.iGroupId = groupIndex;
			lv.uAlign = LVGA_HEADER_LEFT | LVGA_FOOTER_LEFT;

			int ret = m_List.SetGroupInfo( groupIndex, &lv );
			debug( L"CKeySequences::OnInitDialog: group #%d '%s' ret=%d\n", groupIndex, m_Options.ComposeSequenceGroups[groupIndex].Name, ret );
		}

		m_List.EnableGroupView( TRUE );
	}

	//
	// Fill the list with compose sequences
	//

	_FillList( );
	
	return TRUE;
}

void CKeySequences::OnListItemChanged( NMHDR* /*pnmhdr*/, LRESULT* pResult ) {
	UINT nsel = m_List.GetSelectedCount( );
	if ( nsel ) {
		m_btnEdit.EnableWindow( ( 1 == nsel ) );
		m_btnRemove.EnableWindow( TRUE );
	} else {
		m_btnEdit.EnableWindow( FALSE );
		m_btnRemove.EnableWindow( FALSE );
	}

	*pResult = 0;
}

void CKeySequences::OnListDoubleClick( NMHDR* /*pnmhdr*/, LRESULT* pResult ) {
	OnBnClickedEdit( );

	*pResult = 0;
}

void CKeySequences::OnListColumnClick( NMHDR* pnmhdr, LRESULT* pResult ) {
	NMLISTVIEW* pnmlv = reinterpret_cast<NMLISTVIEW*>( pnmhdr );

	int nPrevSortColumn = m_nSortColumn;
	m_nSortColumn = pnmlv->iSubItem;
	if ( m_nSortColumn == nPrevSortColumn ) {
		m_SortState = static_cast<SORTSTATE>( ( m_SortState + 1 ) % 3 );
	} else {
		m_SortState = ssAscending;
	}
	debug( L"CKeySequences::OnListColumnClick: column %d sortstate %d self 0x%p\n", m_nSortColumn, m_SortState, this );

	//
	// Step 1: tell combo box to sort itself
	//

	_ASSERTE( m_nSortColumn >= 0 && m_nSortColumn < NumberOfColumns );
	_ASSERTE( m_SortState >= ssUnsorted && m_SortState < ssMaximumValue );
	PFNLVCOMPARE pfnCompare = ColumnSortFuncMap[m_nSortColumn][m_SortState];
	if ( !pfnCompare ) {
		debug( L"CKeySequences::OnListColumnClick: problem with ColumnSortFuncMap[column %d][state %d]: it's nullptr?\n", m_nSortColumn, m_SortState );
		return;
	}

	if ( !m_List.SortItemsEx( pfnCompare, reinterpret_cast<DWORD_PTR>( this ) ) ) {
		debug( L"CKeySequences::OnListColumnClick: SortItemsEx failed?\n" );
	}

	m_ListIndexMap.RemoveAll( );
	int count = m_List.GetItemCount( );
	for ( int n = 0; n < count; n++ ) {
		m_ListIndexMap.SetAtGrow( n, static_cast<unsigned>( m_List.GetItemData( n ) ) );
	}

	//
	// Step 2: get the header control to display the appropriate indication
	//

	if ( -1 != nPrevSortColumn ) {
		_SetColumnSortState( nPrevSortColumn, ssUnsorted );
	}
	_SetColumnSortState( m_nSortColumn, m_SortState );

	Invalidate( );
	UpdateWindow( );

	*pResult = 0;
}

void CKeySequences::OnBnClickedAdd( ) {
	ComposeSequence sequence;
	CComposeSequenceEditor edit( sequence, semAdd, this );

	INT_PTR rc = edit.DoModal( );
	if ( IDOK == rc ) {
		int groupCount = m_Options.ComposeSequenceGroups.GetCount( );
		int focusedGroup = -1;
		for ( int groupIndex = 0; groupIndex < groupCount; groupIndex++ ) {
			LVGROUP lv = { sizeof( LVGROUP ), LVGF_STATE };
			lv.stateMask = LVGS_FOCUSED;
			int ret = m_List.GetGroupInfo( groupIndex, &lv );
			if ( ( lv.state & LVGS_FOCUSED ) != 0 ) {
				focusedGroup = groupIndex;
				break;
			}
		}
		if ( -1 == focusedGroup ) {
			focusedGroup = 0;
		}

		SetRedraw( FALSE );

		unsigned key = _MakeComposeSequenceGroupKey( focusedGroup, m_Options.ComposeSequenceGroups[focusedGroup].ComposeSequences.Add( sequence ) );
		_AddOneKeySequence( sequence, key );
		_SetColumnWidths( );

		SetModified( );

		SetRedraw( TRUE );
		Invalidate( );
		UpdateWindow( );
	}
}

void CKeySequences::OnBnClickedEdit( ) {
	if ( m_List.GetSelectedCount( ) < 1 ) {
		return;
	}

	POSITION pos = m_List.GetFirstSelectedItemPosition( );
	if ( !pos ) {
		return;
	}

	int nItem = m_List.GetNextSelectedItem( pos );
	ComposeSequence sequence( _GetComposeSequenceFromListIndex( nItem ) );
	CComposeSequenceEditor edit( sequence, semEdit, this );
	INT_PTR rc = edit.DoModal( );
	if ( IDOK == rc ) {
		_GetComposeSequenceFromListIndex( nItem ) = sequence;

		SetRedraw( FALSE );

		_UpdateOneKeySequence( nItem, sequence );
		_SetColumnWidths( );

		SetModified( );

		SetRedraw( TRUE );
		Invalidate( );
		UpdateWindow( );
	}
}

void CKeySequences::OnBnClickedRemove( ) {
	UINT count = m_List.GetSelectedCount( );
	if ( count < 1 ) {
		return;
	}

	int ret = MessageBox( LoadFromStringTable( IDS_KEYSEQUENCES_CONFIRM_PROMPT ), LoadFromStringTable( IDS_KEYSEQUENCES_CONFIRM_TITLE ), MB_YESNO | MB_ICONWARNING );
	if ( IDYES != ret ) {
		return;
	}

	POSITION pos = m_List.GetFirstSelectedItemPosition( );
	int* items = new int[count];
	unsigned* indices = new unsigned[count];
	unsigned n = 0;

	int i = m_List.GetNextSelectedItem( pos );
	while ( ( -1 != i ) && ( n < count ) ) {
		items[n] = i;
		indices[n] = m_ListIndexMap[i];
		n++;
		i = m_List.GetNextSelectedItem( pos );
	}
	count = n;
	qsort_s( items,   count, sizeof( int ),      compare_keys_reverse, nullptr );
	qsort_s( indices, count, sizeof( unsigned ), compare_keys_reverse, nullptr );

	SetRedraw( FALSE );

	// TODO TEST
	n = count;
	do {
		n--;
		m_List.DeleteItem( items[n] );
		m_Options.ComposeSequenceGroups[_GroupIndex( indices[n] )].ComposeSequences.RemoveAt( _SequenceIndex( indices[n] ) );
	} while ( n > 0 );

	delete[] items;
	delete[] indices;

	SetModified( );
	
	SetRedraw( TRUE );
	Invalidate( );
	UpdateWindow( );
}
