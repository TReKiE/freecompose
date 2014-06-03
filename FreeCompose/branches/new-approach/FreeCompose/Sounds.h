#pragma once

#include "OptionsData.h"

class CSounds: public CPropertyPage {
	DECLARE_DYNAMIC( CSounds )
	DECLARE_MESSAGE_MAP( )

public:
	CSounds( COptionsData& Options );
	virtual ~CSounds( );

private:
	COptionsData& m_Options;

	CComboBox m_comboScheme;
	CButton m_buttonRemove;
	CComboBox m_comboEvent;
	CButton m_radioNoSound;
	CButton m_radioApplicationSound;
	CEdit m_editFileName;
	CButton m_buttonBrowse;
	CButton m_radioToneSound;
	CEdit m_editFrequency;
	CEdit m_editDuration;

	int m_nSchemeIndex;
	int m_nEventIndex;
	int m_nRadioIndex;
	CString m_strFileName;
	int m_nFrequency;
	int m_nDuration;

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );

	afx_msg void OnAdd( );
	afx_msg void OnRename( );
	afx_msg void OnRemove( );
	afx_msg void OnBrowse( );

};
