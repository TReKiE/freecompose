#pragma once

void debug( LPCTSTR lpsz, ... );
bool IsCapsLock( void );
CString VkToString( DWORD dw );

template<typename T> inline T compare_keys( void* /*context*/, const void* _elem1, const void* _elem2 ) {
	T elem1 = *( (T*) _elem1 );
	T elem2 = *( (T*) _elem2 );

	if ( elem1 < elem2 )
		return -1;
	else if ( elem1 > elem2 )
		return 1;
	else
		return 0;
}

template<typename T> inline T compare_keys_reverse( void* /*context*/, const void* _elem1, const void* _elem2 ) {
	T elem1 = *( (T*) _elem1 );
	T elem2 = *( (T*) _elem2 );

	if ( elem1 > elem2 )
		return -1;
	else if ( elem1 < elem2 )
		return 1;
	else
		return 0;
}

inline bool operator==( const COMPOSE_KEY_ENTRY& a, const COMPOSE_KEY_ENTRY& b ) {
	if ( a.vkFirst     != b.vkFirst     ) return false;
	if ( a.vkSecond    != b.vkSecond    ) return false;
	if ( a.wchComposed != b.wchComposed ) return false;
	return true;
}

inline bool operator!=( const COMPOSE_KEY_ENTRY& a, const COMPOSE_KEY_ENTRY& b ) {
	return ! operator==( a, b );
}
