// ConfigureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FreeCompose.h"
#include "ConfigureDlg.h"


LPCTSTR lpszVkNames[256] = {
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    _T("VK_BACK"),                          _T("VK_TAB"),                           NULL,                                   NULL,
    _T("VK_CLEAR"),                         _T("VK_RETURN"),                        NULL,                                   NULL,
    _T("VK_SHIFT"),                         _T("VK_CONTROL"),                       _T("VK_MENU"),                          _T("VK_PAUSE"),
    _T("VK_CAPITAL"),                       _T("VK_KANA/VK_HANGUL"),                NULL,                                   _T("VK_JUNJA"),
    _T("VK_FINAL"),                         _T("VK_HANJA/VK_KANJI"),                NULL,                                   _T("VK_ESCAPE"),
    _T("VK_CONVERT"),                       _T("VK_NONCONVERT"),                    _T("VK_ACCEPT"),                        _T("VK_MODECHANGE"),
    _T("VK_SPACE"),                         _T("VK_PRIOR"),                         _T("VK_NEXT"),                          _T("VK_END"),
    _T("VK_HOME"),                          _T("VK_LEFT"),                          _T("VK_UP"),                            _T("VK_RIGHT"),
    _T("VK_DOWN"),                          _T("VK_SELECT"),                        _T("VK_PRINT"),                         _T("VK_EXECUTE"),
    _T("VK_SNAPSHOT"),                      _T("VK_INSERT"),                        _T("VK_DELETE"),                        _T("VK_HELP"),
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   _T("VK_LWIN"),
    _T("VK_RWIN"),                          _T("VK_APPS"),                          NULL,                                   _T("VK_SLEEP"),
    _T("VK_NUMPAD0"),                       _T("VK_NUMPAD1"),                       _T("VK_NUMPAD2"),                       _T("VK_NUMPAD3"),
    _T("VK_NUMPAD4"),                       _T("VK_NUMPAD5"),                       _T("VK_NUMPAD6"),                       _T("VK_NUMPAD7"),
    _T("VK_NUMPAD8"),                       _T("VK_NUMPAD9"),                       _T("VK_MULTIPLY"),                      _T("VK_ADD"),
    _T("VK_SEPARATOR"),                     _T("VK_SUBTRACT"),                      _T("VK_DECIMAL"),                       _T("VK_DIVIDE"),
    _T("VK_F1"),                            _T("VK_F2"),                            _T("VK_F3"),                            _T("VK_F4"),
    _T("VK_F5"),                            _T("VK_F6"),                            _T("VK_F7"),                            _T("VK_F8"),
    _T("VK_F9"),                            _T("VK_F10"),                           _T("VK_F11"),                           _T("VK_F12"),
    _T("VK_F13"),                           _T("VK_F14"),                           _T("VK_F15"),                           _T("VK_F16"),
    _T("VK_F17"),                           _T("VK_F18"),                           _T("VK_F19"),                           _T("VK_F20"),
    _T("VK_F21"),                           _T("VK_F22"),                           _T("VK_F23"),                           _T("VK_F24"),
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    _T("VK_NUMLOCK"),                       _T("VK_SCROLL"),                        _T("VK_OEM_NEC_EQUAL/VK_OEM_FJ_JISHO"), _T("VK_OEM_FJ_MASSHOU"),
    _T("VK_OEM_FJ_TOUROKU"),                _T("VK_OEM_FJ_LOYA"),                   _T("VK_OEM_FJ_ROYA"),                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    _T("VK_LSHIFT"),                        _T("VK_RSHIFT"),                        _T("VK_LCONTROL"),                      _T("VK_RCONTROL"),
    _T("VK_LMENU"),                         _T("VK_RMENU"),                         _T("VK_BROWSER_BACK"),                  _T("VK_BROWSER_FORWARD"),
    _T("VK_BROWSER_REFRESH"),               _T("VK_BROWSER_STOP"),                  _T("VK_BROWSER_SEARCH"),                _T("VK_BROWSER_FAVORITES"),
    _T("VK_BROWSER_HOME"),                  _T("VK_VOLUME_MUTE"),                   _T("VK_VOLUME_DOWN"),                   _T("VK_VOLUME_UP"),
    _T("VK_MEDIA_NEXT_TRACK"),              _T("VK_MEDIA_PREV_TRACK"),              _T("VK_MEDIA_STOP"),                    _T("VK_MEDIA_PLAY_PAUSE"),
    _T("VK_LAUNCH_MAIL"),                   _T("VK_LAUNCH_MEDIA_SELECT"),           _T("VK_LAUNCH_APP1"),                   _T("VK_LAUNCH_APP2"),
    NULL,                                   NULL,                                   _T("VK_OEM_1"),                         _T("VK_OEM_PLUS"),
    _T("VK_OEM_COMMA"),                     _T("VK_OEM_MINUS"),                     _T("VK_OEM_PERIOD"),                    _T("VK_OEM_2"),
    _T("VK_OEM_3"),                         NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   NULL,
    NULL,                                   NULL,                                   NULL,                                   _T("VK_OEM_4"),
    _T("VK_OEM_5"),                         _T("VK_OEM_6"),                         _T("VK_OEM_7"),                         _T("VK_OEM_8"),
    NULL,                                   _T("VK_OEM_AX"),                        _T("VK_OEM_102"),                       _T("VK_ICO_HELP"),
    _T("VK_ICO_00"),                        _T("VK_PROCESSKEY"),                    _T("VK_ICO_CLEAR"),                     _T("VK_PACKET"),
    NULL,                                   _T("VK_OEM_RESET"),                     _T("VK_OEM_JUMP"),                      _T("VK_OEM_PA1"),
    _T("VK_OEM_PA2"),                       _T("VK_OEM_PA3"),                       _T("VK_OEM_WSCTRL"),                    _T("VK_OEM_CUSEL"),
    _T("VK_OEM_ATTN"),                      _T("VK_OEM_FINISH"),                    _T("VK_OEM_COPY"),                      _T("VK_OEM_AUTO"),
    _T("VK_OEM_ENLW"),                      _T("VK_OEM_BACKTAB"),                   _T("VK_ATTN"),                          _T("VK_CRSEL"),
    _T("VK_EXSEL"),                         _T("VK_EREOF"),                         _T("VK_PLAY"),                          _T("VK_ZOOM"),
    _T("VK_NONAME"),                        _T("VK_PA1"),                           _T("VK_OEM_CLEAR"),                     NULL,
};


// CConfigureDlg dialog

IMPLEMENT_DYNAMIC(CConfigureDlg, CDialog)

CConfigureDlg::CConfigureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigureDlg::IDD, pParent)
{

}

CConfigureDlg::~CConfigureDlg()
{
}

void CConfigureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KEYCOMBOLIST, m_KeyComboList);
}


BEGIN_MESSAGE_MAP(CConfigureDlg, CDialog)
	ON_WM_SIZING()
END_MESSAGE_MAP()


// CConfigureDlg message handlers

BOOL CConfigureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rect;
		GetWindowRect( &rect );
		m_MinimumSize = rect.Size( );
	}

	m_KeyComboList.InsertColumn( 0, _T("Character"), LVCFMT_LEFT, 60 );
	m_KeyComboList.InsertColumn( 1, _T("Key 1"),     LVCFMT_LEFT, 60, 1 );
	m_KeyComboList.InsertColumn( 2, _T("Key 2"),     LVCFMT_LEFT, 60, 2 );

	const LPCTSTR IsCap   = _T("Shift+");
	const LPCTSTR IsntCap = _T("");

	TCHAR buf[64];
	LPCTSTR lpszCapital;
	LPCTSTR lpszName;
	DWORD dwVk;
	bool fCapital;
	BOOL rc;
	for ( int n = 0; n < ComposeKeyEntries.GetCount( ); n++ ) {
		_sntprintf_s( buf, 64, _TRUNCATE, _T("%c"), ComposeKeyEntries[n].wchComposed );
		int nItem = m_KeyComboList.InsertItem( LVIF_PARAM | LVIF_STATE | LVIF_TEXT, n, buf, 0, (UINT) -1, -1, n );
		ASSERT( -1 != nItem );

		dwVk = (DWORD) ( ComposeKeyEntries[n].vkFirst & (DWORD) 0x7FFFFFFF );
		fCapital = ( ComposeKeyEntries[n].vkFirst & (DWORD) 0x80000000 ) != 0;
		lpszCapital = fCapital ? IsCap : IsntCap;
		lpszName = lpszVkNames[ dwVk ];
		if ( NULL == lpszName ) {
			if ( ( dwVk >= '0' && dwVk <= '9' ) || ( dwVk >= 'A' && dwVk <= 'Z' ) ) {
				_sntprintf_s( buf, 64, _TRUNCATE, _T("%s%c"), lpszCapital, dwVk );
			} else {
				_sntprintf_s( buf, 64, _TRUNCATE, _T("%s?? VK#%d"), lpszCapital, dwVk );
			}
		} else {
			_sntprintf_s( buf, 64, _TRUNCATE, _T("%s%s"), lpszCapital, lpszName );
		}
		rc = m_KeyComboList.SetItem( nItem, 1, LVIF_STATE | LVIF_TEXT, buf, -1, 0, (UINT) -1, 0 );
		ASSERT( TRUE == rc );

		dwVk = (DWORD) ( ComposeKeyEntries[n].vkSecond & (DWORD) 0x7FFFFFFF );
		fCapital = ( ComposeKeyEntries[n].vkSecond & (DWORD) 0x80000000 ) != 0;
		lpszCapital = fCapital ? IsCap : IsntCap;
		lpszName = lpszVkNames[ dwVk ];
		if ( NULL == lpszName ) {
			if ( ( dwVk >= '0' && dwVk <= '9' ) || ( dwVk >= 'A' && dwVk <= 'Z' ) ) {
				_sntprintf_s( buf, 64, _TRUNCATE, _T("%s%c"), lpszCapital, dwVk );
			} else {
				_sntprintf_s( buf, 64, _TRUNCATE, _T("%s?? VK#%d"), lpszCapital, dwVk );
			}
		} else {
			_sntprintf_s( buf, 64, _TRUNCATE, _T("%s%s"), lpszCapital, lpszName );
		}
		rc = m_KeyComboList.SetItem( nItem, 2, LVIF_STATE | LVIF_TEXT, buf, -1, 0, (UINT) -1, 0 );
		ASSERT( TRUE == rc );
	}

	return TRUE;
}

void CConfigureDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	if ( pRect->right - pRect->left < m_MinimumSize.cx )
		pRect->right = pRect->left + m_MinimumSize.cx;
	if ( pRect->bottom - pRect->top < m_MinimumSize.cy )
		pRect->bottom = pRect->top + m_MinimumSize.cy;
	CDialog::OnSizing(fwSide, pRect);
}
