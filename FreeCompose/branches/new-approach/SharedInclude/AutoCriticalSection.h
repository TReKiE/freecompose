#pragma once

#include "CriticalSection.h"

class AutoCriticalSection: public CriticalSection {
public:

	inline AutoCriticalSection( ) {
		Initialize( );
	}

	inline AutoCriticalSection( DWORD const cSpin ) {
		InitializeWithSpinCount( cSpin );
	}

	inline AutoCriticalSection( DWORD const cSpin, DWORD const flags ) {
		InitializeEx( cSpin, flags );
	}

	inline AutoCriticalSection( AutoCriticalSection const& right ) {
		*this = right;
	}

	virtual inline ~AutoCriticalSection( ) {
		Delete( );
	}

	virtual inline AutoCriticalSection const& operator=( AutoCriticalSection const& right ) {
		cs = right.cs;
		return *this;
	}

private:

	inline AutoCriticalSection( CRITICAL_SECTION const& right ) {
		*this = right;
	}

	virtual inline AutoCriticalSection const& operator=( CRITICAL_SECTION const& right ) {
		cs = right;
		return *this;
	}

	virtual inline operator CRITICAL_SECTION const&( ) const {
		return cs;
	}

	virtual inline void Delete( ) {
		DeleteCriticalSection( &cs );
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

	CRITICAL_SECTION cs;

};
