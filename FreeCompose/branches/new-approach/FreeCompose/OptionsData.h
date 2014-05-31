#pragma once

#include "SoundOptions.h"
#include "XmlOptionsManager.h"

class COptionsData {
public:
	inline COptionsData( ):
		_xmlOptionsHandler( this )
	{

	}

	inline COptionsData( COptionsData const& options ):
		COptionsData( )
	{
		operator=( options );
	}

	inline ~COptionsData( ) {

	}

	COptionsData& operator=( COptionsData const& );
	bool operator==( COptionsData const& ) const;
	bool operator!=( COptionsData const& ) const;

	//
	// Methods
	//

	ComposeSequenceGroup* FindComposeSequenceGroup( CString const& name );

	void Load( void );
	void Save( void );

	//
	// Configuration store
	//

	BOOL StartActive;
	BOOL StartWithWindows;
	CAPS_LOCK_TOGGLE_MODE CapsLockToggleMode;
	CAPS_LOCK_SWAP_MODE CapsLockSwapMode;
	DWORD ComposeVk;
	DWORD SwapCapsLockVk;

	ComposeSequenceGroupArray ComposeSequenceGroups;
 
private:
	CXmlOptionsManager _xmlOptionsHandler;

	bool _CheckIfRegistryKeyExists( void ) const;
	void _LoadSequencesFromRegistry( void );
	bool _LoadFromRegistry( void );
	void _UpdateRunKey( void );
};
