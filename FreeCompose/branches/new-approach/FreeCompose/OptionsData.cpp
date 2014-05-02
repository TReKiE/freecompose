#include "stdafx.h"

#include <psapi.h>

#include <Unicode.h>

#include "FreeCompose.h"
#include "OptionsData.h"

#include "Utils.h"

#define FORCE_DEFAULT_CONFIG 0
#define FORCE_REGISTRY_CONFIG 0

COptionsData& COptionsData::operator=( COptionsData const& options ) {
	StartActive        = options.StartActive;
	StartWithWindows   = options.StartWithWindows;
	CapsLockToggleMode = options.CapsLockToggleMode;
	CapsLockSwapMode   = options.CapsLockSwapMode;
	ComposeVk          = options.ComposeVk;
	SwapCapsLockVk     = options.SwapCapsLockVk;

	ComposeSequences.RemoveAll( );
	ComposeSequences.Copy( options.ComposeSequences );

	return *this;
}

bool COptionsData::operator==( COptionsData const& options ) {
	if ( StartActive        != options.StartActive        ) { return false; }
	if ( StartWithWindows   != options.StartWithWindows   ) { return false; }
	if ( CapsLockToggleMode != options.CapsLockToggleMode ) { return false; }
	if ( CapsLockSwapMode   != options.CapsLockSwapMode   ) { return false; }
	if ( ComposeVk          != options.ComposeVk          ) { return false; }
	if ( SwapCapsLockVk     != options.SwapCapsLockVk     ) { return false; }

	if ( ComposeSequences.GetCount( ) != options.ComposeSequences.GetCount( ) ) {
		return false;
	}
	for ( int n = 0; n < ComposeSequences.GetCount( ); n++ ) {
		if ( ComposeSequences[n] != options.ComposeSequences[n] ) {
			return false;
		}
	}

	return true;
}

bool COptionsData::operator!=( COptionsData const& options ) {
	return ! operator==( options );
}

bool COptionsData::_CheckIfRegistryKeyExists( void ) {
	CString registryKeyName( CString( L"Software\\" ) + theApp.m_pszRegistryKey + L"\\" + theApp.m_pszProfileName );
	HKEY hkey;

	LSTATUS rc = RegOpenKeyEx( HKEY_CURRENT_USER, registryKeyName, 0, KEY_READ, &hkey );
	if ( ERROR_SUCCESS != rc ) {
		debug( L"COptionsData::_LoadFromRegistry: can't open registry key '%s': error %d\n", registryKeyName );
		return false;
	}

	RegCloseKey( hkey );
	return true;
}

void COptionsData::_LoadSequencesFromRegistry( void ) {
	ComposeSequences.RemoveAll( );

	unsigned count = theApp.GetProfileInt( L"Mapping", L"Count", 0 );
	if ( count < 1 ) {
		return;
	}

	ComposeSequence sequence;
	CString section;
	ComposeSequences.SetSize( count );

	debug( L"COptionsData::_LoadSequencesFromRegistry: Loading %d mappings from registry:\n", count );
	for ( unsigned n = 0; n < count; n++ ) {
		section.Format( L"Mapping\\%d", n );
		CString first    ( VkToString  ( theApp.GetProfileInt( section, L"First",    0 ) ) );
		CString second   ( VkToString  ( theApp.GetProfileInt( section, L"Second",   0 ) ) );
		CString composed ( Utf32ToUtf16( theApp.GetProfileInt( section, L"Composed", 0 ) ) );
		sequence.Sequence = first + second;
		sequence.Result = composed;
		debug( L"+ Mapping %3d: '%s' => ='%s'\n", n, static_cast<LPCWSTR>( sequence.Sequence ), static_cast<LPCWSTR>( sequence.Result ) );
		ComposeSequences[n] = sequence;
	}
	debug( L"COptionsData::_LoadSequencesFromRegistry: Mapping load completed.\n" );
}

bool COptionsData::_LoadFromRegistry( void ) {
	if ( !_CheckIfRegistryKeyExists( ) ) {
		return false;
	}

	BOOL fSwapCapsLock;
	int nCapsLockSwapMode;

	StartActive        =  static_cast<BOOL>( theApp.GetProfileInt( L"Startup",  L"StartActive",        TRUE ) );
	StartWithWindows   =  static_cast<BOOL>( theApp.GetProfileInt( L"Startup",  L"StartWithWindows",   FALSE ) );

	fSwapCapsLock      =  static_cast<BOOL>( theApp.GetProfileInt( L"Keyboard", L"SwapCapsLock",       FALSE ) );

	CapsLockToggleMode =
		 static_cast<CAPS_LOCK_TOGGLE_MODE>( theApp.GetProfileInt( L"Keyboard", L"CapsLockToggleMode", 
		 static_cast<CAPS_LOCK_TOGGLE_MODE>( theApp.GetProfileInt( L"Keyboard", L"CapsLockMode",       CLTM_NORMAL ) ) ) );
	// 0 will map to CLSM_NORMAL if fSwapCapsLock == TRUE
	nCapsLockSwapMode  =                     theApp.GetProfileInt( L"Keyboard", L"CapsLockSwapMode",   0 ) + 1;

	ComposeVk          = static_cast<DWORD>( theApp.GetProfileInt( L"Keyboard", L"ComposeKey",         VK_APPS ) );
	SwapCapsLockVk     = static_cast<DWORD>( theApp.GetProfileInt( L"Keyboard", L"SwapCapsLockKey",    VK_LCONTROL ) );
	
	CapsLockSwapMode   = fSwapCapsLock ? static_cast<CAPS_LOCK_SWAP_MODE>( nCapsLockSwapMode ) : CLSM_NORMAL;

	_LoadSequencesFromRegistry( );

	return true;
}

void COptionsData::_UpdateRunKey( void ) {
	LSTATUS rc;
	HKEY hkey;
	
	rc = RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hkey );
	if ( ERROR_SUCCESS != rc ) {
		debug( L"COptionsData::_UpdateRunKey: RegOpenKeyEx failed: %d\n", rc );
		return;
	}

	if ( StartWithWindows ) {
		wchar_t lpszImageFilename[1024];
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
	RegCloseKey( hkey );
}

void COptionsData::Load( void ) {
#if !FORCE_DEFAULT_CONFIG
#if !FORCE_REGISTRY_CONFIG
	debug( L"COptionsData::Load: Trying to load XML configuration file.\n" );
	if ( _xmlOptionsHandler.LoadFromFile( ) ) {
		debug( L"COptionsData::Load: XML configuration file loaded.\n" );
		return;
	}

	debug( L"COptionsData::Load: Couldn't load XML, trying the registry\n" );
#endif
	if ( _LoadFromRegistry( ) ) {
		debug( L"COptionsData::Load: Loaded configuration from registry, saving to XML\n" );
		if ( !_xmlOptionsHandler.SaveToFile( ) ) {
			debug( L"COptionsData::Load: Couldn't save registry configuration to XML\n" );
			return;
		}
	}
#endif

	debug( L"COptionsData::Load: Loading default configuration\n" );
	_xmlOptionsHandler.LoadDefaultConfiguration( );
	if ( !_xmlOptionsHandler.SaveToFile( ) ) {
		debug( L"COptionsData::Load: Couldn't save default configuration to XML\n" );
	}
}

void COptionsData::Save( void ) {
	_xmlOptionsHandler.SaveToFile( );
	_UpdateRunKey( );
}
