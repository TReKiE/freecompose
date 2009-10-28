// ConfigureDlg.cpp : implementation file

#include "stdafx.h"

#include "FreeCompose.h"
#include "ConfigureDlg.h"

#include "Utils.h"

// COptionsDlg dialog

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COptionsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDAPPLY, &COptionsDlg::OnBnClickedApply)
	ON_BN_CLICKED(IDCANCEL, &COptionsDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{
}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TABS, m_Tabs);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDAPPLY, m_btnApply);
	DDX_Control(pDX, IDCANCEL, m_btnClose);
}

// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// do tab stuff
	m_Tabs.InsertItem( 0, _T("Key sequences") );
	m_Tabs.InsertItem( 1, _T("Features") );

	return TRUE;
}

void COptionsDlg::OnBnClickedOk( ) {
	OnBnClickedApply( );
	this->EndDialog( IDOK );
}

void COptionsDlg::OnBnClickedApply( ) {
}

void COptionsDlg::OnBnClickedCancel( ) {
	this->EndDialog( IDCANCEL );
}
