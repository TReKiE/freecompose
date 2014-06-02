#pragma once

//
// Macros
//

#define PACKVERSION( majorversion, minorversion ) MAKELONG( minorversion, majorversion )

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
// Constants
//

DWORD const COMCTL32APIVER_NT4     = PACKVERSION( 4,  0 );
DWORD const COMCTL32APIVER_WIN2K   = PACKVERSION( 5, 81 );
DWORD const COMCTL32APIVER_WINXP   = PACKVERSION( 6,  0 );
DWORD const COMCTL32APIVER_WS03    = COMCTL32APIVER_WINXP;
DWORD const COMCTL32APIVER_VISTA   = PACKVERSION( 6, 10 );
DWORD const COMCTL32APIVER_WS08    = COMCTL32APIVER_VISTA;
DWORD const COMCTL32APIVER_WIN7    = COMCTL32APIVER_VISTA;
DWORD const COMCTL32APIVER_WIN8    = COMCTL32APIVER_VISTA;
DWORD const COMCTL32APIVER_WINBLUE = COMCTL32APIVER_VISTA;

#if _NTDDI_VERSION < NTDDI_VISTA
#define LVS_EX_COLUMNSNAPPOINTS 0x40000000
#else
#pragma message("Utils.h: You can remove the temporary definition here if you've upgraded the SDK target to Vista.")
#endif

//
// Globals
//

extern DWORD g_CommonControlsApiVersion;

extern unsigned g_CommonControlsDllMajorVersion;
extern unsigned g_CommonControlsDllMinorVersion;
extern unsigned g_CommonControlsDllBuildNumber;
extern unsigned g_CommonControlsDllQfeNumber;

//
// Function prototypes
//

CString VscToString( UINT vsc );
CString VkToKeyLabel( DWORD vk );
CString VkToString( DWORD vk );
UINT VkToVsc( DWORD vk );

DWORD GetComCtl32Version( void );

CString MakeInstanceExclusionName( const CString& input, const EXCLUSION_KIND kind );

CString FormatCodePoint( CString const& str );

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

template<typename T> extern inline T compare_keys_reverse( void* /*context*/, void const* _elem1, void const* _elem2 ) {
	T const elem1 = *static_cast<T const*>( _elem1 );
	T const elem2 = *static_cast<T const*>( _elem2 );

	if ( elem1 > elem2 )
		return -1;
	else if ( elem1 < elem2 )
		return 1;
	else
		return 0;
}

extern inline bool IsKeyDown( const DWORD vk ) {
	unsigned u = static_cast<unsigned short>( GetKeyState( vk ) );
	return ( u & 0x8000U ) == 0x8000U;
}

extern inline CString LoadFromStringTable( unsigned nResourceId ) {
	return CString( reinterpret_cast<LPCWSTR>( nResourceId ) );
}

template<typename Tin, typename Tout>
extern inline Tout Coerce( Tin const& in ) {
	return static_cast<Tout>( _variant_t( in ) );
}
