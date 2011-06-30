#include "stdafx.h"
#include "TestVehicle.h"
#include "Keys.h"

void Keys( void ) {
	UINT vk;
	UINT vsc;
	HKL hkl = GetKeyboardLayout( 0 );
	int rc;

	vk = VK_OEM_3;
	vsc = MapVirtualKeyEx( vk, MAPVK_VK_TO_VSC, hkl );
	if ( 0 == vsc ) {
		fwprintf( stderr, L"Keys: MapVirtualKeyEx failed: %lu\n", GetLastError( ) );
		return;
	}
	wprintf( L"Keys: MapVirtualKeyEx returns vsc=0x%08x\n", vsc );

	BYTE keyState[256] = { 0, };
	wchar_t keyBuf[256] = { 0, };
	keyState[VK_SHIFT] = 0x80;
	rc = ToUnicodeEx( vk, vsc, keyState, keyBuf, 256, 0, hkl );
	wprintf( L"Keys: ToUnicodeEx returns: %d\n", rc );
	wprintf( L"Keys: keyBuf contains: '%s'\n", keyBuf );
}
