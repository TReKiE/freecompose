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

	virtual inline ~AutoCriticalSection( ) {
		Delete( );
	}

private:

	inline AutoCriticalSection( AutoCriticalSection const& right );
	inline AutoCriticalSection const& operator=( AutoCriticalSection const& right );

	virtual inline void Delete( ) {
		DeleteCriticalSection( &cs );
	}

	virtual inline void Initialize( ) {
		InitializeCriticalSection( &cs );
	}

	virtual inline void InitializeWithSpinCount( DWORD const cSpin ) {
		InitializeCriticalSectionAndSpinCount( &cs, cSpin ) ? true : false;
	}

	virtual inline bool InitializeEx( DWORD const cSpin, DWORD const flags ) {
		return InitializeCriticalSectionEx( &cs, cSpin, flags ) ? true : false;
	}

};
