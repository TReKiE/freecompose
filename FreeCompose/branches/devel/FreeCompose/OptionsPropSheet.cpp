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
}

COptionsPropSheet::~COptionsPropSheet( ) {
}

int COptionsPropSheet::OnCreate( LPCREATESTRUCT lpcs ) {
	ModifyStyle( 0, ( DS_3DLOOK|DS_CONTROL|WS_CHILD|WS_TABSTOP ) | WS_MINIMIZEBOX );
	ModifyStyleEx( 0, WS_EX_APPWINDOW );
	return CPropertySheet::OnCreate( lpcs );
}

BOOL COptionsPropSheet::PreTranslateMessage( MSG* pMsg ) {
	if ( IsDialogMessage( pMsg ) )
		return TRUE;
	return CPropertySheet::PreTranslateMessage( pMsg );
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
