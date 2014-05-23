#include "stdafx.h"

#include "FCShared.h"
#include "Unicode.h"

CString Utf32ToUtf16( UChar32 const* pqz, int const cch ) {
	UChar* pwzDest = nullptr;
	int cchDest = 0;

	UErrorCode errorCode = U_ZERO_ERROR;
	u_strFromUTF32( nullptr, 0, &cchDest, pqz, cch, &errorCode );
	if ( U_BUFFER_OVERFLOW_ERROR != errorCode && U_STRING_NOT_TERMINATED_WARNING != errorCode ) {
		debug( L"Utf32ToUtf16/n: u_strFromUTF32 failed, errorCode=%d\n", errorCode );
		return CString( );
	}

	int cchDestCapacity = cchDest + 1;
	pwzDest = new UChar[cchDestCapacity];
	cchDest = 0;

	errorCode = U_ZERO_ERROR;
	u_strFromUTF32( pwzDest, cchDestCapacity, &cchDest, pqz, cch, &errorCode );
	if ( U_ZERO_ERROR != errorCode && U_STRING_NOT_TERMINATED_WARNING != errorCode ) {
		debug( L"Utf32ToUtf16/n: u_strFromUTF32 failed, errorCode=%d\n", errorCode );
		delete[] pwzDest;
		return CString( );
	}

	CString tmp( pwzDest, cchDest );
	delete[] pwzDest;
	return tmp;
}

CString Utf32ToUtf16( UChar32 const wch ) {
	UChar wchDest = L'\0';
	int cchDest = 1;

	UErrorCode errorCode = U_ZERO_ERROR;
	u_strFromUTF32( &wchDest, 1, &cchDest, &wch, 1, &errorCode );
	if ( U_STRING_NOT_TERMINATED_WARNING != errorCode ) {
		debug( L"Utf32ToUtf16/1: u_strFromUTF32 failed, errorCode=%d\n", errorCode );
		return CString( );
	} else {
		return CString( wchDest );
	}
}

UChar32* Utf16ToUtf32( UChar const* pwz, int const cch, int* pcchResult ) {
	UChar32* pqzDest = nullptr;
	int cchDest = 0;
	if ( pcchResult ) {
		*pcchResult = 0;
	}

	UErrorCode errorCode = U_ZERO_ERROR;
	u_strToUTF32( nullptr, 0, &cchDest, pwz, cch, &errorCode );
	if ( U_BUFFER_OVERFLOW_ERROR != errorCode && U_STRING_NOT_TERMINATED_WARNING != errorCode ) {
		debug( L"Utf16ToUtf32: u_strToUTF32 failed, errorCode=%d\n", errorCode );
		return nullptr;
	}

	int cchDestCapacity = cchDest + 1;
	pqzDest = new UChar32[cchDestCapacity];
	cchDest = 0;

	errorCode = U_ZERO_ERROR;
	u_strToUTF32( pqzDest, cchDestCapacity, &cchDest, pwz, cch, &errorCode );
	if ( U_ZERO_ERROR != errorCode && U_STRING_NOT_TERMINATED_WARNING != errorCode ) {
		debug( L"Utf16ToUtf32: u_strToUTF32 failed, errorCode=%d\n", errorCode );
		delete[] pqzDest;
		return nullptr;
	}

	if ( pcchResult ) {
		*pcchResult = cchDest;
	}

	return pqzDest;
}

int CountCompositeCharacters( UChar const* pwz, int const cch ) {
	icu::UnicodeString unicodeString( icu::UnicodeString( pwz, cch ) );
	UErrorCode error = U_ZERO_ERROR;
	icu::BreakIterator* pbi = icu::BreakIterator::createCharacterInstance( icu::Locale::getDefault( ), error );
	pbi->setText( unicodeString );

	int cComposites = 0;
	int32_t p = pbi->first( );
	while ( icu::BreakIterator::DONE != p ) {
		cComposites++;
		p = pbi->next( );
	}

	delete pbi;
	return cComposites - 1;
}

int CountCompositeCharacters( UChar32 const* pqz, int const cch ) {
	return CountCompositeCharacters( Utf32ToUtf16( pqz, cch ), cch );
}
