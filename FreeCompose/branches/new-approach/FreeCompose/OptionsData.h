#pragma once

class COptionsData {
public:
	COptionsData( );
	COptionsData( COptionsData const& );
	virtual ~COptionsData( );

	COptionsData& operator=( COptionsData const& );
	bool operator==( COptionsData const& );
	bool operator!=( COptionsData const& );

	void Load( void );
	void Save( void );

	bool LoadFromXml( void );
	bool SaveToXml( void );

	CComposeSequenceArray m_ComposeSequences;
 
	BOOL m_fStartActive;
	BOOL m_fStartWithWindows;
	CAPS_LOCK_TOGGLE_MODE m_CapsLockToggleMode;
	CAPS_LOCK_SWAP_MODE m_CapsLockSwapMode;
	DWORD m_vkCompose;
	DWORD m_vkSwapCapsLock;

private:
	bool _FcValidateSequence( COMPOSE_SEQUENCE const& sequence );
	void _FcLoadKeys( void );
	void _FcSaveKeys( void );
	void _UpdateRunKey( void );
};
