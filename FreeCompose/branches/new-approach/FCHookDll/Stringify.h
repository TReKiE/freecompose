#pragma once

//==============================================================================
// Constants
//==============================================================================

extern wchar_t const* stringsFor_bool[];
extern wchar_t const* stringsFor_enum_DISPOSITION[];
extern wchar_t const* stringsFor_enum_COMPOSE_STATE[];

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

	static wchar_t const* from_COMPOSE_STATE( COMPOSE_STATE const value ) {
		return stringsFor_enum_COMPOSE_STATE[static_cast<int>( value )];
	}

};
