// MainFrm.h : interface of the CMainFrame class

#pragma once

#include "ChildView.h"
#include "NTray.h"

class CMainFrame: public CFrameWnd {
public:
	CMainFrame( );
	virtual ~CMainFrame( );

protected: 
	DECLARE_DYNAMIC( CMainFrame )

// Attributes
public:

// Operations
public:

// Overrides
public:

// Implementation
protected:
	CTrayNotifyIcon* m_ptni;
	BOOL m_fActive;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP( )

	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT OnNotifyIcon( WPARAM, LPARAM );
	afx_msg LRESULT OnFcmPip( WPARAM wPip, LPARAM );
	afx_msg void OnClose( );
	afx_msg void OnAppAbout( );
	afx_msg void OnAppEnable( );
	afx_msg void OnAppDisable( );
	afx_msg void OnAppCapsLock( );
	afx_msg void OnAppConfigure( );
	afx_msg void OnUpdateAppEnable( CCmdUI* );
	afx_msg void OnUpdateAppCapsLock( CCmdUI* );

private:
	void _InitializeHook( void );
	void _SetupTrayIcon( void );
};
