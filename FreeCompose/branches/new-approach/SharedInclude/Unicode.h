#pragma once

inline bool IsSurrogate( wchar_t const ch ) {
	return ( ch >= 0xD800 ) && ( ch < 0xE000 );
}

inline bool IsSurrogate( unsigned const ch ) {
	return ( ch >= 0xD800U ) && ( ch < 0xE000U );
}


inline bool IsFirstSurrogate( wchar_t const ch ) {
	return ( ch >= 0xD800 ) && ( ch < 0xDC00 );
}

inline bool IsFirstSurrogate( unsigned const ch ) {
	return ( ch >= 0xD800U ) && ( ch < 0xDC00U );
}


inline bool IsSecondSurrogate( wchar_t const ch ) {
	return ( ch >= 0xDC00 ) && ( ch < 0xE000 );
}

inline bool IsSecondSurrogate( unsigned const ch ) {
	return ( ch >= 0xDC00U ) && ( ch < 0xE000U );
}


inline wchar_t MakeFirstSurrogate( unsigned const ch ) {
	return (wchar_t) ( 0xD800 + ( ( ch - 0x10000 ) >> 10 ) );
}

inline wchar_t MakeSecondSurrogate( unsigned const ch ) {
	return (wchar_t) ( 0xDC00 + ( ( ch - 0x10000 ) & 0x3FF ) );
}

#ifdef __AFXSTR_H__

inline CString Utf32ToUtf16( unsigned const ch ) {
	if ( ch >= 0x10000 ) {
		wchar_t tmp[3] = {
			MakeFirstSurrogate( ch ),
			MakeSecondSurrogate( ch ),
			0
		};
		return CString( tmp );
	} else {
		return CString( (wchar_t) ch );
	}
}

inline bool Utf16ToUtf32( CString const& s, unsigned& ch ) {
	wchar_t w1, w2;

	switch ( s.GetLength( ) ) {
		case 1:
			if ( IsSurrogate( s[0] ) ) {
				return false;
			}
			ch = (unsigned) s[0];
			return true;

		case 2:
			w1 = s[0];
			w2 = s[1];
			if ( ! IsFirstSurrogate( w1 ) || ! IsSecondSurrogate( w2 ) ) {
				return false;
			}
			ch = 0x10000 | ( ( (unsigned) w1 & 0x3FF ) << 10 ) | ( (unsigned) w2 & 0x3FF );
			return true;
	}

	return false;
}

#endif
