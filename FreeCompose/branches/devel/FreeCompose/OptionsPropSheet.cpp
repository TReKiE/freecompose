#include "stdafx.h"

#include "FreeCompose.h"
#include "OptionsPropSheet.h"

#include "Utils.h"

IMPLEMENT_DYNAMIC( COptionsPropSheet, CPropertySheet )

BEGIN_MESSAGE_MAP( COptionsPropSheet, CPropertySheet )
	//{{AFX_MSG_MAP( COptionsPropSheet )
	ON_WM_CREATE( )
	ON_COMMAND( ID_APPLY_NOW, OnApplyNow )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const UINT APP_RECONFIGURE = RegisterWindowMessage( _T("FreeCompose.APP_RECONFIGURE") );

COptionsPropSheet::COptionsPropSheet( const COptionsData& options, CWnd* pParentWnd, UINT iSelectPage ):
	CPropertySheet ( CString( (LPCWSTR) IDS_OPTIONSPROPSHEET_TITLE ), pParentWnd, iSelectPage ),
	m_CurOptions   ( options ),
	m_NewOptions   ( m_CurOptions ),
	m_KeySequences ( m_NewOptions ),
	m_Features     ( m_NewOptions )
{
	AddPage( &m_KeySequences );
	AddPage( &m_Features );

	m_psh.hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	m_psh.dwFlags |= PSH_USEHICON;
}

COptionsPropSheet::~COptionsPropSheet( ) {
}

int COptionsPropSheet::OnCreate( LPCREATESTRUCT lpcs ) {
	int ret = CPropertySheet::OnCreate( lpcs );
	if ( 0 != ret ) {
		return ret;
	}

	// Default Property Sheet window styles:
	//
	//      -- style:   1000 0100 1100 1000 0010 0000 1100 0100
	//                  |     |   ||   |      |       ||    `--- DS_3DLOOK
	//                  |     |   ||   |      |       |`-------- DS_SETFONT
	//                  |     |   ||   |      |       `--------- DS_MODALFRAME
	//                  |     |   ||   |      `----------------- WS_MINIMIZEBOX
	//                  |     |   ||   `------------------------ WS_SYSMENU
	//                  |     |   |`---------------------------- WS_DLGFRAME \ together,
	//                  |     |   `----------------------------- WS_BORDER   / WS_CAPTION
	//                  |     `--------------------------------- WS_CLIPSIBLINGS
	//                  `--------------------------------------- WS_POPUP
	//
	//      -- exstyle: 0000 0000 0000 0001 0000 0001 0000 0001
	//                                    |         |         `- WS_EX_DLGMODALFRAME
	//                                    |         `----------- WS_EX_WINDOWEDGE
	//                                    `--------------------- WS_EX_CONTROLPARENT

	ModifyStyle( WS_MAXIMIZEBOX, WS_MINIMIZEBOX );
	ModifyStyleEx( 0U, WS_EX_APPWINDOW );

	return 0;
}

void COptionsPropSheet::OnApplyNow( ) {
	m_KeySequences.UpdateData( TRUE );
	m_Features.UpdateData( TRUE );

	if ( m_pParentWnd && m_CurOptions != m_NewOptions ) {
		m_pParentWnd->SendMessage( APP_RECONFIGURE, 0, (LPARAM) this );
	}
	
	m_KeySequences.SetModified( FALSE );
	m_Features.SetModified( FALSE );
}
