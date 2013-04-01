#pragma once

//==============================================================================
// Constants
//==============================================================================

extern wchar_t const* stringsFor_bool[2];
extern wchar_t const* stringsFor_enum_DISPOSITION[4];

//==============================================================================
// Classes and functions
//==============================================================================

class Stringify {
public:

	static wchar_t const* from_bool( bool const value ) {
		return stringsFor_bool[static_cast<int>( value )];
	}

	static wchar_t const* from_DISPOSITION( DISPOSITION const value ) {
		return stringsFor_enum_DISPOSITION[static_cast<int>( value )];
	}

};
