#pragma once

#include <unicode/uchar.h>

//==============================================================================
// Constants
//==============================================================================

wchar_t  const LeadSurrogateBase      = 0xD800;
wchar_t  const LeadSurrogateEnd       = 0xDC00;
wchar_t  const TrailSurrogateBase     = 0xDC00;
wchar_t  const TrailSurrogateEnd      = 0xE000;
unsigned const SupplementalPlanesBase = 0x10000;
unsigned const CodePointSpaceEnd      = 0x110000;
wchar_t  const SurrogatePayloadMask   = 0x03FF;
wchar_t  const SurrogateMask          = static_cast<wchar_t>( ~SurrogatePayloadMask );

//==============================================================================
// Inline functions
//==============================================================================

template<typename T>
inline bool const IsSurrogate( T const ch ) {
	return ( U_SURROGATE == u_charType( ch ) );
}

template<typename T>
inline bool const IsLeadSurrogate( T const ch ) {
	return ( UBLOCK_HIGH_SURROGATES == ublock_getCode( ch ) );
}

template<typename T>
inline bool const IsTrailSurrogate( T const ch ) {
	return ( UBLOCK_LOW_SURROGATES == ublock_getCode( ch ) );
}



inline wchar_t MakeFirstSurrogate( unsigned const ch ) {
	return static_cast<wchar_t>( LeadSurrogateBase + ( ( ( ch - SupplementalPlanesBase ) >> 10 ) & SurrogatePayloadMask ) );
}

inline wchar_t MakeSecondSurrogate( unsigned const ch ) {
	return static_cast<wchar_t>( TrailSurrogateBase + ( ( ch - SupplementalPlanesBase ) & SurrogatePayloadMask ) );
}


#ifdef __AFXSTR_H__
inline CString Utf32ToUtf16( unsigned const ch ) {
	if ( ch < SupplementalPlanesBase ) {
		return CString( static_cast<wchar_t>( ch ) );
	} else {
		wchar_t tmp[2] = { MakeFirstSurrogate( ch ), MakeSecondSurrogate( ch ) };
		return CString( tmp, 2 );
	}
}

inline bool Utf16ToUtf32( CString const& s, unsigned& ch ) {
	wchar_t w1, w2;

	switch ( s.GetLength( ) ) {
		case 0:
			return false;

		case 1:
			if ( IsSurrogate( s[0] ) ) {
				return false;
			}
			ch = static_cast<unsigned>( s[0] );
			return true;

		default:
			w1 = s[0];
			w2 = s[1];
			if ( !IsLeadSurrogate( w1 ) || !IsTrailSurrogate( w2 ) ) {
				return false;
			}
			ch = SupplementalPlanesBase + ( ( ( static_cast<unsigned>( w1 ) & SurrogatePayloadMask ) << 10 ) | ( static_cast<unsigned>( w2 ) & SurrogatePayloadMask ) );
			return true;
	}
}
#endif
