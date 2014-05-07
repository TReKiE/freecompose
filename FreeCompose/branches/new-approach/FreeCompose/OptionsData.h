#pragma once

#include "XmlOptionsManager.h"

class COptionsData {
public:
	inline COptionsData( ): _xmlOptionsHandler( this ) { }

	inline COptionsData( COptionsData const& options ):
		COptionsData( )
	{
		operator=( options );
	}

	inline ~COptionsData( ) {
		ComposeSequences.RemoveAll( );
	}

	COptionsData& operator=( COptionsData const& );
	bool operator==( COptionsData const& ) const;
	bool operator!=( COptionsData const& ) const;

	//
	// Methods
	//

	void Load( void );
	void Save( void );

	//
	// Configuration store
	//

	CArray<ComposeSequence> ComposeSequences;
 
	BOOL StartActive;
	BOOL StartWithWindows;
	CAPS_LOCK_TOGGLE_MODE CapsLockToggleMode;
	CAPS_LOCK_SWAP_MODE CapsLockSwapMode;
	DWORD ComposeVk;
	DWORD SwapCapsLockVk;

private:
	CXmlOptionsManager _xmlOptionsHandler;

	bool _CheckIfRegistryKeyExists( void ) const;
	void _LoadSequencesFromRegistry( void );
	bool _LoadFromRegistry( void );
	void _UpdateRunKey( void );
};
