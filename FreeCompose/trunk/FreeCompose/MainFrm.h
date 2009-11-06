// MainFrm.h : interface of the CMainFrame class

#pragma once

#include "OptionsData.h"
#include "NTray.h"

class CMainFrame: public CFrameWnd {
public:
	CMainFrame( );
	virtual ~CMainFrame( );

	virtual BOOL PreTranslateMessage( MSG* pMsg );

	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT OnNotifyIcon( WPARAM, LPARAM );
	afx_msg LRESULT OnReconfigure( WPARAM, LPARAM lparamOptionsPropSheet );
	afx_msg LRESULT OnFcmPip( WPARAM wPip, LPARAM );
	afx_msg void OnClose( );
	afx_msg void OnAppAbout( );
	afx_msg void OnAppEnable( );
	afx_msg void OnAppDisable( );
	afx_msg void OnAppCapsLock( );
	afx_msg void OnAppConfigure( );
	afx_msg void OnUpdateAppEnable( CCmdUI* );
	afx_msg void OnUpdateAppCapsLock( CCmdUI* );

protected:
	DECLARE_MESSAGE_MAP( )

private:
	CTrayNotifyIcon* m_ptni;
	COptionsData* m_pOptions;
	bool m_fActive;

	void _Initialize( void );
	void _Reconfigure( void );
	void _SetupTrayIcon( void );

	DECLARE_DYNAMIC( CMainFrame )
};
