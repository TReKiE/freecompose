#pragma once
#include "afxcmn.h"


// CConfigureDlg dialog

class CConfigureDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigureDlg)

public:
	CConfigureDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigureDlg();

// Dialog Data
	enum { IDD = IDD_CONFIGURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ShortcutList;
};
