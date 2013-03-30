#include "stdafx.h"

#include <psapi.h>

#include <Unicode.h>

#include "FreeCompose.h"
#include "OptionsData.h"

#include "ComposeDefaults.h"
#include "Utils.h"

COptionsData::COptionsData( ) {
	Load( );
}

COptionsData::COptionsData( const COptionsData& options ) {
	operator=( options );
}

COptionsData::~COptionsData( ) {
	m_ComposeSequences.RemoveAll( );
}

COptionsData& COptionsData::operator=( const COptionsData& options ) {
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

bool COptionsData::operator==( const COptionsData& options ) {
	if ( m_fStartActive       != options.m_fStartActive       ) return false;
	if ( m_fStartWithWindows  != options.m_fStartWithWindows  ) return false;
	if ( m_CapsLockToggleMode != options.m_CapsLockToggleMode ) return false;
	if ( m_CapsLockSwapMode   != options.m_CapsLockSwapMode   ) return false;
	if ( m_vkCompose          != options.m_vkCompose          ) return false;
	if ( m_vkSwapCapsLock     != options.m_vkSwapCapsLock     ) return false;

	if ( m_ComposeSequences.GetCount( ) != options.m_ComposeSequences.GetCount( ) )
		return false;
	for ( int n = 0; n < m_ComposeSequences.GetCount( ); n++ ) {
		if ( m_ComposeSequences[n] != options.m_ComposeSequences[n] )
			return false;
	}

	return true;
}

bool COptionsData::operator!=( const COptionsData& options ) {
	return ! operator==( options );
}

bool COptionsData::_FcValidateSequence( const COMPOSE_SEQUENCE& sequence ) {
	if ( ! sequence.chFirst    || IsSurrogate( sequence.chFirst ) )
		return false;
	if ( ! sequence.chSecond   || IsSurrogate( sequence.chSecond ) )
		return false;
	if ( ! sequence.chComposed || IsSurrogate( sequence.chComposed ) )
		return false;

	return true;
}

void COptionsData::_FcLoadKeys( void ) {
	m_ComposeSequences.RemoveAll( );

	int count = theApp.GetProfileInt( L"Mapping", L"Count", 0 );
	if ( count < 1 ) {
		count = _countof( DefaultComposeKeyEntries );
		m_ComposeSequences.SetSize( count );
		for ( int n = 0; n < count; n++ ) {
			m_ComposeSequences[n] = DefaultComposeKeyEntries[n];
		}
		return;
	}

	COMPOSE_SEQUENCE sequence;
	CString section;
	m_ComposeSequences.SetSize( count );
#if _DEBUG
	m_ComposeSequences.Dump( afxDump );
#endif
	int index = 0;
	for ( int n = 0; n < count; n++ ) {
		section.Format( L"Mapping\\%d", n );
		sequence.chFirst    = (unsigned) theApp.GetProfileInt( section, L"First",    0 );
		sequence.chSecond   = (unsigned) theApp.GetProfileInt( section, L"Second",   0 );
		sequence.chComposed = (unsigned) theApp.GetProfileInt( section, L"Composed", 0 );
		if ( _FcValidateSequence( sequence ) ) {
			m_ComposeSequences[index++] = sequence;
		} else {
			debug( L"Bad mapping #%d\n", n );
		}
	}
}

void COptionsData::_FcSaveKeys( void ) {
	theApp.DelRegTree( theApp.GetAppRegistryKey( ), CString( L"Mapping" ) );

	CString section;
	int count = 0;
	for ( int n = 0; n < (int) m_ComposeSequences.GetSize( ); n++ ) {
		if ( !m_ComposeSequences[n].chFirst && !m_ComposeSequences[n].chSecond && !m_ComposeSequences[n].chComposed ) {
			continue;
		}
		section.Format( L"Mapping\\%d", count++ );
		theApp.WriteProfileInt( section, L"First",    (int) m_ComposeSequences[n].chFirst    );
		theApp.WriteProfileInt( section, L"Second",   (int) m_ComposeSequences[n].chSecond   );
		theApp.WriteProfileInt( section, L"Composed", (int) m_ComposeSequences[n].chComposed );
	}

	theApp.WriteProfileInt( L"Mapping", L"Count", count );
}

void COptionsData::_UpdateRunKey( void ) {
	LSTATUS rc;
	HKEY hk;
	
	rc = RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hk );
	if ( ERROR_SUCCESS != rc ) {
		debug( L"COptionsData::_UpdateRunKey: RegOpenKeyEx failed: %d\n", rc );
		return;
	}

	if ( m_fStartWithWindows ) {
		wchar_t lpszImageFilename[1024];
		if ( GetModuleFileNameEx( GetCurrentProcess( ), AfxGetApp( )->m_hInstance, lpszImageFilename, _countof( lpszImageFilename ) ) > 0 ) {
#ifdef NDEBUG
			rc = RegSetValueEx( hk, L"FreeCompose", 0, REG_SZ, (LPBYTE) lpszImageFilename, (DWORD) ( sizeof(wchar_t) * ( wcslen( lpszImageFilename ) + 1 ) ) );
			if ( ERROR_SUCCESS != rc ) {
				debug( L"COptionsData::_UpdateRunKey: RegSetValueEx failed: %d\n", rc );
			}
#endif
		} else {
			debug( L"COptionsData::_UpdateRunKey: GetModuleFileNameEx failed: %d\n", GetLastError( ) );
		}
	} else {
		rc = RegDeleteValue( hk, L"FreeCompose" );
		if ( ERROR_SUCCESS != rc ) {
			debug( L"COptionsData::_UpdateRunKey: RegDeleteValue failed: %d\n", rc );
		}
	}

	RegCloseKey( hk );
}

void COptionsData::Load( void ) {
#ifdef NDEBUG
	if ( LoadFromXml( ) ) {
		return;
	}
#endif
	BOOL fSwapCapsLock;
	int nCapsLockSwapMode;

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
}

void COptionsData::Save( void ) {
	SaveToXml( );
	_UpdateRunKey( );
}
