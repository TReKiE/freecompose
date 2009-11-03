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

COptionsPropSheet::COptionsPropSheet( const COptionsData& options ):
	CPropertySheet ( _T("FreeCompose options"), NULL ),
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
	ModifyStyle( 0, WS_MINIMIZEBOX );
	ModifyStyleEx( 0, WS_EX_APPWINDOW );
	return CPropertySheet::OnCreate( lpcs );
}

BOOL COptionsPropSheet::PreTranslateMessage( MSG* pMsg ) {
	if ( IsDialogMessage( pMsg ) )
		return TRUE;
	return CPropertySheet::PreTranslateMessage( pMsg );
}
