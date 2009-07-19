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
	int m_nColumnWidths[3];

	void _DoLayout( const int excessX = 0, const int excessY = 0 );
	void _FillKeyComboList( void );

	virtual BOOL OnInitDialog();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnKeyComboListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedClose();
};
