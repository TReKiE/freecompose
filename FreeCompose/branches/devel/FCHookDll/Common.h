#pragma once
#pragma warning( disable: 4127 )

#define countof(x) ( sizeof((x)) / sizeof((x)[0]) )

#define LOCK(x) EnterCriticalSection(&(x)); do
#define UNLOCK(x) while ( false ); LeaveCriticalSection(&(x))

#pragma data_seg( push, ".shareddata" )
__declspec(selectany) CRITICAL_SECTION      cs;
__declspec(selectany) HHOOK                 hHook              = NULL;

__declspec(selectany) COMPOSE_KEY_ENTRY*    ComposeKeyEntries  = NULL;
__declspec(selectany) int                   cComposeKeyEntries = 0;

__declspec(selectany) CAPS_LOCK_TOGGLE_MODE clToggleMode       = CLTM_NORMAL;
__declspec(selectany) CAPS_LOCK_SWAP_MODE   clSwapMode         = CLSM_SWAP;
__declspec(selectany) bool                  fSwapCapsLock      = false;

__declspec(selectany) DWORD                 vkCompose          = VK_APPS;
__declspec(selectany) DWORD                 vkCapsLockSwap     = VK_CAPITAL;
#pragma data_seg( pop )

void debug( LPCWSTR lpszMessage, ... );
int CompareCkes( const void*, const void* );

class ByteSet {
public:
	inline ByteSet( ) {
		Clear( );
		count = 0;
	}

	inline void Clear   ( void )                { for ( int n = 0; n < countof( bits ); n++ ) bits[n] = 0;                   }
	inline void Add     ( const DWORD x )       { if ( ! Contains( x ) ) { count++; bits[ x >> 3 ] |=  ( 1 << ( x & 7 ) ); } }
	inline void Remove  ( const DWORD x )       { if (   Contains( x ) ) { count--; bits[ x >> 3 ] &= ~( 1 << ( x & 7 ) ); } }
	inline bool Contains( const DWORD x ) const { return ( bits[ x >> 3 ] & ( 1 << ( x & 7 ) ) ) != 0;                       }
	inline bool IsEmpty ( void )          const { return count == 0;                                                         }

private:
	unsigned char bits[32];
	int count;
};
