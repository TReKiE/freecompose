#pragma once

class COptionsData {
public:
	COptionsData( );
	COptionsData( const COptionsData& );
	virtual ~COptionsData( );

	COptionsData& operator=( const COptionsData& );
	bool operator==( const COptionsData& );
	bool operator!=( const COptionsData& );

	void Load( void );
	void Save( void );

	bool LoadFromXml( void );
	bool SaveToXml( void );

	CComposeSequenceArray m_ComposeSequences;
 
	BOOL m_fStartActive;
	BOOL m_fStartWithWindows;
	BOOL m_fSwapCapsLock;
	CAPS_LOCK_TOGGLE_MODE m_CapsLockToggleMode;
	CAPS_LOCK_SWAP_MODE m_CapsLockSwapMode;
	DWORD m_vkCompose;
	DWORD m_vkSwapCapsLock;

private:
	bool _FcValidateSequence( const COMPOSE_SEQUENCE& sequence );
	void _FcLoadKeys( void );
	void _FcSaveKeys( void );
	void _UpdateRunKey( void );
};
