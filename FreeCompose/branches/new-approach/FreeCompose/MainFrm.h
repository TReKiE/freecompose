// MainFrm.h : interface of the CMainFrame class

#pragma once

#include "OptionsPropSheet.h"
#include "OptionsData.h"
#include "AutoCriticalSection.h"

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
	afx_msg LRESULT OnActivate( WPARAM, LPARAM );
	afx_msg LRESULT OnNotifyIcon( WPARAM, LPARAM );
	afx_msg LRESULT OnReconfigure( WPARAM, LPARAM lparamOptionsPropSheet );
	afx_msg LRESULT OnFcmPip( WPARAM wPip, LPARAM );
	afx_msg void OnClose( );
	afx_msg void OnAppAbout( );
	afx_msg void OnAppCapsLock( );
	afx_msg void OnAppConfigure( );
	afx_msg void OnAppExit( );
	afx_msg void OnAppToggle( );
#ifdef _DEBUG
	afx_msg void OnAppZapConf( );
#endif
	afx_msg void OnUpdateAppCapsLock( CCmdUI* );
	afx_msg void OnUpdateAppToggle( CCmdUI* );

private:
	CTrayNotifyIcon* m_pTrayIcon;
	COptionsData* m_pOptions;
	CString m_strEnabled;
	CString m_strDisabled;
	CString m_strTitle;
	COptionsPropSheet* m_pPropSheet;
	AutoCriticalSection m_csPropSheet;

	void _Initialize( void );
	void _Reconfigure( void );
	void _SetupTrayIcon( void );
	void _UpdateTooltip( void );
};
