// KeySequences.cpp : implementation file
//

#include "stdafx.h"
#include "FreeCompose.h"
#include "KeySequences.h"


// CKeySequences dialog

IMPLEMENT_DYNAMIC(CKeySequences, CPropertyPage)

CKeySequences::CKeySequences()
	: CPropertyPage(CKeySequences::IDD)
{

}

CKeySequences::~CKeySequences()
{
}

void CKeySequences::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CKeySequences, CPropertyPage)
END_MESSAGE_MAP()


// CKeySequences message handlers
