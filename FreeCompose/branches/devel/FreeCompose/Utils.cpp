#include "stdafx.h"
#include "Utils.h"

void debug( LPCTSTR lpsz, ... ) {
	TCHAR szbuf[1024];
	va_list va;

	va_start( va, lpsz );
	_vsntprintf_s( szbuf, 1024, _TRUNCATE, lpsz, va );
	va_end( va );
	OutputDebugString( szbuf );
}

CString VscToString( DWORD vsc ) {
	TCHAR buf[256];
	int rc;

	ATLASSERT( 0 != LOWORD( vsc ) );
	ATLASSERT( 0 != HIWORD( vsc ) );
	ATLASSERT( 0 == ( vsc & 0x80000000 ) );

	memset( buf, 0, sizeof( buf ) );
	debug( _T("VscToString: 1st 0x%08x\n"), vsc );
	vsc |= 1 << 25;
	debug( _T("VscToString: 2nd 0x%08x\n"), vsc );
	rc = GetKeyNameText( vsc, buf, countof( buf ) );
	if ( 0 == rc ) {
		debug( _T("VscToString(0x%08x): GetKeyNameText: error %d\n"), vsc, GetLastError( ) );
		return CString( );
	}

	return CString( buf );
}

CString VkToString( DWORD vk ) {
	CString str( VscToString( VkToVsc( vk ) ) );
	if ( 0 != ( vk & 0x80000000UL ) ) {
		str.Insert( 0, VscToString( VkToVsc( VK_LSHIFT ) ) + CString( _T("+") ) );
	}
	return str;
}

DWORD VkToVsc( DWORD vk ) {
	return MAKELONG( 1, MapVirtualKeyEx( vk & 0x7fffffffUL, MAPVK_VK_TO_VSC, GetKeyboardLayout( 0 ) ) );
}
