// KeySequences.cpp : implementation file

#include "stdafx.h"

#include "FreeCompose.h"
#include "KeySequences.h"

IMPLEMENT_DYNAMIC( CKeySequences, CPropertyPage )

BEGIN_MESSAGE_MAP( CKeySequences, CPropertyPage )
END_MESSAGE_MAP()

CKeySequences::CKeySequences( ):
	CPropertyPage( CKeySequences::IDD )
{
}

CKeySequences::~CKeySequences( ) {
}

void CKeySequences::DoDataExchange( CDataExchange* pDX ) {
	CPropertyPage::DoDataExchange( pDX );
}
