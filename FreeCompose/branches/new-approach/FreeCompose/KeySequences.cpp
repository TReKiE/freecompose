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
	ON_NOTIFY( LVN_COLUMNCLICK, IDC_KEYCOMBOLIST, &CKeySequences::OnKeyComboListColumnClick )
	ON_NOTIFY( NM_DBLCLK,       IDC_KEYCOMBOLIST, &CKeySequences::OnKeyComboListDoubleClick )
	ON_NOTIFY( LVN_ITEMCHANGED, IDC_KEYCOMBOLIST, &CKeySequences::OnKeyComboListItemChanged )

	ON_BN_CLICKED( IDADD,    &CKeySequences::OnBnClickedAdd    )
	ON_BN_CLICKED( IDEDIT,   &CKeySequences::OnBnClickedEdit   )
	ON_BN_CLICKED( IDREMOVE, &CKeySequences::OnBnClickedRemove )
END_MESSAGE_MAP( )

CKeySequences::sortcallbackfunc* CKeySequences::ResultColumnSortFuncMap[] = {
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
	m_nColumnWidths[0] = m_nColumnWidths[1] = 0;
}

CKeySequences::~CKeySequences( ) {

}

void CKeySequences::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_KEYCOMBOLIST, m_KeyComboList );
	DDX_Control( pDX, IDADD,            m_btnAdd       );
	DDX_Control( pDX, IDEDIT,           m_btnEdit      );
	DDX_Control( pDX, IDREMOVE,         m_btnRemove    );
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

	m_nColumnWidths[0] = std::max( m_KeyComboList.GetStringWidth( strResult ) + ITEM_FUDGE_FACTOR, m_nColumnWidths[0] );
	int item = m_KeyComboList.InsertItem( LVIF_PARAM | LVIF_STATE | LVIF_TEXT, static_cast<int>( n ), strResult, 0, static_cast<UINT>( -1 ), -1, n );

	m_nColumnWidths[1] = std::max( m_KeyComboList.GetStringWidth( sequence.Sequence ) + ITEM_FUDGE_FACTOR, m_nColumnWidths[1] );
	m_KeyComboList.SetItem( item, 1, LVIF_STATE | LVIF_TEXT, sequence.Sequence, -1, 0, static_cast<UINT>( -1 ), 0 );
}

// TODO error handling
void CKeySequences::_UpdateOneKeySequence( const INT_PTR n ) {
	const ComposeSequence& sequence = m_Options.ComposeSequences[n];
	CString strResult( _FormatResultString( sequence ) );

	m_nColumnWidths[0] = std::max( m_KeyComboList.GetStringWidth( strResult ) + ITEM_FUDGE_FACTOR, m_nColumnWidths[0] );
	m_KeyComboList.SetItem( static_cast<int>( n ), 0, LVIF_TEXT, strResult, -1, 0, 0, 0 );

	m_nColumnWidths[1] = std::max( m_KeyComboList.GetStringWidth( sequence.Sequence ) + ITEM_FUDGE_FACTOR, m_nColumnWidths[1] );
	m_KeyComboList.SetItem( static_cast<int>( n ), 1, LVIF_TEXT, sequence.Sequence, -1, 0, 0, 0 );
}

void CKeySequences::_AdjustColumns( void ) {
	m_KeyComboList.SetColumnWidth( 0, m_nColumnWidths[0] );
	m_KeyComboList.SetColumnWidth( 1, m_nColumnWidths[1] );
}

//
// Huh. Subtraction is so much easier, for certain kinds of comparisons.
//

int CKeySequences::_ListComparer_Unsorted( LPARAM index1, LPARAM index2, LPARAM ) {
	return static_cast<int>( index1 - index2 );
}

int CKeySequences::_ListComparer_Ascending_Result( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	return static_cast<int>( self->m_Options.ComposeSequences[index1].Result - self->m_Options.ComposeSequences[index2].Result );
}

int CKeySequences::_ListComparer_Descending_Result( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	return static_cast<int>( self->m_Options.ComposeSequences[index2].Result - self->m_Options.ComposeSequences[index1].Result );
}

int CKeySequences::_ListComparer_Ascending_Sequence( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	return static_cast<int>( self->m_Options.ComposeSequences[index1].Sequence.Compare( self->m_Options.ComposeSequences[index2].Result ) );
}

int CKeySequences::_ListComparer_Descending_Sequence( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	return static_cast<int>( self->m_Options.ComposeSequences[index2].Sequence.Compare( self->m_Options.ComposeSequences[index1].Result ) );
}

void CKeySequences::_FillKeyComboList( void ) {
	SetRedraw( FALSE );

	m_KeyComboList.DeleteAllItems( );
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
	_AddOneKeySequence( n );
	_AdjustColumns( );
}

BOOL CKeySequences::OnInitDialog( ) {
	if ( !CPropertyPage::OnInitDialog( ) ) {
		return FALSE;
	}

	DWORD dwExtendedStyles = LVS_EX_FLATSB | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP | LVS_EX_TWOCLICKACTIVATE;
	if ( GetComCtl32Version( ) >= 0x00060000 ) {
		dwExtendedStyles |= LVS_EX_DOUBLEBUFFER;
	}
	m_KeyComboList.SetExtendedStyle( m_KeyComboList.GetExtendedStyle( ) | dwExtendedStyles );

	CString strLabels[] = {
		LoadFromStringTable( IDS_KEYSEQUENCES_RESULT ),
		LoadFromStringTable( IDS_KEYSEQUENCES_SEQUENCE ),
	};

	int widths[2];
	for ( int n = 0; n < 2; n++ ) {
		widths[n] = m_KeyComboList.GetStringWidth( strLabels[n] ) + HEADER_FUDGE_FACTOR;
		m_KeyComboList.InsertColumn( n, strLabels[n], LVCFMT_LEFT, widths[n] );
	}
	m_nColumnWidths[0] = widths[0];
	m_nColumnWidths[1] = widths[1];

	_FillKeyComboList( );
	
	return TRUE;
}

void CKeySequences::OnKeyComboListItemChanged( NMHDR* /*pNMHDR*/, LRESULT* pResult ) {
	UINT nsel = m_KeyComboList.GetSelectedCount( );
	if ( nsel ) {
		m_btnEdit.EnableWindow( ( 1 == nsel ) );
		m_btnRemove.EnableWindow( TRUE );
	} else {
		m_btnEdit.EnableWindow( FALSE );
		m_btnRemove.EnableWindow( FALSE );
	}

	*pResult = 0;
}

void CKeySequences::OnKeyComboListDoubleClick( NMHDR* /*pNMHDR*/, LRESULT* pResult ) {
	OnBnClickedEdit( );

	*pResult = 0;
}

void CKeySequences::OnKeyComboListColumnClick( NMHDR* pNMHDR, LRESULT* pResult ) {
	NMLISTVIEW* pnmlv = reinterpret_cast<NMLISTVIEW*>( pNMHDR );

	m_nSortColumn = pnmlv->iSubItem;
	m_SortState = static_cast<SORTSTATE>( ( m_SortState + 1 ) % 3 );
	debug( L"CKeySequences::OnKeyComboListColumnClick: column %d sortstate %d self 0x%p\n", m_nSortColumn, m_SortState, this );

	//
	// Step 1: tell combo box to sort itself
	//

	PFNLVCOMPARE pfnCompare = nullptr;
	switch ( m_nSortColumn ) {
		case ResultColumn: pfnCompare = ResultColumnSortFuncMap[m_SortState];
		case SequenceColumn: pfnCompare = SequenceColumnSortFuncMap[m_SortState];
	}
	if ( !pfnCompare ) {
		debug( L"CKeySequences::OnKeyComboListColumnClick: !!! m_SortState is invalid !!!\n" );
		return;
	}

	if ( !m_KeyComboList.SortItemsEx( pfnCompare, reinterpret_cast<DWORD_PTR>( this ) ) ) {
		debug( L"CKeySequences::OnKeyComboListColumnClick: SortItems failed?\n" );
	}

	//
	// Step 2: get the header control to display the appropriate indication
	//

	HDITEM hdItem = { HDI_FORMAT, };
	CHeaderCtrl* phdr = m_KeyComboList.GetHeaderCtrl( );
	if ( !phdr->GetItem( m_nSortColumn, &hdItem ) ) {
		debug( L"CKeySequences::OnKeyComboListColumnClick: GetItem failed?\n" );
		return;
	}
	hdItem.fmt = ( hdItem.fmt & ~( HDF_SORTUP | HDF_SORTDOWN ) ) | ColumnHeaderFormatFlagsMap[m_SortState];
	if ( !phdr->SetItem( m_nSortColumn, &hdItem ) ) {
		debug( L"CKeySequences::OnKeyComboListColumnClick: SetItem failed?\n" );
		return;
	}

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
	if ( m_KeyComboList.GetSelectedCount( ) < 1 ) {
		return;
	}

	POSITION pos = m_KeyComboList.GetFirstSelectedItemPosition( );
	if ( !pos ) {
		return;
	}

	int k = m_KeyComboList.GetNextSelectedItem( pos );
	ComposeSequence sequence = m_Options.ComposeSequences[k];
	CComposeSequenceEditor edit( sequence, semEdit, this );
	INT_PTR rc = edit.DoModal( );
	if ( IDOK == rc ) {
		m_Options.ComposeSequences[k] = sequence;
		_UpdateOneKeySequence( k );
		_AdjustColumns( );
		SetModified( );
	}
}

void CKeySequences::OnBnClickedRemove( ) {
	UINT count = m_KeyComboList.GetSelectedCount( );
	if ( count < 1 ) {
		return;
	}

	int ret = MessageBox( LoadFromStringTable( IDS_KEYSEQUENCES_CONFIRM_PROMPT ), LoadFromStringTable( IDS_KEYSEQUENCES_CONFIRM_TITLE ), MB_YESNO | MB_ICONWARNING );
	if ( IDYES != ret ) {
		return;
	}

	POSITION pos = m_KeyComboList.GetFirstSelectedItemPosition( );
	int* items = new int[count];
	unsigned n = 0;

	int i = m_KeyComboList.GetNextSelectedItem( pos );
	while ( ( -1 != i ) && ( n < count ) ) {
		items[n++] = i;
		i = m_KeyComboList.GetNextSelectedItem( pos );
	}
	count = n;
	qsort_s( items, count, sizeof( int ), compare_keys_reverse, nullptr );

	// TODO TEST
	n = count;
	do {
		n--;
		m_KeyComboList.DeleteItem( items[n] );
		m_Options.ComposeSequences.RemoveAt( items[n] );
	} while ( n > 0 );

	delete[] items;

	SetModified( );
}
