#include "stdafx.h"
#include "FCShared.h"
#include "Internal.h"

#include <ShlObj.h>

bool _GetAppDataFolderFromShell( LPWSTR& lpsz ) {
	wchar_t buf[MAX_PATH];

	if ( ! SHGetSpecialFolderPath( NULL, buf, CSIDL_APPDATA, FALSE ) ) {
		debug( L"SHGetSpecialFolderPath failed: %d\n", GetLastError( ) );
		return false;
	}

	lpsz = _CoTaskMemDuplicateStringW( buf );
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
