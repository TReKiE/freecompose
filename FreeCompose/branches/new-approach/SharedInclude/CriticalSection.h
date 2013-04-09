#pragma once

class CriticalSection {
public:

	inline CriticalSection( ) {
	}

	inline CriticalSection( CriticalSection const& right ) {
		*this = right;
	}

	inline CriticalSection( CRITICAL_SECTION const& right ) {
		*this = right;
	}

	virtual inline ~CriticalSection( ) {
	}

	virtual inline CriticalSection const& operator=( CriticalSection const& right ) {
		cs = right.cs;
		return *this;
	}

	virtual inline CriticalSection const& operator=( CRITICAL_SECTION const& right ) {
		cs = right;
		return *this;
	}

	virtual inline operator CRITICAL_SECTION const&( ) const {
		return cs;
	}

	virtual inline void Delete( ) {
		DeleteCriticalSection( &cs );
	}

	virtual inline void Enter( ) {
		EnterCriticalSection( &cs );
	}

	virtual inline void Initialize( ) {
		InitializeCriticalSection( &cs );
	}

	virtual inline bool InitializeWithSpinCount( DWORD const cSpin ) {
		return !!InitializeCriticalSectionAndSpinCount( &cs, cSpin );
	}

	virtual inline bool InitializeEx( DWORD const cSpin, DWORD const flags ) {
		return !!InitializeCriticalSectionEx( &cs, cSpin, flags );
	}

	virtual inline void Leave( ) {
		LeaveCriticalSection( &cs );
	}

	virtual inline DWORD SetSpinCount( DWORD const cSpin ) {
		return SetCriticalSectionSpinCount( &cs, cSpin );
	}

	virtual inline bool TryEnter( ) {
		return !!TryEnterCriticalSection( &cs );
	}

private:

	CRITICAL_SECTION cs;

};
