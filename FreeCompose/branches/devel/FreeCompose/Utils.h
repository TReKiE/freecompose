#pragma once

#define countof(x) ( sizeof((x)) / sizeof((x)[0]) )

//
// Ice Karma's standard lock constructs. Use like this:
//
// LOCK( someCriticalSection ) {
//     ...
//     ...
//     if ( somethingDisagreeable )
//         break;
//     ...
//     ...
// } UNLOCK( someCriticalSection );
//
// Note that you must use the _same_ critical section object in both places!
// And yes, since it's implemented with a do/while loop, you can cheerfully use
// "break" to exit a locked region!
//

#define LOCK(x) EnterCriticalSection(&(x)); do
#define UNLOCK(x) while ( false ); LeaveCriticalSection(&(x))

void debug( LPCWSTR format, ... );

CString VscToString( DWORD vsc );
CString VkToString( DWORD vk );
DWORD VkToVsc( DWORD vk );

CString GetFreeComposeFolder( void );
bool EnsureFreeComposeFolderExists( void );

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
	if ( a.u32Composed != b.u32Composed ) return false;
	return true;
}

inline bool operator!=( const COMPOSE_KEY_ENTRY& a, const COMPOSE_KEY_ENTRY& b ) {
	return ! operator==( a, b );
}

inline bool IsKeyDown( const DWORD vk ) {
	unsigned u = (unsigned short) GetKeyState( vk );
	return ( u & 0x8000U ) == 0x8000U;
}
