#include "stdafx.h"
#include "KeyIsXAlnum.h"

#pragma data_seg( push, ".shareddata" )
bool KeyIsXAlnum::_map[256];
bool KeyIsXAlnum::_initialized = false;
#pragma data_seg( pop )

void KeyIsXAlnum::_Initialize( void ) {
	int n;

	for ( n = 0;        n < 256;       n++ ) _map[n] = false;
	for ( n = '0';      n <= '9';      n++ ) _map[n] = true;
	for ( n = 'A';      n <= 'Z';      n++ ) _map[n] = true;
	for ( n = VK_OEM_1; n <= VK_OEM_3; n++ ) _map[n] = true;
	for ( n = VK_OEM_4; n <= VK_OEM_7; n++ ) _map[n] = true;
	
	_map[VK_OEM_102] = true;

	_initialized = true;
}

bool KeyIsXAlnum::Test( const DWORD vk ) {
	if ( ! _initialized )
		_Initialize( );

	if ( vk < 256 )
		return _map[vk];
	return false;
}
