#pragma once

//==============================================================================
// Functions
//==============================================================================

namespace Stringify {

	//==============================================================================
	// Constants
	//==============================================================================

	extern wchar_t const* stringsFor_bool[];

	static inline wchar_t const* from_bool( bool const value ) {
		return stringsFor_bool[static_cast<int>( value )];
	}

};
