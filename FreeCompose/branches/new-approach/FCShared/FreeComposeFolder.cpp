#include "stdafx.h"
#include "FCShared.h"
#include "Internal.h"
#include "resource.h"

#include <cstdio>

using namespace std;

FCSHARED_API bool GetFreeComposeFolder( LPWSTR& lpsz ) {
	bool ret = false;
	int rc;

	LPWSTR appDataFolder = NULL;
	if ( !_GetAppDataFolder( appDataFolder ) ) {
		return false;
	}

	wchar_t companyName[1024];
	rc = LoadString( hDllInst, AFX_IDS_COMPANY_NAME, companyName, 1024 );
	if ( !rc ) {
		goto ret;
	}

	wchar_t appName[1024];
	rc = LoadString( hDllInst, AFX_IDS_APP_TITLE, appName, 1024 );
	if ( !rc ) {
		goto ret;
	}

	wchar_t buf[1024];
	swprintf_s( buf, 1024, L"%s\\%s\\%s", appDataFolder, companyName, appName );
	lpsz = _CoTaskMemDuplicateStringW( buf );
	ret = true;

ret:
	CoTaskMemFree( appDataFolder );
	return ret;
}

FCSHARED_API bool EnsureFreeComposeFolderExists( void ) {
	bool ret = false;
	int rc;

	LPWSTR appDataFolder = NULL;
	if ( !_GetAppDataFolder( appDataFolder ) ) {
		return false;
	}

	wchar_t companyName[1024];
	rc = LoadString( hDllInst, AFX_IDS_COMPANY_NAME, companyName, 1024 );
	if ( !rc ) {
		goto ret;
	}

	wchar_t appName[1024];
	rc = LoadString( hDllInst, AFX_IDS_APP_TITLE, appName, 1024 );
	if ( !rc ) {
		goto ret;
	}

	wchar_t folder[1024];
	swprintf_s( folder, 1024, L"%s\\%s", appDataFolder, companyName );
	if ( ! CreateDirectory( folder, NULL ) && ( ERROR_ALREADY_EXISTS != GetLastError( ) ) ) {
		debug( L"EnsureFreeComposeFolderExists: CreateDirectory('%s') failed: %d\n", folder, GetLastError( ) );
		goto ret;
	}

	swprintf_s( folder, 1024, L"%s\\%s\\%s", appDataFolder, companyName, appName );
	if ( ! CreateDirectory( folder, NULL ) && ( ERROR_ALREADY_EXISTS != GetLastError( ) ) ) {
		debug( L"EnsureFreeComposeFolderExists: CreateDirectory('%s') failed: %d\n", folder, GetLastError( ) );
		goto ret;
	}

	ret = true;

ret:
	CoTaskMemFree( appDataFolder );
	return ret;
}
