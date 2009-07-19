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

private:
	CListCtrl m_KeyComboList;
	CSize m_MinimumSize;
	CButton m_btnEdit;
	CButton m_btnRemove;
	CButton m_btnClose;

	virtual BOOL OnInitDialog();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnKeyComboListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);

	void _DoLayout( const int excessX = 0, const int excessY = 0 );
};
