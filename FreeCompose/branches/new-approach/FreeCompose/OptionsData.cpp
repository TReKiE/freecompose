#include "stdafx.h"

#include <psapi.h>

#include <Unicode.h>

#include "FreeCompose.h"
#include "OptionsData.h"

#include "ComposeDefaults.h"
#include "Utils.h"

#undef FORCE_DEFAULT_CONFIG

COptionsData::COptionsData( ) {
}

COptionsData::COptionsData( COptionsData const& options ) {
	operator=( options );
}

COptionsData::~COptionsData( ) {
	m_ComposeSequences.RemoveAll( );
}

COptionsData& COptionsData::operator=( COptionsData const& options ) {
	m_fStartActive       = options.m_fStartActive;
	m_fStartWithWindows  = options.m_fStartWithWindows;
	m_CapsLockToggleMode = options.m_CapsLockToggleMode;
	m_CapsLockSwapMode   = options.m_CapsLockSwapMode;
	m_vkCompose          = options.m_vkCompose;
	m_vkSwapCapsLock     = options.m_vkSwapCapsLock;

	m_ComposeSequences.RemoveAll( );
	m_ComposeSequences.Copy( options.m_ComposeSequences );

	return *this;
}

bool COptionsData::operator==( COptionsData const& options ) {
	if ( m_fStartActive       != options.m_fStartActive       ) { return false; }
	if ( m_fStartWithWindows  != options.m_fStartWithWindows  ) { return false; }
	if ( m_CapsLockToggleMode != options.m_CapsLockToggleMode ) { return false; }
	if ( m_CapsLockSwapMode   != options.m_CapsLockSwapMode   ) { return false; }
	if ( m_vkCompose          != options.m_vkCompose          ) { return false; }
	if ( m_vkSwapCapsLock     != options.m_vkSwapCapsLock     ) { return false; }

	if ( m_ComposeSequences.GetCount( ) != options.m_ComposeSequences.GetCount( ) ) {
		return false;
	}
	for ( int n = 0; n < m_ComposeSequences.GetCount( ); n++ ) {
		if ( m_ComposeSequences[n] != options.m_ComposeSequences[n] ) {
			return false;
		}
	}

	return true;
}

bool COptionsData::operator!=( COptionsData const& options ) {
	return ! operator==( options );
}

bool COptionsData::_FcValidateSequence( COMPOSE_SEQUENCE const& sequence ) {
	if ( ! sequence.chFirst    || IsSurrogate( sequence.chFirst    ) ) { return false; }
	if ( ! sequence.chSecond   || IsSurrogate( sequence.chSecond   ) ) { return false; }
	if ( ! sequence.chComposed || IsSurrogate( sequence.chComposed ) ) { return false; }
	return true;
}

void COptionsData::_FcLoadDefaultConfiguration( void ) {
	m_fStartActive = TRUE;
	m_fStartWithWindows = FALSE;
	m_CapsLockToggleMode = CLTM_NORMAL;
	m_CapsLockSwapMode = CLSM_NORMAL;
	m_vkCompose = VK_APPS;
	m_vkSwapCapsLock = VK_LCONTROL;

	size_t count = _countof( DefaultComposeKeyEntries );
	m_ComposeSequences.SetSize( count );
	for ( size_t n = 0; n < count; n++ ) {
		m_ComposeSequences[n] = DefaultComposeKeyEntries[n];
	}
}

bool COptionsData::_FcLoadFromRegistry( void ) {
	BOOL fSwapCapsLock;
	int nCapsLockSwapMode;

	{
		HKEY hkey;
		LSTATUS rc = RegOpenKeyEx( HKEY_CURRENT_USER, (LPCWSTR) ( CString( L"Software\\" ) + theApp.m_pszRegistryKey + L"\\" + theApp.m_pszProfileName ), 0, KEY_READ, &hkey );
		if ( ERROR_SUCCESS != rc ) {
			debug( L"COptionsData::_FcLoadFromRegistry: can't open key HKCU\\Software\\%s\\%s: error %d\n", theApp.m_pszRegistryKey, theApp.m_pszProfileName );
			return false;
		}
		RegCloseKey( hkey );
	}

	m_fStartActive       = (BOOL)  theApp.GetProfileInt( L"Startup",  L"StartActive",        TRUE );
	m_fStartWithWindows  = (BOOL)  theApp.GetProfileInt( L"Startup",  L"StartWithWindows",   FALSE );

	fSwapCapsLock        = (BOOL)  theApp.GetProfileInt( L"Keyboard", L"SwapCapsLock",       FALSE );

	m_CapsLockToggleMode =
		   (CAPS_LOCK_TOGGLE_MODE) theApp.GetProfileInt( L"Keyboard", L"CapsLockToggleMode", 
		   (CAPS_LOCK_TOGGLE_MODE) theApp.GetProfileInt( L"Keyboard", L"CapsLockMode",       CLTM_NORMAL ) );
	// 0 will map to CLSM_NORMAL if fSwapCapsLock == TRUE
	nCapsLockSwapMode    =         theApp.GetProfileInt( L"Keyboard", L"CapsLockSwapMode",   0 ) + 1;

	m_vkCompose          = (DWORD) theApp.GetProfileInt( L"Keyboard", L"ComposeKey",         VK_APPS );
	m_vkSwapCapsLock     = (DWORD) theApp.GetProfileInt( L"Keyboard", L"SwapCapsLockKey",    VK_LCONTROL );
	
	m_CapsLockSwapMode   = fSwapCapsLock ? (CAPS_LOCK_SWAP_MODE) nCapsLockSwapMode : CLSM_NORMAL;

	_FcLoadKeys( );

	return true;
}

void COptionsData::_FcLoadKeys( void ) {
	m_ComposeSequences.RemoveAll( );

	int count = theApp.GetProfileInt( L"Mapping", L"Count", 0 );
	if ( count < 1 ) {
		return;
	}

	COMPOSE_SEQUENCE sequence;
	CString section;
	m_ComposeSequences.SetSize( count );

	debug( L"COptionsData::_FcLoadKeys: Loading %d mappings from registry:\n", count );
	int index = 0;
	for ( int n = 0; n < count; n++ ) {
		section.Format( L"Mapping\\%d", n );
		sequence.chFirst    = (unsigned) theApp.GetProfileInt( section, L"First",    0 );
		sequence.chSecond   = (unsigned) theApp.GetProfileInt( section, L"Second",   0 );
		sequence.chComposed = (unsigned) theApp.GetProfileInt( section, L"Composed", 0 );
		debug( L"+ Mapping %3d: 0x%08X 0x%08X => 0x%08X\n", n, sequence.chFirst, sequence.chSecond, sequence.chComposed );
		if ( _FcValidateSequence( sequence ) ) {
			m_ComposeSequences[index++] = sequence;
		} else {
			debug( L"+ Bad mapping #%d\n", n );
		}
	}
	debug( L"COptionsData::_FcLoadKeys: Mapping load completed, %d mappings validated and loaded.\n", index );
}

void COptionsData::_UpdateRunKey( void ) {
	wchar_t lpszImageFilename[1024];
	LSTATUS rc;
	HKEY hk;
	
	rc = RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hk );
	if ( ERROR_SUCCESS != rc ) {
		debug( L"COptionsData::_UpdateRunKey: RegOpenKeyEx failed: %d\n", rc );
		return;
	}

	if ( m_fStartWithWindows ) {
		rc = GetModuleFileNameEx( GetCurrentProcess( ), AfxGetApp( )->m_hInstance, lpszImageFilename, _countof( lpszImageFilename ) );
		if ( rc > 0 ) {
#ifndef _DEBUG
			rc = RegSetValueEx( hk, L"FreeCompose", 0, REG_SZ, (LPBYTE) lpszImageFilename, (DWORD) ( sizeof(wchar_t) * ( wcslen( lpszImageFilename ) + 1 ) ) );
			if ( ERROR_SUCCESS != rc ) {
				debug( L"COptionsData::_UpdateRunKey: RegSetValueEx failed: %d\n", rc );
			}
#endif
		} else {
			debug( L"COptionsData::_UpdateRunKey: GetModuleFileNameEx failed: %d\n", GetLastError( ) );
		}
	} else {
#ifndef _DEBUG
		rc = RegDeleteValue( hk, L"FreeCompose" );
		if ( ERROR_SUCCESS != rc ) {
			debug( L"COptionsData::_UpdateRunKey: RegDeleteValue failed: %d\n", rc );
		}
#endif
	}
	RegCloseKey( hk );
}

void COptionsData::Load( void ) {
#ifndef FORCE_DEFAULT_CONFIG
	debug( L"COptionsData::Load: Trying to load XML configuration file.\n" );
	if ( _LoadFromXml( ) ) {
		debug( L"COptionsData::Load: XML configuration file loaded.\n" );
		return;
	}

	debug( L"COptionsData::Load: Couldn't load XML, trying the registry\n" );
	if ( _FcLoadFromRegistry( ) ) {
		debug( L"COptionsData::Load: Loaded configuration from registry, saving to XML\n" );
		if ( !_SaveToXml( ) ) {
			debug( L"COptionsData::Load: Couldn't save registry configuration to XML\n" );
			return;
		}
	}
#endif

	debug( L"COptionsData::Load: Loading default configuration\n" );
	_FcLoadDefaultConfiguration( );
	if ( !_SaveToXml( ) ) {
		debug( L"COptionsData::Load: Couldn't save default configuration to XML\n" );
	}
}

void COptionsData::Save( void ) {
	_SaveToXml( );
	_UpdateRunKey( );
}
