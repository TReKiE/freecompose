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

	CSounds( COptionsData& Options );
	~CSounds( );

private:
	COptionsData& m_Options;

	CComboBox m_comboScheme;
	CButton m_buttonRename;
	CButton m_buttonRemove;
	CComboBox m_comboEvent;
	CButton m_radioNoSound;
	CButton m_radioApplicationSound;
	CEdit m_editFileName;
	CButton m_buttonBrowse;
	CButton m_radioToneSound;
	CEdit m_editFrequency;
	CEdit m_editDuration;

	int m_nCurrentScheme;
	int m_nCurrentEvent;
	int m_nRadioIndex;
	CString m_strFileName;
	int m_nFrequency;
	int m_nDuration;

	SoundSchemeVector& m_Schemes;

	inline SoundScheme& _GetCurrentScheme( void ) const;
	inline CString      _GetCurrentSchemeId( void ) const;
	inline SoundEvent*  _GetCurrentEvent( void ) const;
	inline CString      _GetCurrentEventName( void ) const;
	inline void         _SetCurrentEvent( SoundEvent* pEvent );

	void _UpdateNoSound( void );
	void _UpdateApplicationSound( void );
	void _UpdateToneSound( void );
	void _UpdateEventGroup( void );

	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );

	afx_msg void OnAdd( );
	afx_msg void OnRename( );
	afx_msg void OnRemove( );
	afx_msg void OnBrowse( );
	afx_msg void OnSchemeChanged( );
	afx_msg void OnEventChanged( );
	afx_msg void OnNoSoundClicked( );
	afx_msg void OnApplicationSoundClicked( );
	afx_msg void OnToneSoundClicked( );

	using MethodPtr = void (CSounds::*)( void );
	static MethodPtr const _EventGroupDispatchTable[];
};
