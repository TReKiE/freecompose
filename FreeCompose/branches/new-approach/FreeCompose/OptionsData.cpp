#include "stdafx.h"

#include <psapi.h>

#include <Unicode.h>

#include "FreeCompose.h"
#include "OptionsData.h"

#include "Utils.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

COptionsData& COptionsData::operator=( COptionsData const& options ) {
	StartActive        = options.StartActive;
	StartWithWindows   = options.StartWithWindows;
	CapsLockToggleMode = options.CapsLockToggleMode;
	CapsLockSwapMode   = options.CapsLockSwapMode;
	ComposeVk          = options.ComposeVk;
	SwapCapsLockVk     = options.SwapCapsLockVk;
	Sounds             = options.Sounds;

	ComposeSequenceGroups.RemoveAll( );
	ComposeSequenceGroups.Append( options.ComposeSequenceGroups );

	return *this;
}

bool COptionsData::operator==( COptionsData const& options ) const {
	if ( StartActive        != options.StartActive        ) { return false; }
	if ( StartWithWindows   != options.StartWithWindows   ) { return false; }
	if ( CapsLockToggleMode != options.CapsLockToggleMode ) { return false; }
	if ( CapsLockSwapMode   != options.CapsLockSwapMode   ) { return false; }
	if ( ComposeVk          != options.ComposeVk          ) { return false; }
	if ( SwapCapsLockVk     != options.SwapCapsLockVk     ) { return false; }
	if ( Sounds             != options.Sounds             ) { return false; }

	if ( ComposeSequenceGroups.GetCount( ) != options.ComposeSequenceGroups.GetCount( ) ) {
		return false;
	}

	INT_PTR groupCount = ComposeSequenceGroups.GetCount( );
	for ( int groupIndex = 0; groupIndex < groupCount; groupIndex++ ) {
		ComposeSequenceGroup const& lGroup = ComposeSequenceGroups[groupIndex];
		ComposeSequenceGroup const& rGroup = options.ComposeSequenceGroups[groupIndex];

		if ( 0 != lGroup.Name.Compare( rGroup.Name ) ) {
			return false;
		}

		if ( lGroup.ComposeSequences.GetCount( ) != rGroup.ComposeSequences.GetCount( ) ) {
			return false;
		}

		INT_PTR sequenceCount = lGroup.ComposeSequences.GetCount( );
		for ( int sequenceIndex = 0; sequenceIndex < sequenceCount; sequenceIndex++ ) {
			if ( lGroup.ComposeSequences[sequenceIndex] != rGroup.ComposeSequences[sequenceIndex] ) {
				return false;
			}
		}
	}

	return true;
}

bool COptionsData::operator!=( COptionsData const& options ) const {
	return !operator==( options );
}

bool COptionsData::_CheckIfRegistryKeyExists( void ) const {
	CString registryKeyName( CString( L"Software\\" ) + theApp.m_pszRegistryKey + L"\\" + theApp.m_pszProfileName );
	CRegKey key;

	LSTATUS rc = key.Open( HKEY_CURRENT_USER, static_cast<LPCWSTR>( registryKeyName ), KEY_READ );
	if ( ERROR_SUCCESS != rc ) {
		debug( L"COptionsData::_LoadFromRegistry: can't open registry key '%s': error %ld\n", registryKeyName.GetString( ), rc );
		return false;
	}

	key.Close( );
	return true;
}

void COptionsData::_LoadSequencesFromRegistry( void ) {
	ComposeSequenceGroups.RemoveAll( );

	unsigned count = theApp.GetProfileInt( L"Mapping", L"Count", 0 );
	if ( count < 1 ) {
		return;
	}

	ComposeSequenceGroup group;
	ComposeSequence sequence;
	CString section;
	group.Name = LoadFromStringTable( IDS_DEFAULT_GROUP_NAME );

	for ( unsigned n = 0; n < count; n++ ) {
		section.Format( L"Mapping\\%u", n );
		CString first    (   VkToString( theApp.GetProfileInt( section, L"First",    0 ) ) );
		CString second   (   VkToString( theApp.GetProfileInt( section, L"Second",   0 ) ) );
		CString composed ( Utf32ToUtf16( theApp.GetProfileInt( section, L"Composed", 0 ) ) );
		sequence.Sequence = first + second;
		sequence.Result = composed;
		group.ComposeSequences.Add( sequence );
	}

	ComposeSequenceGroups.Add( group );
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
	CRegKey key;
	
	rc = key.Open( HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", KEY_SET_VALUE );
	if ( ERROR_SUCCESS != rc ) {
		debug( L"COptionsData::_UpdateRunKey: CRegKey::Open failed: %d\n", rc );
		return;
	}

	if ( StartWithWindows ) {
		wchar_t lpszImageFilename[1024];
		rc = GetModuleFileNameEx( GetCurrentProcess( ), AfxGetApp( )->m_hInstance, lpszImageFilename, _countof( lpszImageFilename ) );
		if ( rc > 0 ) {
#ifndef _DEBUG
			rc = key.SetStringValue( L"FreeCompose", lpszImageFilename, REG_SZ );
			if ( ERROR_SUCCESS != rc ) {
				debug( L"COptionsData::_UpdateRunKey: key.SetStringValue failed: %d\n", rc );
			}
#endif
		} else {
			debug( L"COptionsData::_UpdateRunKey: GetModuleFileNameEx failed: %lu\n", GetLastError( ) );
		}
	} else {
#ifndef _DEBUG
		rc = key.DeleteValue( L"FreeCompose" );
		if ( ERROR_SUCCESS != rc ) {
			debug( L"COptionsData::_UpdateRunKey: key.DeleteValue failed: %d\n", rc );
		}
#endif
	}
	key.Close( );
}

ComposeSequenceGroup* COptionsData::FindComposeSequenceGroup( CString const& name ) {
	INT_PTR limit = ComposeSequenceGroups.GetCount( );
	for ( int index = 0; index < limit; index++ ) {
		if ( 0 == name.CompareNoCase( ComposeSequenceGroups[index].Name ) ) {
			return &ComposeSequenceGroups[index];
		}
	}

	return nullptr;
}

void COptionsData::Load( void ) {
	BOOL bForceDefault  = theApp.m_CommandLineInfo.m_bForceDefaultConfiguration;
	BOOL bForceRegistry = theApp.m_CommandLineInfo.m_bForceRegistryConfiguration;

	if ( !bForceDefault && !bForceRegistry ) {
		debug( L"COptionsData::Load: Trying to load user configuration file\n" );
		if ( _xmlOptionsHandler.LoadFromFile( ) ) {
			debug( L"COptionsData::Load: User configuration file loaded\n" );
			return;
		}
	}

	if ( !bForceDefault ) {
		debug( L"COptionsData::Load: Trying to import user configuration from the registry\n" );
		if ( _LoadFromRegistry( ) ) {
			debug( L"COptionsData::Load: Imported configuration from the registry, saving to XML\n" );
			if ( !_xmlOptionsHandler.SaveToFile( ) ) {
				debug( L"COptionsData::Load: Couldn't save configuration imported from the registry\n" );
			}
			return;
		}
	}

	debug( L"COptionsData::Load: Loading default configuration\n" );
	_xmlOptionsHandler.LoadDefaultConfiguration( );
	if ( !_xmlOptionsHandler.SaveToFile( ) ) {
		debug( L"COptionsData::Load: Couldn't save default configuration\n" );
	}
}

void COptionsData::Save( void ) {
	_xmlOptionsHandler.SaveToFile( );
	_UpdateRunKey( );
}
