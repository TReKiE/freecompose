#include "stdafx.h"

#include "FreeCompose.h"
#include "Sounds.h"

IMPLEMENT_DYNAMIC( CSounds, CPropertyPage )

BEGIN_MESSAGE_MAP( CSounds, CPropertyPage )
END_MESSAGE_MAP()

CSounds::CSounds( COptionsData& Options ):
	CPropertyPage( IDD ),
	m_Options( Options )
{

}

CSounds::~CSounds( ) {

}

void CSounds::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );
}

BOOL CSounds::OnInitDialog( ) {
	if ( !CPropertyPage::OnInitDialog( ) ) {
		return FALSE;
	}

	return TRUE;
}
