// MainFrm.h : interface of the CMainFrame class

#pragma once

#include "NTray.h"

#include "OptionsPropSheet.h"
#include "OptionsData.h"

#undef USE_TIMER

class CMainFrame: public CFrameWnd {
	DECLARE_DYNAMIC( CMainFrame )
	DECLARE_MESSAGE_MAP( )

public:
	CMainFrame( );
	virtual ~CMainFrame( );

protected:
	virtual BOOL PreTranslateMessage( MSG* pMsg );

	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg LRESULT OnNotifyIcon( WPARAM, LPARAM );
	afx_msg LRESULT OnReconfigure( WPARAM, LPARAM lparamOptionsPropSheet );
	afx_msg LRESULT OnFcmPip( WPARAM wPip, LPARAM );
	afx_msg void OnClose( );
	afx_msg void OnAppAbout( );
	afx_msg void OnAppToggle( );
	afx_msg void OnAppCapsLock( );
	afx_msg void OnAppConfigure( );
	afx_msg void OnAppExit( );
	afx_msg void OnUpdateAppToggle( CCmdUI* );
	afx_msg void OnUpdateAppCapsLock( CCmdUI* );
#ifdef USE_TIMER
	afx_msg void OnTimer( UINT_PTR nId );
#endif

private:
#ifdef USE_TIMER
	UINT_PTR m_uTimerId;
#endif
	CTrayNotifyIcon* m_ptni;
	COptionsData* m_pOptions;
	bool m_fActive;
	CString m_strEnabled;
	CString m_strDisabled;
	CString m_strTitle;
	COptionsPropSheet* m_pPropSheet;
	CRITICAL_SECTION m_csPropSheet;

	void _Initialize( void );
	void _Reconfigure( void );
	void _SetupTrayIcon( void );
};
