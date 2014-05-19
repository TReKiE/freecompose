#include "stdafx.h"
#include "FCShared.h"
#include "Internal.h"
#include "resource.h"

FCSHARED_API bool GetFreeComposeFolder( LPWSTR& lpsz ) {
	bool ret = false;

	LPWSTR appDataFolder = nullptr;
	if ( !_GetAppDataFolder( appDataFolder ) ) {
		return false;
	}

	CString strCompanyName( reinterpret_cast<LPCWSTR>( AFX_IDS_COMPANY_NAME ) );
	CString strAppName( reinterpret_cast<LPCWSTR>( AFX_IDS_APP_TITLE ) );
	CString path;
	path.Format( L"%s\\%s\\%s", appDataFolder, strCompanyName, strAppName );
	lpsz = _CoTaskMemDuplicateString( path );
	ret = true;

	CoTaskMemFree( appDataFolder );
	return ret;
}

FCSHARED_API bool EnsureFreeComposeFolderExists( void ) {
	DWORD dwError;
	BOOL rc;
	bool ret = false;

	LPWSTR appDataFolder = nullptr;
	if ( !_GetAppDataFolder( appDataFolder ) ) {
		return false;
	}

	CString strCompanyName( reinterpret_cast<LPCWSTR>( AFX_IDS_COMPANY_NAME ) );
	CString strAppName( reinterpret_cast<LPCWSTR>( AFX_IDS_APP_TITLE ) );
	CString folder;
	folder.Format( L"%s\\%s", appDataFolder, strCompanyName );

	SetLastError( 0 );
	rc = CreateDirectory( folder, nullptr );
	dwError = GetLastError( );
	if ( !rc && ( ERROR_ALREADY_EXISTS != dwError ) ) {
		debug( L"EnsureFreeComposeFolderExists: CreateDirectory('%s') failed: %lu\n", static_cast<LPCWSTR>( folder ), dwError );
		goto ret;
	}

	folder.AppendFormat( L"\\%s", strAppName );

	SetLastError( 0 );
	rc = CreateDirectory( folder, nullptr );
	dwError = GetLastError( );
	if ( !rc && ( ERROR_ALREADY_EXISTS != dwError ) ) {
		debug( L"EnsureFreeComposeFolderExists: CreateDirectory('%s') failed: %lu\n", static_cast<LPCWSTR>( folder ), dwError );
		goto ret;
	}

	ret = true;

ret:
	CoTaskMemFree( appDataFolder );
	return ret;
}
