#pragma once

class AutoCriticalSection {
public:

	inline AutoCriticalSection( ) {
		__try {
			InitializeCriticalSection( &cs );
		}
		__except ( GetExceptionCode() == STATUS_NO_MEMORY ) {
			debug( L"AutoCriticalSection::`ctor(): STATUS_NO_MEMORY!\n" );
			DebugBreak( );
		}
	}

	inline AutoCriticalSection( DWORD const cSpin ) {
		SetLastError( ERROR_SUCCESS );
		if ( !InitializeCriticalSectionAndSpinCount( &cs, cSpin ) ) {
			DWORD dwError = GetLastError( );
			debug( L"AutoCriticalSection::`ctor(cSpin=%lu): InitializeCriticalSectionAndSpinCount failed: error=%lu\n", cSpin, dwError );
			DebugBreak( );
		}
	}

	inline ~AutoCriticalSection( ) {
		DeleteCriticalSection( &cs );
	}

	_Acquires_lock_(this->cs) inline void Enter( ) {
		EnterCriticalSection( &cs );
	}

	_Releases_lock_(this->cs) inline void Leave( ) {
		LeaveCriticalSection( &cs );
	}

	inline DWORD SetSpinCount( DWORD const cSpin ) {
		return SetCriticalSectionSpinCount( &cs, cSpin );
	}

#pragma warning(push)
#pragma warning(disable: 26116)
	_Acquires_lock_(this->cs) inline bool TryEnter( ) {
		return TryEnterCriticalSection( &cs ) ? true : false;
	}
#pragma warning(pop)

private:

	AutoCriticalSection( AutoCriticalSection const& right );
	AutoCriticalSection const& operator=( AutoCriticalSection const& right );

	CRITICAL_SECTION cs;

};
