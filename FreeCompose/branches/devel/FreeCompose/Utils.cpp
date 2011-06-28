#include "stdafx.h"
#include "Utils.h"

static DWORD ComCtl32Version = 0UL;

CString VscToString( UINT vsc ) {
	wchar_t buf[256];
	int rc;

	memset( buf, 0, sizeof( buf ) );
	rc = GetKeyNameText( vsc, buf, countof( buf ) );
	if ( 0 == rc ) {
		debug( _T("VscToString(0x%08x): GetKeyNameText: error %d\n"), vsc, GetLastError( ) );
		return CString( );
	}

	return CString( buf );
}

CString VkToString( DWORD vk ) {
	UINT vsc = MapVirtualKeyEx( vk & 0x7fffffffUL, MAPVK_VK_TO_VSC, GetKeyboardLayout( 0 ) );
	if ( ! vsc ) {
		return CString( );
	}

	BYTE keyState[256] = { 0, };
	wchar_t keyBuf[8] = { 0, };

	if ( 0 != ( vk & 0x80000000 ) ) {
		keyState[VK_SHIFT] = 0x80;
	}
	ToUnicodeEx( vk, vsc, keyState, keyBuf, 256, 0, GetKeyboardLayout( 0 ) );
	return CString( keyBuf );
}

CString VkToKeyLabel( DWORD vk ) {
	CString str( VscToString( VkToVsc( vk ) ) );
	if ( 0 != ( vk & 0x80000000UL ) ) {
		str.Insert( 0, VscToString( VkToVsc( VK_SHIFT ) ) + CString( _T("+") ) );
	}
	return str;
}

UINT VkToVsc( DWORD vk ) {
	UINT vsc = MapVirtualKeyEx( vk & 0x7fffffffUL, MAPVK_VK_TO_VSC, GetKeyboardLayout( 0 ) );
	if ( 0 == vsc )
		return 0;

	return MAKELONG( 1, vsc );
}

DWORD GetComCtl32Version( void ) {
	if ( 0 != ComCtl32Version ) {
		goto out1;
	}

	HMODULE hmod = LoadLibrary( L"COMCTL32.DLL" );
	if ( NULL == hmod ) {
		debug( L"GetComCtl32Version: LoadLibrary failed: %d\n", GetLastError( ) );
		goto out1;
	}

	DLLGETVERSIONPROC dgvp = (DLLGETVERSIONPROC) GetProcAddress( hmod, "DllGetVersion" );
	if ( NULL == dgvp ) {
		debug( L"GetComCtl32Version: GetProcAddress failed: %d\n", GetLastError( ) );
		goto out2;
	}

	DLLVERSIONINFO dvi;
	HRESULT hr;
	ZeroMemory( &dvi, sizeof( dvi ) );
	dvi.cbSize = sizeof( dvi );
	hr = (*dgvp)( &dvi );
	if ( FAILED(hr) ) {
		debug( L"GetComCtl32Version: DLLGETVERSIONPROC call failed, hr=0x%08x\n", hr );
		goto out2;
	}

	ComCtl32Version = MAKELONG( dvi.dwMinorVersion, dvi.dwMajorVersion );

out2:
	FreeLibrary( hmod );
out1:
	return ComCtl32Version;
}
