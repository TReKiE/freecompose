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
	ON_NOTIFY( LVN_ITEMCHANGED, IDC_KEYCOMBOLIST, &CKeySequences::OnKeyComboListItemChanged )
	ON_NOTIFY( NM_DBLCLK,       IDC_KEYCOMBOLIST, &CKeySequences::OnKeyComboListDoubleClick )

	ON_BN_CLICKED( IDADD,    &CKeySequences::OnBnClickedAdd    )
	ON_BN_CLICKED( IDEDIT,   &CKeySequences::OnBnClickedEdit   )
	ON_BN_CLICKED( IDREMOVE, &CKeySequences::OnBnClickedRemove )
END_MESSAGE_MAP( )

CKeySequences::CKeySequences( COptionsData& Options ):
	CPropertyPage ( IDD ),
	m_Options     ( Options )
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

void CKeySequences::_FillKeyComboList( void ) {
	m_KeyComboList.DeleteAllItems( );
	INT_PTR limit = m_Options.ComposeSequences.GetCount( );
	for ( INT_PTR n = 0; n < limit; n++ ) {
		_AddOneKeySequence( n );
	}
	_AdjustColumns( );
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
