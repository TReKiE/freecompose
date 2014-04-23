#include "stdafx.h"
#include "Utils.h"

static DWORD ComCtl32Version = 0UL;

// XXX
CString VscToString( UINT vsc ) {
	wchar_t buf[256];
	int rc;

	memset( buf, 0, sizeof( buf ) );
	rc = GetKeyNameText( vsc, buf, _countof( buf ) );
	if ( 0 == rc ) {
		debug( L"VscToString(0x%08x): GetKeyNameText: error %d\n", vsc, GetLastError( ) );
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

//
// The following method is adapted from code by Joseph M. Newcomer in a CodeProject article
// titled 'Avoiding Multiple Instances of an Application', originally published on 2000-05-16
// at http://www.codeproject.com/Articles/538/Avoiding-Multiple-Instances-of-an-Application .
//
CString MakeInstanceExclusionName( CString const& input, EXCLUSION_KIND const kind ) {
	switch ( kind ) {
		case UNIQUE_TO_SYSTEM:
			return CString( input );

		case UNIQUE_TO_DESKTOP: {
			CString s = input;
			DWORD len;
			HDESK desktop = GetThreadDesktop( GetCurrentThreadId( ) );
			BOOL result = GetUserObjectInformation( desktop, UOI_NAME, NULL, 0, &len );
			DWORD err = ::GetLastError( );
			if ( !result && err == ERROR_INSUFFICIENT_BUFFER ) {
				LPBYTE data = new BYTE[len];
				result = GetUserObjectInformation( desktop, UOI_NAME, data, len, &len );
				s += L"-";
				s += (wchar_t*) data;
				delete[] data;
			} else {
				s += L"-Win9x";
			}
			return s;
		}

		case UNIQUE_TO_SESSION: {
			CString s = input;
			HANDLE token;
			DWORD len;
			BOOL result = OpenProcessToken( GetCurrentProcess( ), TOKEN_QUERY, &token );
			if ( result ) {
				GetTokenInformation( token, TokenStatistics, NULL, 0, &len );
				LPBYTE data = new BYTE[len];
				GetTokenInformation( token, TokenStatistics, data, len, &len );
				LUID uid = ( (PTOKEN_STATISTICS) data )->AuthenticationId;
				CString t;
				t.Format( L"-%08x%08x", uid.HighPart, uid.LowPart );
				s += t;
				delete[] data;
			}
			return s;
		}

		case UNIQUE_TO_TRUSTEE: {
			CString s = input;

			const size_t NAMELENGTH = 64;
			TCHAR userName[NAMELENGTH];
			DWORD userNameLength = NAMELENGTH;
			TCHAR domainName[NAMELENGTH];
			DWORD domainNameLength = NAMELENGTH;

			if ( GetUserName( userName, &userNameLength ) ) {
				// The NetApi calls are very time consuming
				// This technique gets the domain name via an
				// environment variable
				domainNameLength = ExpandEnvironmentStrings( L"%USERDOMAIN%", domainName, NAMELENGTH );
				CString t;
				t.Format( L"-%s-%s", domainName, userName );
				s += t;
			}
			return s;
		}

		default:
			debug( L"CreateExclusionName: invalid value for EXCLUSION_KIND kind: %d\n", kind );
			return CString( input );
	}
}

CString GetFreeComposeFolderAsCString( void ) {
	CString str;
	LPWSTR lpsz;
	if ( GetFreeComposeFolder( lpsz ) ) {
		str = lpsz;
		CoTaskMemFree( lpsz );
	}
	return str;
}
