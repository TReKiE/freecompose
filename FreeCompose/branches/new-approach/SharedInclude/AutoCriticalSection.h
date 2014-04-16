#pragma once

class AutoCriticalSection {
public:

	inline AutoCriticalSection( ) {
		InitializeCriticalSection( &cs );
	}

	inline AutoCriticalSection( DWORD const cSpin ) {
		InitializeCriticalSectionAndSpinCount( &cs, cSpin );
	}

	inline ~AutoCriticalSection( ) {
		DeleteCriticalSection( &cs );
	}

	inline void Enter( ) {
		EnterCriticalSection( &cs );
	}

	inline void Leave( ) {
		LeaveCriticalSection( &cs );
	}

	inline DWORD SetSpinCount( DWORD const cSpin ) {
		return SetCriticalSectionSpinCount( &cs, cSpin );
	}

	inline bool TryEnter( ) {
		return TryEnterCriticalSection( &cs ) ? true : false;
	}

private:

	AutoCriticalSection( AutoCriticalSection const& right );
	AutoCriticalSection const& operator=( AutoCriticalSection const& right );

	CRITICAL_SECTION cs;

};
