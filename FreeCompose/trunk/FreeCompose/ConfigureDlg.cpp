// ConfigureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FreeCompose.h"
#include "ConfigureDlg.h"


// CConfigureDlg dialog

IMPLEMENT_DYNAMIC(CConfigureDlg, CDialog)

CConfigureDlg::CConfigureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigureDlg::IDD, pParent)
{

}

CConfigureDlg::~CConfigureDlg()
{
}

void CConfigureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHORTCUTLIST, m_ShortcutList);
}


BEGIN_MESSAGE_MAP(CConfigureDlg, CDialog)
END_MESSAGE_MAP()


// CConfigureDlg message handlers
