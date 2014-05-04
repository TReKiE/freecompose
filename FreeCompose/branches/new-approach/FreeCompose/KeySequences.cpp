// KeySequences.cpp : implementation file

#include "stdafx.h"

#include <numeric>

#include <Unicode.h>

#include "FreeCompose.h"
#include "KeySequences.h"
#include "ComposeSequenceEditor.h"

#include "Utils.h"

const int ITEM_FUDGE_FACTOR   = 12;
const int HEADER_FUDGE_FACTOR = ITEM_FUDGE_FACTOR + 3;

IMPLEMENT_DYNAMIC( CKeySequences, CPropertyPage )

BEGIN_MESSAGE_MAP( CKeySequences, CPropertyPage )
	ON_NOTIFY( NM_DBLCLK,       IDC_KEYCOMBOLIST, &CKeySequences::OnListDoubleClick )
	ON_NOTIFY( LVN_COLUMNCLICK, IDC_KEYCOMBOLIST, &CKeySequences::OnListColumnClick )
	ON_NOTIFY( LVN_ITEMCHANGED, IDC_KEYCOMBOLIST, &CKeySequences::OnListItemChanged )

	ON_BN_CLICKED( IDADD,    &CKeySequences::OnBnClickedAdd    )
	ON_BN_CLICKED( IDEDIT,   &CKeySequences::OnBnClickedEdit   )
	ON_BN_CLICKED( IDREMOVE, &CKeySequences::OnBnClickedRemove )
END_MESSAGE_MAP( )

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

int ColumnHeaderFormatFlagsMap[] = {
	0,
	HDF_SORTUP,
	HDF_SORTDOWN
};

CKeySequences::CKeySequences( COptionsData& Options ):
	CPropertyPage ( IDD ),
	m_Options     ( Options ),
	m_nSortColumn ( -1 ),
	m_SortState   ( ssUnsorted )
{

}

CKeySequences::~CKeySequences( ) {

}

void CKeySequences::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_KEYCOMBOLIST, m_List      );
	DDX_Control( pDX, IDADD,            m_btnAdd    );
	DDX_Control( pDX, IDEDIT,           m_btnEdit   );
	DDX_Control( pDX, IDREMOVE,         m_btnRemove );
}

// TODO error handling
inline CString CKeySequences::_FormatResultString( ComposeSequence const& sequence ) {
	CString strResult;
	unsigned chResult;
	Utf16ToUtf32( sequence.Result, chResult );
	strResult.Format( L"U+%06X %s", chResult, sequence.Result );
	return strResult;
}

// TODO error handling
void CKeySequences::_AddOneKeySequence( const INT_PTR n ) {
	ComposeSequence const& sequence = m_Options.ComposeSequences[n];
	CString strResult( _FormatResultString( sequence ) );

	m_nColumnWidths[0] = std::max( m_List.GetStringWidth( strResult ) + ITEM_FUDGE_FACTOR, m_nColumnWidths[0] );
	int item = m_List.InsertItem( LVIF_TEXT | LVIF_PARAM, static_cast<int>( n ), strResult, 0, 0, 0, n );

	m_nColumnWidths[1] = std::max( m_List.GetStringWidth( sequence.Result ) + ITEM_FUDGE_FACTOR, m_nColumnWidths[1] );
	m_List.SetItem( item, 1, LVIF_TEXT, sequence.Result, 0, 0, 0, 0 );

	m_nColumnWidths[2] = std::max( m_List.GetStringWidth( sequence.Sequence ) + ITEM_FUDGE_FACTOR, m_nColumnWidths[2] );
	m_List.SetItem( item, 2, LVIF_TEXT, sequence.Sequence, 0, 0, 0, 0 );
}

// TODO error handling
void CKeySequences::_UpdateOneKeySequence( const INT_PTR n ) {
	const ComposeSequence& sequence = m_Options.ComposeSequences[n];
	CString strResult( _FormatResultString( sequence ) );

	m_nColumnWidths[0] = std::max( m_List.GetStringWidth( strResult ) + ITEM_FUDGE_FACTOR, m_nColumnWidths[0] );
	m_List.SetItem( static_cast<int>( n ), 0, LVIF_TEXT, strResult, 0, 0, 0, 0 );

	m_nColumnWidths[1] = std::max( m_List.GetStringWidth( sequence.Result ) + ITEM_FUDGE_FACTOR, m_nColumnWidths[1] );
	m_List.SetItem( static_cast<int>( n ), 1, LVIF_TEXT, sequence.Result, 0, 0, 0, 0 );

	m_nColumnWidths[2] = std::max( m_List.GetStringWidth( sequence.Sequence ) + ITEM_FUDGE_FACTOR, m_nColumnWidths[2] );
	m_List.SetItem( static_cast<int>( n ), 2, LVIF_TEXT, sequence.Sequence, 0, 0, 0, 0 );
}

void CKeySequences::_AdjustColumns( void ) {
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
// Huh. Subtraction is so much easier, for certain kinds of comparisons.
//

int CKeySequences::_ListComparer_Unsorted( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	index1 = self->m_pnSortIndices[ index1 ];
	index2 = self->m_pnSortIndices[ index2 ];
	return static_cast<int>( index1 - index2 );
}

int CKeySequences::_ListComparer_Ascending_Result( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	index1 = self->m_pnSortIndices[ index1 ];
	index2 = self->m_pnSortIndices[ index2 ];
	CArray<ComposeSequence>& composeSequences = self->m_Options.ComposeSequences;
	CString& result1 = composeSequences[index1].Result;
	CString& result2 = composeSequences[index2].Result;
	return static_cast<int>( result1.Compare( result2 ) );
}

int CKeySequences::_ListComparer_Descending_Result( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	index1 = self->m_pnSortIndices[ index1 ];
	index2 = self->m_pnSortIndices[ index2 ];
	CArray<ComposeSequence>& composeSequences = self->m_Options.ComposeSequences;
	CString& result1 = composeSequences[index1].Result;
	CString& result2 = composeSequences[index2].Result;
	return static_cast<int>( result2.Compare( result1 ) );
}

int CKeySequences::_ListComparer_Ascending_Sequence( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	index1 = self->m_pnSortIndices[ index1 ];
	index2 = self->m_pnSortIndices[ index2 ];
	CArray<ComposeSequence>& composeSequences = self->m_Options.ComposeSequences;
	CString& sequence1 = composeSequences[index1].Sequence;
	CString& sequence2 = composeSequences[index2].Sequence;
	return static_cast<int>( sequence1.Compare( sequence2 ) );
}

int CKeySequences::_ListComparer_Descending_Sequence( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	index1 = self->m_pnSortIndices[ index1 ];
	index2 = self->m_pnSortIndices[ index2 ];
	CArray<ComposeSequence>& composeSequences = self->m_Options.ComposeSequences;
	CString& sequence1 = composeSequences[index1].Sequence;
	CString& sequence2 = composeSequences[index2].Sequence;
	return static_cast<int>( sequence2.Compare( sequence1 ) );
}

void CKeySequences::_FillKeyComboList( void ) {
	SetRedraw( FALSE );

	m_List.DeleteAllItems( );
	INT_PTR limit = m_Options.ComposeSequences.GetCount( );
	for ( INT_PTR n = 0; n < limit; n++ ) {
		_AddOneKeySequence( n );
	}
	_AdjustColumns( );

	SetRedraw( TRUE );
	Invalidate( );
	UpdateWindow( );
}

void CKeySequences::_AddNewKeySequence( const INT_PTR n ) {
	SetRedraw( FALSE );

	_AddOneKeySequence( n );
	_AdjustColumns( );

	SetRedraw( TRUE );
	Invalidate( );
	UpdateWindow( );
}

BOOL CKeySequences::OnInitDialog( ) {
	if ( !CPropertyPage::OnInitDialog( ) ) {
		return FALSE;
	}

	m_pListHeader = m_List.GetHeaderCtrl( );

	DWORD dwExtendedStyles = LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP | LVS_EX_TWOCLICKACTIVATE;
	if ( g_dwCommonControlsVersion >= COMCTL32VER_WINXP ) {
		dwExtendedStyles |= LVS_EX_DOUBLEBUFFER;
	}
	if ( g_dwCommonControlsVersion >= COMCTL32VER_VISTA ) {
		dwExtendedStyles |= LVS_EX_COLUMNSNAPPOINTS;
	}
	m_List.SetExtendedStyle( m_List.GetExtendedStyle( ) | dwExtendedStyles );

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

	_FillKeyComboList( );
	
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

	PFNLVCOMPARE pfnCompare = nullptr;
	switch ( m_nSortColumn ) {
		case ResultCodePointColumn: pfnCompare = ResultColumnsSortFuncMap[m_SortState];  break;
		case ResultCharacterColumn: pfnCompare = ResultColumnsSortFuncMap[m_SortState];  break;
		case SequenceColumn:        pfnCompare = SequenceColumnSortFuncMap[m_SortState]; break;
	}
	if ( !pfnCompare ) {
		debug( L"CKeySequences::OnListColumnClick: !!! m_nSortColumn is invalid !!!\n" );
		return;
	}

	int count = m_List.GetItemCount( );
	m_pnSortIndices = new int[count];
	for ( int n = 0; n < count; n++ ) {
		m_pnSortIndices[n] = static_cast<int>( m_List.GetItemData( n ) );
	}
	if ( !m_List.SortItemsEx( pfnCompare, reinterpret_cast<DWORD_PTR>( this ) ) ) {
		debug( L"CKeySequences::OnListColumnClick: SortItems failed?\n" );
	}
	delete[] m_pnSortIndices;

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
		_AddNewKeySequence( m_Options.ComposeSequences.Add( sequence ) );
		SetModified( );
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
	int k = static_cast<int>( m_List.GetItemData( nItem ) );
	ComposeSequence sequence = m_Options.ComposeSequences[k];
	CComposeSequenceEditor edit( sequence, semEdit, this );
	INT_PTR rc = edit.DoModal( );
	if ( IDOK == rc ) {
		m_Options.ComposeSequences[k] = sequence;

		SetRedraw( FALSE );

		_UpdateOneKeySequence( k );
		_AdjustColumns( );
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
	int* indices = new int[count];
	unsigned n = 0;

	int i = m_List.GetNextSelectedItem( pos );
	while ( ( -1 != i ) && ( n < count ) ) {
		items[n] = i;
		indices[n] = static_cast<int>( m_List.GetItemData( i ) );
		n++;
		i = m_List.GetNextSelectedItem( pos );
	}
	count = n;
	qsort_s( items,   count, sizeof( int ), compare_keys_reverse, nullptr );
	qsort_s( indices, count, sizeof( int ), compare_keys_reverse, nullptr );

	// TODO TEST
	n = count;
	do {
		n--;
		m_List.DeleteItem( items[n] );
		m_Options.ComposeSequences.RemoveAt( indices[n] );
	} while ( n > 0 );

	delete[] items;
	delete[] indices;

	SetModified( );
}
