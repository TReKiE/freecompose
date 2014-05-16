#pragma once

#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/brkiter.h>

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


#ifdef __AFXSTR_H__
inline CString Utf32ToUtf16( UChar32 const* pqz, int const cch = -1 ) {
	UErrorCode errorCode = U_ZERO_ERROR;

	UChar* pwzDest = nullptr;
	int cchDest = 0;

	u_strFromUTF32( nullptr, 0, &cchDest, pqz, cch, &errorCode );
	if ( U_ZERO_ERROR != errorCode ) {
		debug( L"Utf32ToUtf16: u_strFromUTF32 failed, errorCode=%d\n", errorCode );
		return CString( );
	}

	int cchDestCapacity = cchDest;
	pwzDest = new UChar[cchDestCapacity];
	cchDest = 0;

	u_strFromUTF32( pwzDest, cchDestCapacity, &cchDest, pqz, cch, &errorCode );

	if ( U_ZERO_ERROR != errorCode ) {
		debug( L"Utf32ToUtf16/n: u_strFromUTF32 failed, errorCode=%d\n", errorCode );
		delete[] pwzDest;
		return CString( );
	}

	CString tmp( pwzDest, cchDest );
	delete[] pwzDest;
	return tmp;
}

inline CString Utf32ToUtf16( UChar32 const uch ) {
	UErrorCode errorCode = U_ZERO_ERROR;

	UChar wchDest = L'\0';
	int cchDest = 1;

	u_strFromUTF32( &wchDest, 1, &cchDest, &uch, 1, &errorCode );
	if ( U_ZERO_ERROR != errorCode ) {
		debug( L"Utf32ToUtf16/1: u_strFromUTF32 failed, errorCode=%d\n", errorCode );
		return CString( );
	} else {
		return CString( wchDest );
	}
}

inline UChar32* Utf16ToUtf32( UChar const* pwz, int const cch = -1 ) {
	UErrorCode errorCode = U_ZERO_ERROR;

	UChar32* pqzDest = nullptr;
	int cchDest = 0;

	u_strToUTF32( nullptr, 0, &cchDest, pwz, cch, &errorCode );
	if ( U_ZERO_ERROR != errorCode ) {
		debug( L"Utf16ToUtf32: u_strToUTF32 failed, errorCode=%d\n", errorCode );
		return nullptr;
	}

	int cchDestCapacity = cchDest;
	pqzDest = new UChar32[cchDestCapacity];
	cchDest = 0;

	u_strToUTF32( pqzDest, cchDestCapacity, &cchDest, pwz, cch, &errorCode );

	if ( U_ZERO_ERROR != errorCode ) {
		debug( L"Utf32ToUtf16: u_strToUTF32 failed, errorCode=%d\n", errorCode );
		delete[] pqzDest;
		return nullptr;
	}

	return pqzDest;
}
#endif
