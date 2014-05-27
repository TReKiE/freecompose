#include "stdafx.h"

#include "Unicode.h"
#include "Utils.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

// XXX
CString VscToString( UINT vsc ) {
	wchar_t buf[256];
	int rc;

	memset( buf, 0, sizeof( buf ) );
	rc = GetKeyNameText( vsc, buf, _countof( buf ) );
	if ( 0 == rc ) {
		debug( L"VscToString(0x%08x): GetKeyNameText: error %lu\n", vsc, GetLastError( ) );
		return CString( );
	}

	return CString( buf );
}

// XXX
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
	ToUnicodeEx( vk, vsc, keyState, keyBuf, _countof( keyBuf ), 0, GetKeyboardLayout( 0 ) );
	return CString( keyBuf );
}

// XXX
CString VkToKeyLabel( DWORD vk ) {
	CString str( VscToString( VkToVsc( vk ) ) );
	// XXX htf do we get the AltGr label??
	if ( 0 != ( vk & 0x80000000UL ) ) {
		str.Insert( 0, VscToString( VkToVsc( VK_SHIFT ) ) + CString( L"+" ) );
	}
	return str;
}

// XXX
UINT VkToVsc( DWORD vk ) {
	UINT vsc = MapVirtualKeyEx( vk & 0x7fffffffUL, MAPVK_VK_TO_VSC, GetKeyboardLayout( 0 ) );
	if ( 0 == vsc )
		return 0;

	return MAKELONG( 1, vsc );
}

DWORD GetComCtl32Version( void ) {
	if ( g_CommonControlsApiVersion ) {
		return g_CommonControlsApiVersion;
	}

	HMODULE hmod = LoadLibrary( L"COMCTL32.DLL" );
	if ( !hmod ) {
		debug( L"GetComCtl32Version: LoadLibrary failed: %lu\n", GetLastError( ) );
		return 0;
	}

	DLLGETVERSIONPROC pfnDllGetVersion = reinterpret_cast<DLLGETVERSIONPROC>( GetProcAddress( hmod, "DllGetVersion" ) );
	if ( !pfnDllGetVersion ) {
		debug( L"GetComCtl32Version: GetProcAddress failed: %lu\n", GetLastError( ) );
		FreeLibrary( hmod );
		return 0;
	}

	DLLVERSIONINFO2 dvi = { sizeof( DLLVERSIONINFO2 ), };
	HRESULT hr = (*pfnDllGetVersion)( &dvi.info1 );
	if ( FAILED( hr ) ) {
		debug( L"GetComCtl32Version: DLLGETVERSIONPROC call failed, hr=0x%08lX\n", hr );
	}

	FreeLibrary( hmod );

	g_CommonControlsApiVersion     = PACKVERSION( dvi.info1.dwMajorVersion, dvi.info1.dwMinorVersion );
	g_CommonControlsDllMajorVersion = static_cast<unsigned>( ( dvi.ullVersion >> 48 ) & 0xFFFFull );
	g_CommonControlsDllMinorVersion = static_cast<unsigned>( ( dvi.ullVersion >> 32 ) & 0xFFFFull );
	g_CommonControlsDllBuildNumber  = static_cast<unsigned>( ( dvi.ullVersion >> 16 ) & 0xFFFFull );
	g_CommonControlsDllQfeNumber    = static_cast<unsigned>( ( dvi.ullVersion       ) & 0xFFFFull );

	return g_CommonControlsApiVersion;
}

//
// The following method is adapted from code by Joseph M. Newcomer in a CodeProject article
// titled 'Avoiding Multiple Instances of an Application', originally published on 2000-05-16
// at http://www.codeproject.com/Articles/538/Avoiding-Multiple-Instances-of-an-Application .
//
CString MakeInstanceExclusionName( CString const& input, EXCLUSION_KIND const kind ) {
	switch ( kind ) {
		case UNIQUE_TO_SYSTEM:
			return input;

		case UNIQUE_TO_DESKTOP: {
			CString s = input;

			HDESK desktop = GetThreadDesktop( GetCurrentThreadId( ) );

			DWORD len;
			SetLastError( ERROR_SUCCESS );
			BOOL result = GetUserObjectInformation( desktop, UOI_NAME, nullptr, 0, &len );
			DWORD err = GetLastError( );
			if ( !result && ( ERROR_INSUFFICIENT_BUFFER == err ) ) {
				BYTE* data = new BYTE[len];
				GetUserObjectInformation( desktop, UOI_NAME, data, len, &len );
				s += L"-";
				s += reinterpret_cast<wchar_t*>( data );
				delete[] data;
			} else {
				s += L"-static";
			}
			return s;
		}

		case UNIQUE_TO_SESSION: {
			CString s = input;

			HANDLE token;
			if ( OpenProcessToken( GetCurrentProcess( ), TOKEN_QUERY, &token ) ) {
				DWORD len;
				GetTokenInformation( token, TokenStatistics, nullptr, 0, &len );

				LPBYTE data = new BYTE[len];
				GetTokenInformation( token, TokenStatistics, data, len, &len );

				CloseHandle( token );

				LUID uid = reinterpret_cast<PTOKEN_STATISTICS>( data )->AuthenticationId;

				CString t;
				t.Format( L"-%08x%08x", uid.HighPart, uid.LowPart );
				s += t;

				delete[] data;
			}
			return s;
		}

		case UNIQUE_TO_TRUSTEE: {
			size_t const NAMELENGTH = 64;

			CString s = input;

			wchar_t userName[NAMELENGTH];
			DWORD userNameLength = NAMELENGTH;
			GetUserName( userName, &userNameLength );

			// The NetApi calls are very time consuming.
			// This technique gets the domain name via an environment variable.
			wchar_t domainName[NAMELENGTH];
			DWORD domainNameLength = ExpandEnvironmentStrings( L"%USERDOMAIN%", domainName, NAMELENGTH );

			if ( userNameLength < 1 ) {
				wcscpy_s( userName, L"<unknown_username>" );
			}
			if ( domainNameLength < 1 ) {
				wcscpy_s( domainName, L"<unknown_domainname>" );
			}

			CString t;
			t.Format( L"-%s-%s", domainName, userName );
			return s + t;
		}

		default:
			debug( L"CreateExclusionName: invalid value for EXCLUSION_KIND kind: %d\n", kind );
			return input;
	}
}

CString FormatCodePoint( CString const& str ) {
	CString strResult;
	int limit = 0;
	UChar32* pqzCodePoints = Utf16ToUtf32( str, str.GetLength( ), limit );
	for ( int index = 0; index < limit; index++ ) {
		strResult.AppendFormat( L"%sU+%06X", ( index ? L", " : L"" ), pqzCodePoints[index] );
	}
	delete[] pqzCodePoints;
	return strResult;
}
