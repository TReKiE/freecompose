#pragma once

#include "OptionsData.h"

class CSounds: public CPropertyPage {
	DECLARE_DYNAMIC( CSounds )
	DECLARE_MESSAGE_MAP( )

public:
	CSounds( ) = delete;
	CSounds( CSounds const& ) = delete;
	CSounds( CSounds&& ) = delete;
	CSounds& operator=( CSounds const& ) = delete;
	CSounds& operator=( CSounds&& ) = delete;

	inline CSounds( COptionsData& Options ):
		CPropertyPage  ( IDT_SOUNDS ),
		m_Options      ( Options ),
		m_Schemes      ( Options.Sounds.Schemes ),
		m_nSchemeIndex ( 0 ),
		m_nEventIndex  ( 0 ),
		m_nRadioIndex  ( 0 ),
		m_nFrequency   ( 0 ),
		m_nDuration    ( 0 )
	{

	}

	inline ~CSounds( )
	{

	}

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

	SoundSchemeVector& m_Schemes;

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );

	afx_msg void OnAdd( );
	afx_msg void OnRename( );
	afx_msg void OnRemove( );
	afx_msg void OnBrowse( );
	afx_msg void OnUpdateRemove( CCmdUI* pui );
	afx_msg void OnUpdateBrowse( CCmdUI* pui );

};
