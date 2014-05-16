#pragma once

#include "OptionsData.h"

class CFeatures: public CPropertyPage {
	DECLARE_DYNAMIC( CFeatures )
	DECLARE_MESSAGE_MAP( )

public:
	enum { IDD = IDD_FEATURES };

	CFeatures( COptionsData& Options );
	virtual ~CFeatures( );

private:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );

	afx_msg void OnChanged( void );
	afx_msg void OnComposeKeyChanged( );
	afx_msg void OnSwapCapsLockChanged( );
	afx_msg void OnSwapCapsLockKeyChanged( );
	afx_msg void OnCapsLockModeChanged( UINT uID );

private:
	COptionsData& m_Options;

	CComboBox m_cbComposeKey;
	CComboBox m_cbSwapCapsLockKey;
	CButton m_btnSwapCapsLock;
	CButton m_btnCltmNormal;
	CButton m_btnCltmPressTwice;
	CButton m_btnCltmDisabled;
};
