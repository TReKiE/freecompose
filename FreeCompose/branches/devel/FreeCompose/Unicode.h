#pragma once

inline bool IsSurrogate( const wchar_t ch ) {
	return ( ch >= 0xD800 ) && ( ch < 0xE000 );
}

inline bool IsFirstSurrogate( const wchar_t ch ) {
	return ( ch >= 0xD800 ) && ( ch < 0xDC00 );
}

inline bool IsSecondSurrogate( const wchar_t ch ) {
	return ( ch >= 0xDC00 ) && ( ch < 0xE000 );
}

inline wchar_t MakeFirstSurrogate( unsigned ch ) {
	return (wchar_t) ( 0xD800 + ( ( ch - 0x10000 ) >> 10 ) );
}

inline wchar_t MakeSecondSurrogate( unsigned ch ) {
	return (wchar_t) ( 0xDC00 + ( ( ch - 0x10000 ) & 0x3FF ) );
}

inline bool Utf32ToUtf16( unsigned ch, CString& s ) {
	s.Empty( );
	if ( ch >= 0x10000 ) {
		s.Insert( 0, MakeFirstSurrogate( ch ) );
		s.Insert( 1, MakeSecondSurrogate( ch ) );
	} else {
		s.Insert( 0, (wchar_t) ch );
	}
	return true;
}

inline bool Utf16ToUtf32( CString& s, unsigned& ch ) {
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
