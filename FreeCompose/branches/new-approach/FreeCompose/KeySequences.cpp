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
// CKeySequences declarations
//

BEGIN_MESSAGE_MAP( CKeySequences, CPropertyPage )
	ON_NOTIFY( NM_DBLCLK,       IDC_KS_COMPOSE_SEQUENCE_LIST, &CKeySequences::OnListDoubleClick )
	ON_NOTIFY( LVN_COLUMNCLICK, IDC_KS_COMPOSE_SEQUENCE_LIST, &CKeySequences::OnListColumnClick )
	ON_NOTIFY( LVN_ITEMCHANGED, IDC_KS_COMPOSE_SEQUENCE_LIST, &CKeySequences::OnListItemChanged )

	ON_BN_CLICKED( IDADD,    &CKeySequences::OnBnClickedAdd    )
	ON_BN_CLICKED( IDEDIT,   &CKeySequences::OnBnClickedEdit   )
	ON_BN_CLICKED( IDREMOVE, &CKeySequences::OnBnClickedRemove )
END_MESSAGE_MAP( )
IMPLEMENT_DYNAMIC( CKeySequences, CPropertyPage )

//
// Constants
//

int const ITEM_FUDGE_FACTOR   = 12;
int const HEADER_FUDGE_FACTOR = ITEM_FUDGE_FACTOR + 3;

wchar_t const BooleanToGlyph[2] = { L'\u2718', L'\u2714' };

//
// Types
//

class GroupAndSequenceIndex {
public:
	GroupAndSequenceIndex( ): nGroup( -1 ), nSequence( -1 ) { }

	GroupAndSequenceIndex( int g, int s ): nGroup( g ), nSequence( s ) { }

	bool operator>( GroupAndSequenceIndex const& rhs ) {
		return ( nGroup > rhs.nGroup || ( nGroup == rhs.nGroup && nSequence > rhs.nSequence ) );
	}

	bool operator<( GroupAndSequenceIndex const& rhs ) {
		return ( nGroup < rhs.nGroup || ( nGroup == rhs.nGroup && nSequence < rhs.nSequence ) );
	}

	static int CompareReverse( void* /*context*/, void const* _elem1, void const* _elem2 ) {
		GroupAndSequenceIndex* elem1 = static_cast<GroupAndSequenceIndex*>( const_cast<void*>( _elem1 ) );
		GroupAndSequenceIndex* elem2 = static_cast<GroupAndSequenceIndex*>( const_cast<void*>( _elem2 ) );

		if ( *elem1 > *elem2 )
			return -1;
		else if ( *elem1 < *elem2 )
			return 1;
		else
			return 0;
	}

	int nGroup;
	int nSequence;
};

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

CKeySequences::sortcallbackfunc* CKeySequences::EnabledColumnSortFuncMap[] = {
	&CKeySequences::_ListComparer_Unsorted,
	&CKeySequences::_ListComparer_Ascending_Enabled,
	&CKeySequences::_ListComparer_Descending_Enabled,
};

CKeySequences::sortcallbackfunc* CKeySequences::CaseInsensitiveColumnSortFuncMap[] = {
	&CKeySequences::_ListComparer_Unsorted,
	&CKeySequences::_ListComparer_Ascending_CaseInsensitive,
	&CKeySequences::_ListComparer_Descending_CaseInsensitive,
};

CKeySequences::sortcallbackfunc* CKeySequences::ReversibleColumnSortFuncMap[] = {
	&CKeySequences::_ListComparer_Unsorted,
	&CKeySequences::_ListComparer_Ascending_Reversible,
	&CKeySequences::_ListComparer_Descending_Reversible,
};

CKeySequences::sortcallbackfunc** CKeySequences::ColumnSortFuncMap[] = {
	EnabledColumnSortFuncMap,
	ResultColumnsSortFuncMap,
	ResultColumnsSortFuncMap,
	SequenceColumnSortFuncMap,
	CaseInsensitiveColumnSortFuncMap,
	ReversibleColumnSortFuncMap,
};

int const CKeySequences::ColumnHeaderFormatFlagsMap[] = {
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
	m_SortState   ( ssUnsorted ),
	m_nSortColumn ( -1 )
{

}

CKeySequences::~CKeySequences( ) {

}

//
// CKeySequences private methods
//

inline unsigned CKeySequences::_MakeComposeSequenceGroupKey( INT_PTR const groupIndex, INT_PTR const sequenceIndex ) {
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

inline ComposeSequence& CKeySequences::_GetComposeSequenceFromListIndex( INT_PTR const nItemIndex ) {
	return _GetComposeSequence( m_ListIndexMap[nItemIndex] );
}

inline int CKeySequences::_MeasureListItemText( CString const& str ) {
	return m_List.GetStringWidth( str ) + ITEM_FUDGE_FACTOR;
}

inline void CKeySequences::_MeasureListItemStringsAndUpdate( CString const& strCodePoint, CString const& strCharacter, CString const& strSequence, CString const& strEnabled, CString const& strCaseInsensitive, CString const& strReversible ) {
	m_nColumnWidths[0] = std::max( _MeasureListItemText( strEnabled         ), m_nColumnWidths[0] );
	m_nColumnWidths[1] = std::max( _MeasureListItemText( strCodePoint       ), m_nColumnWidths[1] );
	m_nColumnWidths[2] = std::max( _MeasureListItemText( strCharacter       ), m_nColumnWidths[2] );
	m_nColumnWidths[3] = std::max( _MeasureListItemText( strSequence        ), m_nColumnWidths[3] );
	m_nColumnWidths[4] = std::max( _MeasureListItemText( strCaseInsensitive ), m_nColumnWidths[4] );
	m_nColumnWidths[5] = std::max( _MeasureListItemText( strReversible      ), m_nColumnWidths[5] );
}

void CKeySequences::_AddGroup( int const groupIndex ) {
	LVGROUP lv = {
		/* cbSize    */ sizeof( LVGROUP ),
		/* mask      */ LVGF_HEADER | LVGF_GROUPID | LVGF_STATE | LVGF_ALIGN,
		/* pszHeader */ const_cast<LPWSTR>( static_cast<LPCWSTR>( m_Options.ComposeSequenceGroups[groupIndex].Name ) ),
		/* cchHeader */ 0,
		/* pszFooter */ nullptr,
		/* cchFooter */ 0,
		/* iGroupId  */ groupIndex,
		/* stateMask */ LVGS_COLLAPSIBLE,
		/* state     */ LVGS_COLLAPSIBLE,
		/* uAlign    */ LVGA_HEADER_LEFT | LVGA_FOOTER_LEFT
	};
	m_List.InsertGroup( groupIndex, &lv );
}

void CKeySequences::_UpdateGroup( int const groupIndex ) {
	LVGROUP lv = { sizeof( LVGROUP ), LVGF_HEADER | LVGF_GROUPID, };
	lv.pszHeader = const_cast<LPWSTR>( static_cast<LPCWSTR>( m_Options.ComposeSequenceGroups[groupIndex].Name ) );
	lv.iGroupId = groupIndex;
	m_List.SetGroupInfo( groupIndex, &lv );
}

int CKeySequences::_AddComposeSequence( ComposeSequence const& sequence, unsigned const csgKey ) {
	CString strResult          ( FormatCodePoint( sequence.Result ) );
	CString strEnabled         ( BooleanToGlyph[!sequence.Disabled      ] );
	CString strCaseInsensitive ( BooleanToGlyph[sequence.CaseInsensitive] );
	CString strReversible      ( BooleanToGlyph[sequence.Reversible     ] );
	_MeasureListItemStringsAndUpdate( strResult, sequence.Result, sequence.Sequence, strEnabled, strCaseInsensitive, strReversible );

	LVITEM lvItem = { LVIF_TEXT | LVIF_PARAM | LVIF_GROUPID };
	lvItem.iItem = m_List.GetItemCount( );
	lvItem.iSubItem = 0;
	lvItem.pszText = const_cast<LPWSTR>( static_cast<LPCWSTR>( strEnabled ) );
	lvItem.lParam = static_cast<LPARAM>( csgKey );
	lvItem.iGroupId = _GroupIndex( csgKey );
	int nItemIndex = m_List.InsertItem( &lvItem );
	m_List.SetItem( nItemIndex, 1, LVIF_TEXT, strResult,          0, 0, 0, 0 );
	m_List.SetItem( nItemIndex, 2, LVIF_TEXT, sequence.Result,    0, 0, 0, 0 );
	m_List.SetItem( nItemIndex, 3, LVIF_TEXT, sequence.Sequence,  0, 0, 0, 0 );
	m_List.SetItem( nItemIndex, 4, LVIF_TEXT, strCaseInsensitive, 0, 0, 0, 0 );
	m_List.SetItem( nItemIndex, 5, LVIF_TEXT, strReversible,      0, 0, 0, 0 );
	return nItemIndex;
}

void CKeySequences::_UpdateComposeSequence( int const nItemIndex, ComposeSequence const& sequence ) {
	CString strResult          ( FormatCodePoint( sequence.Result ) );
	CString strEnabled         ( BooleanToGlyph[!sequence.Disabled      ] );
	CString strCaseInsensitive ( BooleanToGlyph[sequence.CaseInsensitive] );
	CString strReversible      ( BooleanToGlyph[sequence.Reversible     ] );
	_MeasureListItemStringsAndUpdate( strResult, sequence.Result, sequence.Sequence, strEnabled, strCaseInsensitive, strReversible );

	LVITEM lvItem = { LVIF_TEXT | LVIF_GROUPID };
	lvItem.iItem = nItemIndex;
	lvItem.pszText = const_cast<LPWSTR>( static_cast<LPCWSTR>( strEnabled ) );
	lvItem.iGroupId = _GroupIndex( m_ListIndexMap[nItemIndex] );
	m_List.SetItem( &lvItem );
	m_List.SetItem( nItemIndex, 1, LVIF_TEXT, strResult,          0, 0, 0, 0 );
	m_List.SetItem( nItemIndex, 2, LVIF_TEXT, sequence.Result,    0, 0, 0, 0 );
	m_List.SetItem( nItemIndex, 3, LVIF_TEXT, sequence.Sequence,  0, 0, 0, 0 );
	m_List.SetItem( nItemIndex, 4, LVIF_TEXT, strCaseInsensitive, 0, 0, 0, 0 );
	m_List.SetItem( nItemIndex, 5, LVIF_TEXT, strReversible,      0, 0, 0, 0 );
}

void CKeySequences::_FillList( void ) {
	m_ListIndexMap.RemoveAll( );
	_DoWithRedrawDisabled( [&]( ) {
		m_List.DeleteAllItems( );

		INT_PTR groupCount = m_Options.ComposeSequenceGroups.GetCount( );
		for ( int groupIndex = 0; groupIndex < groupCount; groupIndex++ ) {
			INT_PTR sequenceCount = m_Options.ComposeSequenceGroups[groupIndex].ComposeSequences.GetCount( );
			for ( int sequenceIndex = 0; sequenceIndex < sequenceCount; sequenceIndex++ ) {
				unsigned csgKey = _MakeComposeSequenceGroupKey( groupIndex, sequenceIndex );
				int nListIndex = _AddComposeSequence( _GetComposeSequence( csgKey ), csgKey );
				m_ListIndexMap.SetAtGrow( nListIndex, csgKey );
			}
		}
		_SetColumnWidths( );
	} );
}

void CKeySequences::_SetColumnWidths( void ) {
	for ( int index = 0; index < NumberOfColumns; index++ ) {
		m_List.SetColumnWidth( index, m_nColumnWidths[index] );
	}
}

void CKeySequences::_SetColumnSortState( int nColumn, SORTSTATE state ) {
	CHeaderCtrl* pListHeader = m_List.GetHeaderCtrl( );

	HDITEM hdItem = { HDI_FORMAT, };
	if ( !pListHeader->GetItem( nColumn, &hdItem ) ) {
		debug( L"CKeySequences::_SetColumnSortState: GetItem failed?\n" );
		return;
	}

	hdItem.fmt = ( hdItem.fmt & ~( HDF_SORTUP | HDF_SORTDOWN ) ) | ColumnHeaderFormatFlagsMap[state];
	if ( !pListHeader->SetItem( nColumn, &hdItem ) ) {
		debug( L"CKeySequences::_SetColumnSortState: SetItem failed?\n" );
		return;
	}
}

void CKeySequences::_DoWithRedrawDisabled( std::function<void( void )> func ) {
	SetRedraw( FALSE );

	func( );

	SetRedraw( TRUE );
	Invalidate( );
	UpdateWindow( );
}

void CKeySequences::_RefreshListIndexMap( void ) {
	m_ListIndexMap.RemoveAll( );
	int count = m_List.GetItemCount( );
	for ( int n = 0; n < count; n++ ) {
		m_ListIndexMap.SetAtGrow( n, static_cast<unsigned>( m_List.GetItemData( n ) ) );
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

int CKeySequences::_ListComparer_Ascending_Enabled( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool enabled1 = !self->_GetComposeSequenceFromListIndex( index1 ).Disabled;
	bool enabled2 = !self->_GetComposeSequenceFromListIndex( index2 ).Disabled;

	return enabled1 > enabled2;
}

int CKeySequences::_ListComparer_Descending_Enabled( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool enabled1 = !self->_GetComposeSequenceFromListIndex( index1 ).Disabled;
	bool enabled2 = !self->_GetComposeSequenceFromListIndex( index2 ).Disabled;

	return enabled2 > enabled1;
}

int CKeySequences::_ListComparer_Ascending_CaseInsensitive( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool caseInsensitive1 = !self->_GetComposeSequenceFromListIndex( index1 ).CaseInsensitive;
	bool caseInsensitive2 = !self->_GetComposeSequenceFromListIndex( index2 ).CaseInsensitive;

	return caseInsensitive1 > caseInsensitive2;
}

int CKeySequences::_ListComparer_Descending_CaseInsensitive( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool caseInsensitive1 = !self->_GetComposeSequenceFromListIndex( index1 ).CaseInsensitive;
	bool caseInsensitive2 = !self->_GetComposeSequenceFromListIndex( index2 ).CaseInsensitive;

	return caseInsensitive2 > caseInsensitive1;
}

int CKeySequences::_ListComparer_Ascending_Reversible( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool reversible1 = !self->_GetComposeSequenceFromListIndex( index1 ).Reversible;
	bool reversible2 = !self->_GetComposeSequenceFromListIndex( index2 ).Reversible;

	return reversible1 > reversible2;
}

int CKeySequences::_ListComparer_Descending_Reversible( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool reversible1 = !self->_GetComposeSequenceFromListIndex( index1 ).Reversible;
	bool reversible2 = !self->_GetComposeSequenceFromListIndex( index2 ).Reversible;

	return reversible2 > reversible1;
}

//
// CKeySequences implementation
//

void CKeySequences::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );

	DDX_Control( pDX, IDC_KS_COMPOSE_SEQUENCE_LIST, m_List      );
	DDX_Control( pDX, IDADD,                        m_btnAdd    );
	DDX_Control( pDX, IDEDIT,                       m_btnEdit   );
	DDX_Control( pDX, IDREMOVE,                     m_btnRemove );
}

BOOL CKeySequences::OnInitDialog( ) {
	if ( !CPropertyPage::OnInitDialog( ) ) {
		return FALSE;
	}

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
	// TODO can we use ModifyStylesEx?
	m_List.SetExtendedStyle( m_List.GetExtendedStyle( ) | dwExtendedStyles );

	//
	// Configure our three columns
	//

	CString strLabels[NumberOfColumns] = {
		CString( ),
		LoadFromStringTable( IDS_KS_COLUMN_RESULT_CODEPOINT ),
		LoadFromStringTable( IDS_KS_COLUMN_RESULT_CHARACTER ),
		LoadFromStringTable( IDS_KS_COLUMN_SEQUENCE ),
		LoadFromStringTable( IDS_KS_COLUMN_CASEINSENSITIVE ),
		LoadFromStringTable( IDS_KS_COLUMN_REVERSIBLE ),
	};

	for ( int n = 0; n < NumberOfColumns; n++ ) {
		m_nColumnWidths[n] = m_List.GetStringWidth( strLabels[n] ) + HEADER_FUDGE_FACTOR;
		m_List.InsertColumn( n, strLabels[n], LVCFMT_LEFT, m_nColumnWidths[n] );
	}

	//
	// Configure groups
	//

	if ( g_CommonControlsApiVersion >= COMCTL32APIVER_WINXP ) {
		INT_PTR groupCount = m_Options.ComposeSequenceGroups.GetCount( );
		for ( int groupIndex = 0; groupIndex < groupCount; groupIndex++ ) {
			LVGROUP lv = {
				/* cbSize    */ sizeof( LVGROUP ),
				/* mask      */ LVGF_HEADER | LVGF_GROUPID | LVGF_STATE | LVGF_ALIGN,
				/* pszHeader */ const_cast<LPWSTR>( static_cast<LPCWSTR>( m_Options.ComposeSequenceGroups[groupIndex].Name ) ),
				/* cchHeader */ 0,
				/* pszFooter */ nullptr,
				/* cchFooter */ 0,
				/* iGroupId  */ groupIndex,
				/* stateMask */ LVGS_COLLAPSIBLE,
				/* state     */ LVGS_COLLAPSIBLE,
				/* uAlign    */ LVGA_HEADER_LEFT | LVGA_FOOTER_LEFT
			};
			int ret = m_List.InsertGroup( groupIndex, &lv );
			debug( L"CKeySequences::OnInitDialog: group #%d '%s' ret=%d\n", groupIndex, static_cast<LPCWSTR>( m_Options.ComposeSequenceGroups[groupIndex].Name ), ret );
		}

		m_List.EnableGroupView( groupCount > 1 );
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
	*pResult = 0;

	int nPrevSortColumn = m_nSortColumn;
	m_nSortColumn = pnmlv->iSubItem;
	if ( m_nSortColumn == nPrevSortColumn ) {
		m_SortState = static_cast<SORTSTATE>( ( m_SortState + 1 ) % 3 );
	} else {
		m_SortState = ssAscending;
	}
	debug( L"CKeySequences::OnListColumnClick: column %d sortstate %d self 0x%p\n", m_nSortColumn, m_SortState, this );

	_DoWithRedrawDisabled( [&]( ) {
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

		_RefreshListIndexMap( );

		//
		// Step 2: get the header control to display the appropriate indication
		//

		if ( -1 != nPrevSortColumn ) {
			_SetColumnSortState( nPrevSortColumn, ssUnsorted );
		}
		_SetColumnSortState( m_nSortColumn, m_SortState );
	} );
}

void CKeySequences::OnBnClickedAdd( ) {
	ComposeSequence sequence;
	CComposeSequenceEditor edit( sequence, semAdd, this );

	INT_PTR rc = edit.DoModal( );
	if ( IDOK != rc ) {
		return;
	}

	INT_PTR groupCount = m_Options.ComposeSequenceGroups.GetCount( );
	INT_PTR focusedGroup = -1;
	for ( int groupIndex = 0; groupIndex < groupCount; groupIndex++ ) {
		LVGROUP lv = { sizeof( LVGROUP ), LVGF_STATE };
		lv.stateMask = LVGS_FOCUSED;
		int ret = m_List.GetGroupInfo( groupIndex, &lv );
		if ( ret < 0 ) {
			debug( L"CKeySequences::OnBnClickedAdd: m_List.GetGroupInfo failed: ret=%d\n", ret );
			break;
		}
		if ( ( lv.state & LVGS_FOCUSED ) != 0 ) {
			focusedGroup = groupIndex;
			break;
		}
	}
	if ( -1 == focusedGroup ) {
		focusedGroup = 0;
	}

	_DoWithRedrawDisabled( [&]( ) {
		unsigned key = _MakeComposeSequenceGroupKey( focusedGroup, m_Options.ComposeSequenceGroups[focusedGroup].ComposeSequences.Add( sequence ) );
		_UpdateGroup( static_cast<int>( focusedGroup ) );
		m_List.EnableGroupView( m_Options.ComposeSequenceGroups.GetCount( ) != 1 );
		_AddComposeSequence( sequence, key );
		_SetColumnWidths( );
		SetModified( );
	} );
}

void CKeySequences::OnBnClickedEdit( ) {
	if ( m_List.GetSelectedCount( ) != 1 ) {
		return;
	}

	POSITION pos = m_List.GetFirstSelectedItemPosition( );
	if ( !pos ) {
		return;
	}

	int nItem = m_List.GetNextSelectedItem( pos );
	ComposeSequence& origSequence = _GetComposeSequenceFromListIndex( nItem );
	ComposeSequence sequence( origSequence );

	CComposeSequenceEditor edit( sequence, semEdit, this );
	INT_PTR rc = edit.DoModal( );
	if ( IDOK != rc ) {
		return;
	}

	origSequence = sequence;
	_DoWithRedrawDisabled( [&]( ) {
		_UpdateGroup( _GroupIndex( m_ListIndexMap[nItem] ) );
		_UpdateComposeSequence( nItem, sequence );
		_SetColumnWidths( );
		m_List.EnableGroupView( m_Options.ComposeSequenceGroups.GetCount( ) != 1 );
		SetModified( );
	} );
}

void CKeySequences::OnBnClickedRemove( ) {
	UINT count = m_List.GetSelectedCount( );
	if ( count < 1 ) {
		return;
	}

	int ret = MessageBox( LoadFromStringTable( IDS_KS_CONFIRM_DELETE_PROMPT ), LoadFromStringTable( IDS_KS_CONFIRM_DELETE_TITLE ), MB_YESNO | MB_ICONWARNING );
	if ( IDYES != ret ) {
		return;
	}

	POSITION pos = m_List.GetFirstSelectedItemPosition( );
	GroupAndSequenceIndex* items = new GroupAndSequenceIndex[count];
	unsigned index = 0;

	int nItemIndex = m_List.GetNextSelectedItem( pos );
	while ( ( -1 != nItemIndex ) && ( index < count ) ) {
		int mapped = m_ListIndexMap[nItemIndex];
		items[index] = GroupAndSequenceIndex( _GroupIndex( mapped ), _SequenceIndex( mapped ) );
		index++;
		nItemIndex = m_List.GetNextSelectedItem( pos );
	}

	count = index;
	qsort_s( items, count, sizeof( GroupAndSequenceIndex ), GroupAndSequenceIndex::CompareReverse, nullptr );

	for ( index = 0; index < count; index++ ) {
		m_Options.ComposeSequenceGroups[items[index].nGroup].ComposeSequences.RemoveAt( items[index].nSequence );
	};

	SetModified( );

	_FillList( );
	m_List.EnableGroupView( m_Options.ComposeSequenceGroups.GetCount( ) != 1 );

	delete[] items;
}
