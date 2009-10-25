#pragma once
#include "afxcmn.h"


// COptionsDlg dialog

class COptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionsDlg();

// Dialog Data
	enum { IDD = IDD_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	//CSize m_MinimumSize;
	int m_nColumnWidths[3];

	CTabCtrl m_Tabs;

	CListCtrl m_KeyComboList;
	CButton m_btnEdit;
	CButton m_btnRemove;

	CButton m_btnOk;
	CButton m_btnApply;
	CButton m_btnClose;

	//void _DoLayout( const int excessX = 0, const int excessY = 0 );
	void _FillKeyComboList( void );

	virtual BOOL OnInitDialog();
	//afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnKeyComboListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedCancel();
};
