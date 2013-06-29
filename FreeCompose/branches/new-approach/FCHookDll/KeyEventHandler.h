#pragma once

//==============================================================================
// Data types
//==============================================================================

enum DISPOSITION {
	D_NOT_HANDLED,
	D_ACCEPT_KEY,
	D_REJECT_KEY,
	D_REGENERATE_KEY,
};

//==============================================================================
// Classes and functions
//==============================================================================

class KeyEventHandler {
public:

	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* /*pkb*/ );
	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* /*pkb*/ );

};

namespace Stringify {

	extern wchar_t const* stringsFor_enum_DISPOSITION[];

	static inline wchar_t const* from_DISPOSITION( DISPOSITION const value ) {
		return stringsFor_enum_DISPOSITION[static_cast<int>( value )];
	}

};
