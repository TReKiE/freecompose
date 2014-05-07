#include "stdafx.h"
#include "FCShared.h"
#include "Internal.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cerrno>

#include <share.h>

#ifndef _DEBUG
FCSHARED_API void InitializeDebugLogFile( void ) {
	// do nothing
}

FCSHARED_API void TerminateDebugLogFile( void ) {
	// do nothing
}
#else
static FILE* debugFile = nullptr;

FCSHARED_API void InitializeDebugLogFile( void ) {
	if ( ! EnsureFreeComposeFolderExists( ) ) {
		debug( L"InitializeDebug: Can't make sure our AppData folder exists\n" );
		return;
	}

	LPWSTR lpsz;
	if ( ! GetFreeComposeFolder( lpsz ) ) {
		debug( L"InitializeDebug: can't get name of our AppData folder\n" );
		return;
	}

	wchar_t buf[1024];
	swprintf_s( buf, 1024, L"%s\\debug.log", lpsz );
	CoTaskMemFree( lpsz );

	errno = 0;
	debugFile = _wfsopen( buf, L"at+,ccs=UTF-16LE", _SH_DENYNO );
	if ( nullptr == debugFile ) {
		debug( L"InitializeDebug: _wfsopen('%s') failed: %d\n", buf, errno );
		return;
	}
	setvbuf( debugFile, nullptr, _IONBF, 0 );

	wchar_t wcstr[256];
	time_t timenow = time( nullptr );
	tm tmnow;
	if ( 0 == localtime_s( &tmnow, &timenow ) ) {
		wcsftime( wcstr, _countof( wcstr ), L"========  Log opened at %Y-%m-%d %H:%M:%S  ========\n", &tmnow );
		fputws( wcstr, debugFile );
	}
}

FCSHARED_API void TerminateDebugLogFile( void ) {
	if ( debugFile ) {
		fclose( debugFile );
		debugFile = nullptr;
	}
}

FCSHARED_API void debug( _In_z_ _Printf_format_string_ LPCWSTR format, ... ) {
	wchar_t buf[1024];
	va_list va;

	va_start( va, format );
	vswprintf_s( buf, 1024, format, va );
	va_end( va );

	OutputDebugString( buf );
	if ( debugFile ) {
		fputws( buf, debugFile );
	}
}
#endif
