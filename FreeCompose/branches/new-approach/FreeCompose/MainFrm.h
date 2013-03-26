// MainFrm.h : interface of the CMainFrame class

#pragma once

#include "OptionsPropSheet.h"
#include "OptionsData.h"

#undef USE_TIMER

// forward declaration
class CTrayNotifyIcon;

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

private:
	CTrayNotifyIcon* m_ptni;
	COptionsData* m_pOptions;
	CString m_strEnabled;
	CString m_strDisabled;
	CString m_strTitle;
	COptionsPropSheet* m_pPropSheet;
	CRITICAL_SECTION m_csPropSheet;

	void _Initialize( void );
	void _Reconfigure( void );
	void _SetupTrayIcon( void );

#ifdef USE_TIMER
protected:
	afx_msg void OnTimer( UINT_PTR nId );

private:
	UINT_PTR m_uTimerId;
#endif
};
