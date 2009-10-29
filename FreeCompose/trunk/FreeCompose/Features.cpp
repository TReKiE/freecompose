// Features.cpp : implementation file

#include "stdafx.h"

#include "FreeCompose.h"
#include "Features.h"

#include "Utils.h"

IMPLEMENT_DYNAMIC( CFeatures, CPropertyPage )

BEGIN_MESSAGE_MAP( CFeatures, CPropertyPage )
END_MESSAGE_MAP( )

CFeatures::CFeatures( ):
	CPropertyPage( CFeatures::IDD )
{
}

CFeatures::~CFeatures( ) {
}

void CFeatures::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );
}
