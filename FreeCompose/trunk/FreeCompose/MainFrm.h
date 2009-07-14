// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "ChildView.h"
#include "NTray.h"

extern const UINT FCM_ENABLE;
extern const UINT FCM_DISABLE;

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:

// Implementation
protected:
	HICON m_hIcon;
	CTrayNotifyIcon* m_ptni;
	BOOL m_fActive;
public:
	virtual ~CMainFrame();

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnFcmNotifyIcon(WPARAM, LPARAM);
	afx_msg LRESULT OnFcmEnable(WPARAM, LPARAM);
	afx_msg LRESULT OnFcmDisable(WPARAM, LPARAM);
	afx_msg LRESULT OnFcmPip(WPARAM wPip, LPARAM);
	afx_msg LRESULT OnFcmKey(WPARAM wKey, LPARAM);
	afx_msg LRESULT OnTaskbarCreated(WPARAM, LPARAM);
	afx_msg void OnClose();
};


