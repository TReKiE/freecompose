#include "stdafx.h"

#include "FCShared.h"
#include "Internal.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cerrno>

#include <share.h>

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

#ifdef _DEBUG

static FILE* debugLogFile = nullptr;

FCSHARED_API void InitializeDebugLogFile( void ) {
	if ( !EnsureFreeComposeFolderExists( ) ) {
		debug( L"InitializeDebug: Can't make sure our AppData folder exists\n" );
		return;
	}

	CString strPath = GetFreeComposeFolder( );
	if ( strPath.IsEmpty( ) ) {
		debug( L"InitializeDebug: can't get name of our AppData folder\n" );
		return;
	}
	strPath.Append( L"\\debug.log" );

	errno = 0;
	debugLogFile = _wfsopen( strPath, L"at+,ccs=UTF-16LE", _SH_DENYNO );
	if ( nullptr == debugLogFile ) {
		debug( L"InitializeDebug: _wfsopen('%s') failed: %d\n", static_cast<LPCWSTR>( strPath ), errno );
		return;
	}
	setvbuf( debugLogFile, nullptr, _IONBF, 0 );

	wchar_t wcstr[256];
	time_t timenow = time( nullptr );
	tm tmnow;
	if ( 0 == localtime_s( &tmnow, &timenow ) ) {
		wcsftime( wcstr, _countof( wcstr ), L"========  Log opened at %Y-%m-%d %H:%M:%S  ========\n", &tmnow );
		fputws( wcstr, debugLogFile );
	}
}

FCSHARED_API void TerminateDebugLogFile( void ) {
	if ( debugLogFile ) {
		fclose( debugLogFile );
		debugLogFile = nullptr;
	}
}

void _log( LPCWSTR pwz ) {
	OutputDebugString( pwz );
	if ( debugLogFile ) {
		fputws( pwz, debugLogFile );
	}
}

FCSHARED_API void debug( _In_z_ _Printf_format_string_ LPCWSTR format, ... ) {
	wchar_t buf[4096];
	va_list va;

	va_start( va, format );
	vswprintf_s( buf, _countof( buf ), format, va );
	va_end( va );
	_log( buf );
}

#ifdef _NOISY_DEBUG
FCSHARED_API void noisydebug( _In_z_ _Printf_format_string_ LPCWSTR format, ... ) {
	wchar_t buf[4096];
	va_list va;

	va_start( va, format );
	vswprintf_s( buf, _countof( buf ), format, va );
	va_end( va );
	_log( buf );
}
#endif

#endif
