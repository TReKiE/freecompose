#include "stdafx.h"

#include "FreeCompose.h"
#include "OptionsPropSheet.h"

#include "Utils.h"

IMPLEMENT_DYNAMIC( COptionsPropSheet, CPropertySheet )

BEGIN_MESSAGE_MAP( COptionsPropSheet, CPropertySheet )
	//{{AFX_MSG_MAP( COptionsPropSheet )
	ON_WM_CREATE( )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

COptionsPropSheet::COptionsPropSheet( CWnd* pParentWnd, const COptionsData& options ):
	CPropertySheet ( _T("FreeCompose options"), pParentWnd ),
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
	ModifyStyle( WS_POPUP, 0 );
	ModifyStyle( 0, WS_CHILD | WS_MINIMIZEBOX | WS_SYSMENU );
	ModifyStyleEx( 0, WS_EX_APPWINDOW | WS_EX_CONTROLPARENT );
	return CPropertySheet::OnCreate( lpcs );
}
