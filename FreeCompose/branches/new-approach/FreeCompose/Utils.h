#pragma once

//
// Data types
//

enum EXCLUSION_KIND {
	UNIQUE_TO_SESSION,
	UNIQUE_TO_DESKTOP,
	UNIQUE_TO_TRUSTEE,
	UNIQUE_TO_SYSTEM,
};

//
// Function prototypes
//

CString VscToString( UINT vsc );
CString VkToKeyLabel( DWORD vk );
CString VkToString( DWORD vk );
UINT VkToVsc( DWORD vk );

DWORD GetComCtl32Version( void );

CString MakeInstanceExclusionName( const CString& input, const EXCLUSION_KIND kind );

//
// Inline functions
//

template<typename T> extern inline T compare_keys( void* /*context*/, const void* _elem1, const void* _elem2 ) {
	T& elem1 = *( static_cast<T*>( _elem1 ) );
	T& elem2 = *( static_cast<T*>( _elem2 ) );

	if ( elem1 < elem2 )
		return -1;
	else if ( elem1 > elem2 )
		return 1;
	else
		return 0;
}

template<typename T> extern inline T compare_keys_reverse( void* /*context*/, const void* _elem1, const void* _elem2 ) {
	T& elem1 = *( (T*) _elem1 );
	T& elem2 = *( (T*) _elem2 );

	if ( elem1 > elem2 )
		return -1;
	else if ( elem1 < elem2 )
		return 1;
	else
		return 0;
}

extern inline bool operator==( const COMPOSE_SEQUENCE& a, const COMPOSE_SEQUENCE& b ) {
	if ( a.chFirst    != b.chFirst    ) return false;
	if ( a.chSecond   != b.chSecond   ) return false;
	if ( a.chComposed != b.chComposed ) return false;
	return true;
}

extern inline bool operator!=( const COMPOSE_SEQUENCE& a, const COMPOSE_SEQUENCE& b ) {
	return ! operator==( a, b );
}

extern inline bool IsKeyDown( const DWORD vk ) {
	unsigned u = (unsigned short) GetKeyState( vk );
	return ( u & 0x8000U ) == 0x8000U;
}
