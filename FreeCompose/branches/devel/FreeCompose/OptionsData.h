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

	CComposeKeyEntryArray m_ComposeKeyEntries;
 
	BOOL m_fStartActive;
	BOOL m_fStartWithWindows;
	BOOL m_fSwapCtrlAndCaps;
	BOOL m_fDisableCapsLock;

private:
	bool _FcValidateCke( const COMPOSE_KEY_ENTRY& cke );
	void _FcLoadKeys( void );
	void _FcSaveKeys( void );
	void _UpdateRunKey( void );
};
