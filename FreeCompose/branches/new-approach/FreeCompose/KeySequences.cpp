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
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_KEYCOMBOLIST, &CKeySequences::OnKeyComboListItemChanged)
	ON_NOTIFY(NM_DBLCLK, IDC_KEYCOMBOLIST, &CKeySequences::OnKeyComboListDoubleClick)
	ON_BN_CLICKED(IDADD, &CKeySequences::OnBnClickedAdd)
	ON_BN_CLICKED(IDEDIT, &CKeySequences::OnBnClickedEdit)
	ON_BN_CLICKED(IDREMOVE, &CKeySequences::OnBnClickedRemove)
END_MESSAGE_MAP( )

CKeySequences::CKeySequences( COptionsData& Options ):
	CPropertyPage ( CKeySequences::IDD ),
	m_Options     ( Options )
{
	memset( m_nColumnWidths, 0, sizeof( m_nColumnWidths ) );
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
void CKeySequences::_DoAddOneKeySequence( const INT_PTR n ) {
	const COMPOSE_SEQUENCE& sequence = m_Options.m_ComposeSequences[ n ];
	CString col0, col1, col2;
	int item;
	int width;
	BOOL ret;

	col0.Format( L"U+%06X %s", sequence.chComposed, static_cast<wchar_t const*>( Utf32ToUtf16( sequence.chComposed ) ) );
	width = m_KeyComboList.GetStringWidth( col0 ) + ITEM_FUDGE_FACTOR;
	if ( width > m_nColumnWidths[0] )
		m_nColumnWidths[0] = width;
	item = m_KeyComboList.InsertItem( LVIF_PARAM | LVIF_STATE | LVIF_TEXT, (int) n, col0, 0, (UINT) -1, -1, n );
	ASSERT( n == item );

	col1 = Utf32ToUtf16( sequence.chFirst );
	width = m_KeyComboList.GetStringWidth( col1 ) + ITEM_FUDGE_FACTOR;
	if ( width > m_nColumnWidths[1] )
		m_nColumnWidths[1] = width;
	ret = m_KeyComboList.SetItem( item, 1, LVIF_STATE | LVIF_TEXT, col1, -1, 0, (UINT) -1, 0 );
	ASSERT( TRUE == ret );

	col2 = Utf32ToUtf16( sequence.chSecond );
	width = m_KeyComboList.GetStringWidth( col2 ) + ITEM_FUDGE_FACTOR;
	if ( width > m_nColumnWidths[1] )
		m_nColumnWidths[1] = width;
	ret = m_KeyComboList.SetItem( item, 2, LVIF_STATE | LVIF_TEXT, col2, -1, 0, (UINT) -1, 0 );
	ASSERT( TRUE == ret );
}

// TODO error handling
void CKeySequences::_DoUpdateOneKeySequence( const INT_PTR n ) {
	const COMPOSE_SEQUENCE& sequence = m_Options.m_ComposeSequences[ n ];
	CString col0, col1, col2;
	int width;

	col0.Format( L"U+%06X %s", sequence.chComposed, static_cast<wchar_t const*>( Utf32ToUtf16( sequence.chComposed ) ) );
	width = m_KeyComboList.GetStringWidth( col0 ) + ITEM_FUDGE_FACTOR;
	if ( width > m_nColumnWidths[0] )
		m_nColumnWidths[0] = width;
	m_KeyComboList.SetItem( (int) n, 0, LVIF_TEXT, col0, -1, 0, 0, 0 );

	col1 = Utf32ToUtf16( sequence.chFirst );
	width = m_KeyComboList.GetStringWidth( col1 ) + ITEM_FUDGE_FACTOR;
	if ( width > m_nColumnWidths[1] )
		m_nColumnWidths[1] = width;
	m_KeyComboList.SetItem( (int) n, 1, LVIF_TEXT, col1, -1, 0, 0, 0 );

	col2 = Utf32ToUtf16( sequence.chSecond );
	width = m_KeyComboList.GetStringWidth( col2 ) + ITEM_FUDGE_FACTOR;
	if ( width > m_nColumnWidths[1] )
		m_nColumnWidths[1] = width;
	m_KeyComboList.SetItem( (int) n, 2, LVIF_TEXT, col2, -1, 0, 0, 0 );
}

void CKeySequences::_AdjustColumns( void ) {
	m_KeyComboList.SetColumnWidth( 0, m_nColumnWidths[0] );
	m_KeyComboList.SetColumnWidth( 1, m_nColumnWidths[1] );
	m_KeyComboList.SetColumnWidth( 2, m_nColumnWidths[1] );
}

void CKeySequences::_FillKeyComboList( void ) {
	m_KeyComboList.DeleteAllItems( );
	for ( int n = 0; n < m_Options.m_ComposeSequences.GetCount( ); n++ ) {
		_DoAddOneKeySequence( n );
	}
	_AdjustColumns( );
}

void CKeySequences::_AddNewKeySequence( const INT_PTR n ) {
	_DoAddOneKeySequence( n );
	_AdjustColumns( );
}

BOOL CKeySequences::OnInitDialog( ) {
	if ( ! CPropertyPage::OnInitDialog( ) )
		return FALSE;

	DWORD dwExtendedStyles =
		  LVS_EX_FLATSB
		| LVS_EX_FULLROWSELECT
		| LVS_EX_GRIDLINES
		| LVS_EX_HEADERDRAGDROP
		| LVS_EX_LABELTIP
		| LVS_EX_TWOCLICKACTIVATE;
	if ( GetComCtl32Version( ) >= 0x00060000 ) {
		dwExtendedStyles |= LVS_EX_DOUBLEBUFFER;
	}
	m_KeyComboList.SetExtendedStyle( m_KeyComboList.GetExtendedStyle( ) | dwExtendedStyles );

	CString strLabels[3] = {
		CString( (LPCWSTR) IDS_KEYSEQUENCES_CHARACTER ),
		CString( (LPCWSTR) IDS_KEYSEQUENCES_KEY1 ),
		CString( (LPCWSTR) IDS_KEYSEQUENCES_KEY2 ),
	};

	int widths[3];
	for ( int n = 0; n < 3; n++ ) {
		widths[n] = m_KeyComboList.GetStringWidth( strLabels[n] ) + HEADER_FUDGE_FACTOR;
		m_KeyComboList.InsertColumn( n, strLabels[n], LVCFMT_LEFT, widths[n] );
	}
	m_nColumnWidths[0] = widths[0];
	m_nColumnWidths[1] = std::max( widths[0], widths[1] );

	_FillKeyComboList( );
	
	return TRUE;
}

void CKeySequences::OnKeyComboListItemChanged( NMHDR* /*pNMHDR*/, LRESULT* pResult ) {
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );

	UINT nsel = m_KeyComboList.GetSelectedCount( );
	if ( 0 == nsel ) {
		m_btnEdit.EnableWindow( FALSE );
		m_btnRemove.EnableWindow( FALSE );
	} else {
		m_btnEdit.EnableWindow( ( 1 == nsel ) );
		m_btnRemove.EnableWindow( TRUE );
	}

	*pResult = 0;
}

void CKeySequences::OnKeyComboListDoubleClick( NMHDR* /*pNMHDR*/, LRESULT* pResult ) {
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );

	OnBnClickedEdit( );

	*pResult = 0;
}

void CKeySequences::OnBnClickedAdd( ) {
	COMPOSE_SEQUENCE sequence = { 0, 0, 0, };
	CComposeSequenceEditor edit( sequence, semAdd, this );
	INT_PTR rc = edit.DoModal( );
	if ( IDOK == rc ) {
		_AddNewKeySequence( m_Options.m_ComposeSequences.Add( sequence ) );
		SetModified( );
	}
}

void CKeySequences::OnBnClickedEdit( ) {
	if ( m_KeyComboList.GetSelectedCount( ) < 1 )
		return;

	POSITION pos = m_KeyComboList.GetFirstSelectedItemPosition( );
	if ( NULL == pos )
		return;

	int k = m_KeyComboList.GetNextSelectedItem( pos );
	COMPOSE_SEQUENCE sequence = m_Options.m_ComposeSequences[ k ];
	CComposeSequenceEditor edit( sequence, semEdit, this );
	INT_PTR rc = edit.DoModal( );
	if ( IDOK == rc ) {
		m_Options.m_ComposeSequences[ k ] = sequence;
		_DoUpdateOneKeySequence( k );
		_AdjustColumns( );
		SetModified( );
	}
}

void CKeySequences::OnBnClickedRemove( ) {
	UINT count = m_KeyComboList.GetSelectedCount( );
	if ( count < 1 )
		return;

	int ret = MessageBox( CString( (LPCWSTR) IDS_KEYSEQUENCES_CONFIRM_PROMPT ), CString( (LPCWSTR) IDS_KEYSEQUENCES_CONFIRM_TITLE ), MB_YESNO | MB_ICONWARNING );
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
	qsort_s( items, count, sizeof( int ), compare_keys_reverse, NULL );

	for ( n = 0; n < count; n++ ) {
		m_KeyComboList.DeleteItem( items[n] );
		m_Options.m_ComposeSequences.RemoveAt( items[n] );
	}

	delete[] items;

	SetModified( );
}
