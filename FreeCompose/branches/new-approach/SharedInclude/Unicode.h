#pragma once

#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/brkiter.h>

//==============================================================================
// Inline functions
//==============================================================================

inline FCSHARED_API bool const IsSurrogate( UChar32 const ch ) {
	return ( U_SURROGATE == u_charType( ch ) );
}

inline FCSHARED_API bool const IsLeadSurrogate( UChar32 const ch ) {
	return ( UBLOCK_HIGH_SURROGATES == ublock_getCode( ch ) );
}

inline FCSHARED_API bool const IsTrailSurrogate( UChar32 const ch ) {
	return ( UBLOCK_LOW_SURROGATES == ublock_getCode( ch ) );
}

FCSHARED_API CString Utf32ToUtf16( UChar32 const* pqz, int const cch = -1 );
FCSHARED_API CString Utf32ToUtf16( UChar32 const uch );
FCSHARED_API UChar32* Utf16ToUtf32( UChar const* pwz, int const cch, int& cchResult );

FCSHARED_API int CountCompositeCharacters( UChar const* pwz, int const cch = -1 );
FCSHARED_API int CountCompositeCharacters( UChar32 const* pqz, int const cch = -1 );
