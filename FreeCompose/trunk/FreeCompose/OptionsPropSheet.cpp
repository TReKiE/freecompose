#include "stdafx.h"
#include "FreeCompose.h"
#include "OptionsPropSheet.h"

IMPLEMENT_DYNAMIC( COptionsPropSheet, CPropertySheet )
BEGIN_MESSAGE_MAP( COptionsPropSheet, CPropertySheet )
END_MESSAGE_MAP()

COptionsPropSheet::COptionsPropSheet( ):
	CPropertySheet( _T("FreeCompose options") )
{
	AddPage( &m_page1 );
}

COptionsPropSheet::~COptionsPropSheet( )
{

}
