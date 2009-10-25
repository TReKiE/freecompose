// MainFrm.h : interface of the CMainFrame class

#pragma once

#include "ChildView.h"
#include "NTray.h"

extern const UINT FCM_ENABLE;
extern const UINT FCM_DISABLE;
extern const UINT FCM_CAPSLOCK;

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
	HICON m_hIcon;
	CTrayNotifyIcon* m_ptni;
	BOOL m_fActive;
	CMenu m_popup;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP( )

	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT OnFcmNotifyIcon( WPARAM, LPARAM );
	afx_msg LRESULT OnFcmEnable( WPARAM, LPARAM );
	afx_msg LRESULT OnFcmDisable( WPARAM, LPARAM );
	afx_msg LRESULT OnFcmCapsLock( WPARAM, LPARAM );
	afx_msg LRESULT OnFcmPip( WPARAM wPip, LPARAM );
	afx_msg LRESULT OnFcmKey( WPARAM wKey, LPARAM );
	afx_msg LRESULT OnTaskbarCreated( WPARAM, LPARAM );
	afx_msg void OnClose( );

private:
	void _InitializeHook( void );
	bool _IsCapsLock( void );
};
