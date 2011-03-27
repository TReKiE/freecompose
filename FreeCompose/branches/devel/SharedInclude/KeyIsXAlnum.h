#pragma once

#ifdef FCSHARED_EXPORTS
#define FCSHARED_API __declspec(dllexport)
#else
#define FCSHARED_API __declspec(dllimport)
#endif

class FCSHARED_API KeyIsXAlnum {
public:
	static bool Test( const DWORD dwVK );

private:
	static void _Initialize( void );

	static bool _map[256];
	static bool _initialized;
};
