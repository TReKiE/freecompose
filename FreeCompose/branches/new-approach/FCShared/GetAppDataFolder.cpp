#include "stdafx.h"
#include "FCShared.h"
#include "Internal.h"

#include <ShlObj.h>

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

bool _GetAppDataFolderFromShell( LPWSTR& lpsz ) {
	wchar_t buf[MAX_PATH];

	if ( ! SHGetSpecialFolderPath( nullptr, buf, CSIDL_APPDATA, FALSE ) ) {
		debug( L"SHGetSpecialFolderPath failed: %lu\n", GetLastError( ) );
		return false;
	}

	lpsz = _CoTaskMemDuplicateString( buf );
	return true;
}

bool _GetAppDataFolder( LPWSTR& lpsz ) {
	if ( ! _GetAppDataFolderFromKfm( lpsz ) ) {
		if ( ! _GetAppDataFolderFromShell( lpsz ) ) {
			return false;
		}
	}
	return true;
}
