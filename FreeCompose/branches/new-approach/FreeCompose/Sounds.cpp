#include "stdafx.h"

#include "FreeCompose.h"
#include "Sounds.h"

IMPLEMENT_DYNAMIC( CSounds, CPropertyPage )

BEGIN_MESSAGE_MAP( CSounds, CPropertyPage )
END_MESSAGE_MAP()

CSounds::CSounds( ):
	CPropertyPage( IDD )
{

}

CSounds::~CSounds( ) {

}

void CSounds::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );
}
