#pragma once

class CriticalSection {
public:

	inline CriticalSection( ) {
	}

	virtual inline ~CriticalSection( ) {
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

	virtual inline void InitializeWithSpinCount( DWORD const cSpin ) {
		InitializeCriticalSectionAndSpinCount( &cs, cSpin );
	}

	virtual inline bool InitializeEx( DWORD const cSpin, DWORD const flags ) {
		return InitializeCriticalSectionEx( &cs, cSpin, flags ) ? true : false;
	}

	virtual inline void Leave( ) {
		LeaveCriticalSection( &cs );
	}

	virtual inline DWORD SetSpinCount( DWORD const cSpin ) {
		return SetCriticalSectionSpinCount( &cs, cSpin );
	}

	virtual inline bool TryEnter( ) {
		return TryEnterCriticalSection( &cs ) ? true : false;
	}

private:

	inline CriticalSection( CriticalSection const& right );
	inline CriticalSection const& operator=( CriticalSection const& right );

protected:

	CRITICAL_SECTION cs;

};
