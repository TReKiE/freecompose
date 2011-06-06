#include "stdafx.h"
#include "Utils.h"

#include <errno.h>
#include <share.h>

#include <ShlObj.h>
#include <ShObjIdl.h>
#include <KnownFolders.h>

extern bool _GetAppDataFolderFromKfm( CString& str );
static bool _GetAppDataFolderFromShell( CString& str );

#ifdef NDEBUG
void InitializeDebug( void ) {
	// do nothing
}

void TerminateDebug( void ) {
	// do nothing
}

void debug( LPCWSTR /*format*/, ... ) {
	// do nothing
}
#else
static FILE* debugFile = NULL;

void InitializeDebug( void ) {
	if ( ! EnsureFreeComposeFolderExists( ) ) {
		debug( L"InitializeDebug: Can't make sure our AppData folder exists\n" );
		return;
	}

	CString str;
	if ( ! GetFreeComposeFolder( str ) ) {
		debug( L"InitializeDebug: Can't get path to our AppData folder\n" );
		return;
	}

	str.Append( L"\\debug.log" );
	errno = 0;
	debugFile = _wfsopen( str, L"at+,ccs=UTF-16LE", _SH_DENYNO );
	if ( NULL == debugFile ) {
		debug( L"InitializeDebug: _wfsopen('%s') failed: %d\n", str, errno );
		return;
	}

	wchar_t wcstr[256];
	time_t timenow = time( NULL );
	tm* ptmnow = localtime( &timenow );
	wcsftime( wcstr, 1024, L"========  Log opened at %Y-%m-%d %H:%M:%S  ========\n", ptmnow );

	setvbuf( debugFile, NULL, _IONBF, 0 );
	fputws( wcstr, debugFile );
}

void TerminateDebug( void ) {
	if ( debugFile ) {
		fclose( debugFile );
		debugFile = NULL;
	}
}

void debug( LPCWSTR format, ... ) {
	CString buf;
	va_list va;

	va_start( va, format );
	buf.FormatV( format, va );
	va_end( va );

	OutputDebugString( buf );
	if ( debugFile ) {
		fputws( buf, debugFile );
	}
}
#endif

CString VscToString( DWORD vsc ) {
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
	CString str( VscToString( VkToVsc( vk ) ) );
	if ( 0 != ( vk & 0x80000000UL ) ) {
		str.Insert( 0, VscToString( VkToVsc( VK_SHIFT ) ) + CString( _T("+") ) );
	}
	return str;
}

DWORD VkToVsc( DWORD vk ) {
	UINT vsc = MapVirtualKeyEx( vk & 0x7fffffffUL, MAPVK_VK_TO_VSC, GetKeyboardLayout( 0 ) );
	if ( 0 == vsc )
		return 0;

	return MAKELONG( 1, vsc );
}

static bool _GetAppDataFolderFromShell( CString& str ) {
	wchar_t buf[MAX_PATH];

	if ( ! SHGetSpecialFolderPath( NULL, buf, CSIDL_APPDATA, FALSE ) ) {
		debug( L"SHGetSpecialFolderPath failed: %d\n", GetLastError( ) );
		return false;
	}

	str = buf;
	return true;
}

bool GetAppDataFolder( OUT CString& str ) {
	if ( ! _GetAppDataFolderFromKfm ( str ) ) {
		if ( ! _GetAppDataFolderFromShell( str ) ) {
			return false;
		}
	}
	return true;
}

bool GetFreeComposeFolder( OUT CString& str ) {
	CString appDataFolder;
	if ( ! GetAppDataFolder( appDataFolder ) ) {
		return false;
	}

	CString companyName ( reinterpret_cast<LPCWSTR>( AFX_IDS_COMPANY_NAME ) );
	CString appName     ( reinterpret_cast<LPCWSTR>( AFX_IDS_APP_TITLE ) );
	if ( companyName.IsEmpty( ) || appName.IsEmpty( ) ) {
		return false;
	}

	str.Format( L"%s\\%s\\%s", appDataFolder, companyName, appName );
	return true;
}

bool EnsureFreeComposeFolderExists( void ) {
	CString folder;
	if ( ! GetAppDataFolder( folder ) ) {
		return false;
	}

	CString companyName ( reinterpret_cast<LPCWSTR>( AFX_IDS_COMPANY_NAME ) );
	CString appName     ( reinterpret_cast<LPCWSTR>( AFX_IDS_APP_TITLE ) );

	folder.AppendFormat( L"\\%s", companyName );
	if ( ! CreateDirectory( folder, NULL ) && ( ERROR_ALREADY_EXISTS != GetLastError( ) ) ) {
		debug( L"EnsureFreeComposeFolderExists: CreateDirectory('%s') failed: %d\n", folder, GetLastError( ) );
		return false;
	}

	folder.AppendFormat( L"\\%s", appName );
	if ( ! CreateDirectory( folder, NULL ) && ( ERROR_ALREADY_EXISTS != GetLastError( ) ) ) {
		debug( L"EnsureFreeComposeFolderExists: CreateDirectory('%s') failed: %d\n", folder, GetLastError( ) );
		return false;
	}

	return true;
}
