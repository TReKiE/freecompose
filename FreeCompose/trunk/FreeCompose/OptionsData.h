#pragma once

class COptionsData {
public:
	COptionsData( );
	COptionsData( const COptionsData& );
	virtual ~COptionsData( );

	COptionsData& operator=( const COptionsData& );
	bool operator==( const COptionsData& );
	bool operator!=( const COptionsData& );

	CComposeKeyEntryArray m_ComposeKeyEntries;
 
	BOOL m_fStartActive;
	BOOL m_fStartWithWindows;
	BOOL m_fSwapCtrlAndCaps;
	BOOL m_fDisableCapsLock;

private:
	void _FcLoadKeys( void );
	void _FcSaveKeys( void );
};
