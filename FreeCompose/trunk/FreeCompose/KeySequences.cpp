// KeySequences.cpp : implementation file

#include "stdafx.h"

#include "FreeCompose.h"
#include "KeySequences.h"

#include "Utils.h"

static const LPCSTR lpszVkNames[256] = {
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    "VK_BACK",                          "VK_TAB",                           NULL,                               NULL,
    "VK_CLEAR",                         "VK_RETURN",                        NULL,                               NULL,
    "VK_SHIFT",                         "VK_CONTROL",                       "VK_MENU",                          "VK_PAUSE",
    "VK_CAPITAL",                       "VK_KANA/VK_HANGUL",                NULL,                               "VK_JUNJA",
    "VK_FINAL",                         "VK_KANJI/VK_HANJA",                NULL,                               "VK_ESCAPE",
    "VK_CONVERT",                       "VK_NONCONVERT",                    "VK_ACCEPT",                        "VK_MODECHANGE",
    "VK_SPACE",                         "VK_PRIOR",                         "VK_NEXT",                          "VK_END",
    "VK_HOME",                          "VK_LEFT",                          "VK_UP",                            "VK_RIGHT",
    "VK_DOWN",                          "VK_SELECT",                        "VK_PRINT",                         "VK_EXECUTE",
    "VK_SNAPSHOT",                      "VK_INSERT",                        "VK_DELETE",                        "VK_HELP",
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               "VK_LWIN",
    "VK_RWIN",                          "VK_APPS",                          NULL,                               "VK_SLEEP",
    "VK_NUMPAD0",                       "VK_NUMPAD1",                       "VK_NUMPAD2",                       "VK_NUMPAD3",
    "VK_NUMPAD4",                       "VK_NUMPAD5",                       "VK_NUMPAD6",                       "VK_NUMPAD7",
    "VK_NUMPAD8",                       "VK_NUMPAD9",                       "VK_MULTIPLY",                      "VK_ADD",
    "VK_SEPARATOR",                     "VK_SUBTRACT",                      "VK_DECIMAL",                       "VK_DIVIDE",
    "VK_F1",                            "VK_F2",                            "VK_F3",                            "VK_F4",
    "VK_F5",                            "VK_F6",                            "VK_F7",                            "VK_F8",
    "VK_F9",                            "VK_F10",                           "VK_F11",                           "VK_F12",
    "VK_F13",                           "VK_F14",                           "VK_F15",                           "VK_F16",
    "VK_F17",                           "VK_F18",                           "VK_F19",                           "VK_F20",
    "VK_F21",                           "VK_F22",                           "VK_F23",                           "VK_F24",
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    "VK_NUMLOCK",                       "VK_SCROLL",                        "VK_OEM_NEC_EQUAL/VK_OEM_FJ_JISHO", "VK_OEM_FJ_MASSHOU",
    "VK_OEM_FJ_TOUROKU",                "VK_OEM_FJ_LOYA",                   "VK_OEM_FJ_ROYA",                   NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    "VK_LSHIFT",                        "VK_RSHIFT",                        "VK_LCONTROL",                      "VK_RCONTROL",
    "VK_LMENU",                         "VK_RMENU",                         "VK_BROWSER_BACK",                  "VK_BROWSER_FORWARD",
    "VK_BROWSER_REFRESH",               "VK_BROWSER_STOP",                  "VK_BROWSER_SEARCH",                "VK_BROWSER_FAVORITES",
    "VK_BROWSER_HOME",                  "VK_VOLUME_MUTE",                   "VK_VOLUME_DOWN",                   "VK_VOLUME_UP",
    "VK_MEDIA_NEXT_TRACK",              "VK_MEDIA_PREV_TRACK",              "VK_MEDIA_STOP",                    "VK_MEDIA_PLAY_PAUSE",
    "VK_LAUNCH_MAIL",                   "VK_LAUNCH_MEDIA_SELECT",           "VK_LAUNCH_APP1",                   "VK_LAUNCH_APP2",
    NULL,                               NULL,                               "VK_OEM_1",                         "VK_OEM_PLUS",
    "VK_OEM_COMMA",                     "VK_OEM_MINUS",                     "VK_OEM_PERIOD",                    "VK_OEM_2",
    "VK_OEM_3",                         NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               NULL,
    NULL,                               NULL,                               NULL,                               "VK_OEM_4",
    "VK_OEM_5",                         "VK_OEM_6",                         "VK_OEM_7",                         "VK_OEM_8",
    NULL,                               "VK_OEM_AX",                        "VK_OEM_102",                       "VK_ICO_HELP",
    "VK_ICO_00",                        "VK_PROCESSKEY",                    "VK_ICO_CLEAR",                     "VK_PACKET",
    NULL,                               "VK_OEM_RESET",                     "VK_OEM_JUMP",                      "VK_OEM_PA1",
    "VK_OEM_PA2",                       "VK_OEM_PA3",                       "VK_OEM_WSCTRL",                    "VK_OEM_CUSEL",
    "VK_OEM_ATTN",                      "VK_OEM_FINISH",                    "VK_OEM_COPY",                      "VK_OEM_AUTO",
    "VK_OEM_ENLW",                      "VK_OEM_BACKTAB",                   "VK_ATTN",                          "VK_CRSEL",
    "VK_EXSEL",                         "VK_EREOF",                         "VK_PLAY",                          "VK_ZOOM",
    "VK_NONAME",                        "VK_PA1",                           "VK_OEM_CLEAR",                     NULL,
};

const int ITEM_FUDGE_FACTOR   = 12;
const int HEADER_FUDGE_FACTOR = ITEM_FUDGE_FACTOR + 3;

IMPLEMENT_DYNAMIC( CKeySequences, CPropertyPage )

BEGIN_MESSAGE_MAP( CKeySequences, CPropertyPage )
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_KEYCOMBOLIST, &CKeySequences::OnKeyComboListItemChanged)
	ON_BN_CLICKED(IDEDIT, &CKeySequences::OnBnClickedEdit)
	ON_BN_CLICKED(IDREMOVE, &CKeySequences::OnBnClickedRemove)
END_MESSAGE_MAP( )

CKeySequences::CKeySequences( ):
	CPropertyPage( CKeySequences::IDD )
{
	memset( m_nColumnWidths, 0, sizeof( m_nColumnWidths ) );
}

CKeySequences::~CKeySequences( ) {
}

void CKeySequences::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_KEYCOMBOLIST, m_KeyComboList );
	DDX_Control( pDX, IDEDIT,           m_btnEdit );
	DDX_Control( pDX, IDREMOVE,         m_btnRemove );
}

CString CKeySequences::_VkToString( DWORD _vk ) {
	DWORD vk = (DWORD) ( _vk & (DWORD) 0x7FFFFFFF );
	bool fCapital = ( _vk & (DWORD) 0x80000000 ) != 0;
	CString str;

	if ( NULL == lpszVkNames[ vk ] ) {
		if ( ( vk >= '0' && vk <= '9' ) || ( vk >= 'A' && vk <= 'Z' ) ) {
			str = (TCHAR) vk;
		} else {
			str.Format( _T("?? (VK#%d)"), vk );
		}
	} else {
		str = lpszVkNames[ vk ];
	}
	if ( fCapital )
		str.Insert( 0, _T("Shift+") );
	return str;
}

void CKeySequences::_FillKeyComboList( void ) {
	CString col0, col1, col2;
	int width;
	BOOL ret;

	for ( int n = 0; n < ComposeKeyEntries.GetCount( ); n++ ) {
		col0 = ComposeKeyEntries[n].wchComposed;
		width = m_KeyComboList.GetStringWidth( col0 ) + ITEM_FUDGE_FACTOR;
		if ( width > m_nColumnWidths[0] )
			m_nColumnWidths[0] = width;
		int nItem = m_KeyComboList.InsertItem( LVIF_PARAM | LVIF_STATE | LVIF_TEXT, n, col0, 0, (UINT) -1, -1, n );
		ASSERT( -1 != nItem );

		col1 = _VkToString( ComposeKeyEntries[n].vkFirst );
		width = m_KeyComboList.GetStringWidth( col1 ) + ITEM_FUDGE_FACTOR;
		if ( width > m_nColumnWidths[1] )
			m_nColumnWidths[1] = width;
		ret = m_KeyComboList.SetItem( nItem, 1, LVIF_STATE | LVIF_TEXT, col1, -1, 0, (UINT) -1, 0 );
		ASSERT( TRUE == ret );

		col2 = _VkToString( ComposeKeyEntries[n].vkSecond );
		width = m_KeyComboList.GetStringWidth( col2 ) + ITEM_FUDGE_FACTOR;
		if ( width > m_nColumnWidths[2] )
			m_nColumnWidths[2] = width;
		ret = m_KeyComboList.SetItem( nItem, 2, LVIF_STATE | LVIF_TEXT, col2, -1, 0, (UINT) -1, 0 );
		ASSERT( TRUE == ret );
	}

	int m = max( m_nColumnWidths[1], m_nColumnWidths[2] );
	m_nColumnWidths[1] = m_nColumnWidths[2] = m;
	m_KeyComboList.SetColumnWidth( 0, m_nColumnWidths[0] );
	m_KeyComboList.SetColumnWidth( 1, m_nColumnWidths[1] );
	m_KeyComboList.SetColumnWidth( 2, m_nColumnWidths[2] );
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

void CKeySequences::OnBnClickedEdit( ) {
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
		ComposeKeyEntries.RemoveAt( items[n] );
	}

	delete[] items;

	FcSetComposeKeyEntries( ComposeKeyEntries.GetData( ), (DWORD) ComposeKeyEntries.GetCount( ) );

	SetModified( );
}
