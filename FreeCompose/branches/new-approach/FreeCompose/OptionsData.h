#pragma once

class COptionsData {
public:
	inline COptionsData( ) {

	}

	inline COptionsData( COptionsData const& options ) {
		operator=( options );
	}

	inline ~COptionsData( ) {
		ComposeSequences.RemoveAll( );
	}

	COptionsData& operator=( COptionsData const& );
	bool operator==( COptionsData const& );
	bool operator!=( COptionsData const& );

	void Load( void );
	void Save( void );

	CArray<ComposeSequence> ComposeSequences;
 
	BOOL StartActive;
	BOOL StartWithWindows;
	CAPS_LOCK_TOGGLE_MODE CapsLockToggleMode;
	CAPS_LOCK_SWAP_MODE CapsLockSwapMode;
	DWORD ComposeVk;
	DWORD SwapCapsLockVk;

private:
	bool _LoadFromRegistry( void );
	void _LoadKeys( void );
	void _UpdateRunKey( void );

	bool _InterpretConfiguration( void* pvDoc );
	bool _LoadDefaultConfiguration( void );
	bool _LoadXmlFile( void );
	bool _SaveXmlFile( void );
};
