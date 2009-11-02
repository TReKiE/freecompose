// KeySequences.cpp : implementation file

#include "stdafx.h"

#include "FreeCompose.h"
#include "KeySequences.h"
#include "EditKeySequence.h"

#include "Utils.h"

const int ITEM_FUDGE_FACTOR   = 12;
const int HEADER_FUDGE_FACTOR = ITEM_FUDGE_FACTOR + 3;

IMPLEMENT_DYNAMIC( CKeySequences, CPropertyPage )

BEGIN_MESSAGE_MAP( CKeySequences, CPropertyPage )
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_KEYCOMBOLIST, &CKeySequences::OnKeyComboListItemChanged)
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

void CKeySequences::_DoAddOneKeySequence( const int n, const COMPOSE_KEY_ENTRY& cke ) {
	CString col0, col1, col2;
	int width;
	BOOL ret;

	col0 = cke.wchComposed;
	width = m_KeyComboList.GetStringWidth( col0 ) + ITEM_FUDGE_FACTOR;
	if ( width > m_nColumnWidths[0] )
		m_nColumnWidths[0] = width;
	int nItem = m_KeyComboList.InsertItem( LVIF_PARAM | LVIF_STATE | LVIF_TEXT, n, col0, 0, (UINT) -1, -1, n );
	ASSERT( -1 != nItem );

	col1 = VkToString( cke.vkFirst );
	width = m_KeyComboList.GetStringWidth( col1 ) + ITEM_FUDGE_FACTOR;
	if ( width > m_nColumnWidths[1] )
		m_nColumnWidths[1] = width;
	ret = m_KeyComboList.SetItem( nItem, 1, LVIF_STATE | LVIF_TEXT, col1, -1, 0, (UINT) -1, 0 );
	ASSERT( TRUE == ret );

	col2 = VkToString( cke.vkSecond );
	width = m_KeyComboList.GetStringWidth( col2 ) + ITEM_FUDGE_FACTOR;
	if ( width > m_nColumnWidths[2] )
		m_nColumnWidths[2] = width;
	ret = m_KeyComboList.SetItem( nItem, 2, LVIF_STATE | LVIF_TEXT, col2, -1, 0, (UINT) -1, 0 );
	ASSERT( TRUE == ret );
}

void CKeySequences::_AdjustColumns( void ) {
	int m = max( m_nColumnWidths[1], m_nColumnWidths[2] );
	m_nColumnWidths[1] = m_nColumnWidths[2] = m;
	m_KeyComboList.SetColumnWidth( 0, m_nColumnWidths[0] );
	m_KeyComboList.SetColumnWidth( 1, m_nColumnWidths[1] );
	m_KeyComboList.SetColumnWidth( 2, m_nColumnWidths[2] );
}

void CKeySequences::_FillKeyComboList( void ) {
	m_Options.m_ComposeKeyEntries.RemoveAll( );
	m_Options.m_ComposeKeyEntries.Copy( ComposeKeyEntries );
	m_KeyComboList.DeleteAllItems( );

	for ( int n = 0; n < m_Options.m_ComposeKeyEntries.GetCount( ); n++ ) {
		_DoAddOneKeySequence( n, m_Options.m_ComposeKeyEntries[ n ] );
	}
	_AdjustColumns( );
}

void CKeySequences::_AddNewKeySequence( const COMPOSE_KEY_ENTRY& cke ) {
	_DoAddOneKeySequence( m_Options.m_ComposeKeyEntries.GetCount( ), cke );
	_AdjustColumns( );
}

BOOL CKeySequences::OnInitDialog( ) {
	if ( ! CPropertyPage::OnInitDialog( ) )
		return FALSE;

	m_KeyComboList.ModifyStyle( LVS_SINGLESEL, 0 );
	m_KeyComboList.SetExtendedStyle( m_KeyComboList.GetExtendedStyle( ) | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	CString strLabels[3] = { CString( _T("Character") ), CString( _T("Key 1") ), CString( _T("Key 2") ), };

	for ( int n = 0; n < 3; n++ ) {
		m_nColumnWidths[n] = m_KeyComboList.GetStringWidth( strLabels[n] ) + HEADER_FUDGE_FACTOR;
		m_KeyComboList.InsertColumn( n, strLabels[n], LVCFMT_LEFT, m_nColumnWidths[n] );
	}

	_FillKeyComboList( );
	
	return TRUE;
}

void CKeySequences::OnKeyComboListItemChanged( NMHDR* pNMHDR, LRESULT* pResult ) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );

	if ( NM_CLICK == pNMLV->hdr.code || NM_DBLCLK == pNMLV->hdr.code ) {
		UINT nsel = m_KeyComboList.GetSelectedCount( );
		if ( 0 == nsel ) {
			m_btnEdit.EnableWindow( FALSE );
			m_btnRemove.EnableWindow( FALSE );
		} else {
			m_btnEdit.EnableWindow( ( 1 == nsel ) );
			m_btnRemove.EnableWindow( TRUE );
		}
	}

	*pResult = 0;
}

void CKeySequences::OnBnClickedAdd( ) {
	COMPOSE_KEY_ENTRY newcke;

	CEditKeySequence edit( newcke, true, this );
	INT_PTR rc = edit.DoModal( );
	if ( IDOK == rc ) {
		m_Options.m_ComposeKeyEntries.Add( newcke );
		_AddNewKeySequence( newcke );
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
	COMPOSE_KEY_ENTRY cke( m_Options.m_ComposeKeyEntries[ k ] );
	CEditKeySequence edit( cke, false, this );
	INT_PTR rc = edit.DoModal( );
	if ( IDOK == rc ) {
		m_Options.m_ComposeKeyEntries[ k ] = cke;
		m_KeyComboList.Update( k );
		SetModified( );
	}
}

void CKeySequences::OnBnClickedRemove( ) {
	UINT count = m_KeyComboList.GetSelectedCount( );
	if ( count < 1 )
		return;

	int ret = MessageBox( _T("Are you sure?"), _T("Remove key definitions"), MB_YESNO | MB_ICONWARNING );
	if ( IDYES != ret ) {
		return;
	}

	POSITION pos = m_KeyComboList.GetFirstSelectedItemPosition( );
	int i = m_KeyComboList.GetNextSelectedItem( pos );
	int* items = new int[count];
	int n = 0;

	while ( -1 != i ) {
		items[n++] = i;
		i = m_KeyComboList.GetNextSelectedItem( pos );
	}
	qsort_s( items, count, sizeof( int ), compare_keys_reverse, NULL );

	for ( UINT n = 0; n < count; n++ ) {
		m_KeyComboList.DeleteItem( items[n] );
		m_Options.m_ComposeKeyEntries.RemoveAt( items[n] );
	}

	delete[] items;

	SetModified( );
}
