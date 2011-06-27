#pragma once

#include "FCShared.h"

class FCSHARED_API KeyIsXAlnum {
public:
	static bool Test( const DWORD dwVK );

private:
	static void _Initialize( void );

	static bool _map[256];
	static bool _initialized;
};
