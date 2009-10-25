#pragma once
#pragma warning( disable: 4127 )

#define countof(x) ( sizeof((x)) / sizeof((x)[0]) )

#define LOCK(x) EnterCriticalSection(&(x)); do
#define UNLOCK(x) while ( false ); LeaveCriticalSection(&(x))

#pragma data_seg( push, ".shareddata" )
__declspec(selectany) CRITICAL_SECTION   cs;
__declspec(selectany) COMPOSE_KEY_ENTRY* ComposeKeyEntries  = NULL;
__declspec(selectany) int                cComposeKeyEntries = 0;
__declspec(selectany) HHOOK              hHook              = NULL;
__declspec(selectany) volatile bool      fDisableCapsLock   = false;
#pragma data_seg( pop )

void debug( LPCTSTR lpszMessage, ... );
int CompareCkes( const void*, const void* );


class ByteSet {
public:
	ByteSet( ) {
		Clear( );
		count = 0;
	}

	inline void Clear   ( void )                { for ( int n = 0; n < countof( bits ); n++ ) bits[n] = 0;               }
	inline void Add     ( const DWORD x )       { if ( ! Contains( x ) ) count++; bits[ x >> 3 ] |=  ( 1 << ( x & 7 ) ); }
	inline void Remove  ( const DWORD x )       { if (   Contains( x ) ) count--; bits[ x >> 3 ] &= ~( 1 << ( x & 7 ) ); }
	inline bool Contains( const DWORD x ) const { return ( bits[ x >> 3 ] & ( 1 << ( x & 7 ) ) ) != 0;                   }
	inline bool IsEmpty ( void )          const { return count == 0;                                                     }

private:
	unsigned char bits[32];
	int count;
};
