#include "stdafx.h"

#include <psapi.h>

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

	m_fSwapCapsLock      = options.m_fSwapCapsLock;
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
	if ( m_fSwapCapsLock      != options.m_fSwapCapsLock      ) return false;
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
	if ( 0 == ( sequence.chFirst &  0xFF ) )
		return false;
	if ( 0 != ( sequence.chFirst & ~0x800000FF ) )
		return false;

	if ( 0 == ( sequence.chSecond &  0xFF ) )
		return false;
	if ( 0 != ( sequence.chSecond & ~0x800000FF ) )
		return false;

	if ( 0 == sequence.chComposed )
		return false;
	if ( sequence.chComposed >= 0xD800u && sequence.chComposed < 0xE000u )
		return false;
	if ( sequence.chComposed > 0x10FFFFu )
		return false;

	return true;
}

void COptionsData::_FcLoadKeys( void ) {
	m_ComposeSequences.RemoveAll( );

	int count = theApp.GetProfileInt( _T("Mapping"), _T("Count"), 0 );
	if ( count < 1 ) {
		count = _countof( DefaultComposeKeyEntries );
		m_ComposeSequences.SetSize( count );
		for ( int n = 0; n < count; n++ ) {
			m_ComposeSequences[n] = DefaultComposeKeyEntries[n];
		}
		return;
	}

	COMPOSE_SEQUENCE cke;
	CString section;
	m_ComposeSequences.SetSize( count );
	int index = 0;
	for ( int n = 0; n < count; n++ ) {
		section.Format( _T("Mapping\\%d"), n );
		cke.chFirst    = (unsigned) theApp.GetProfileInt( section, _T("First"),    0 );
		cke.chSecond   = (unsigned) theApp.GetProfileInt( section, _T("Second"),   0 );
		cke.chComposed = (unsigned) theApp.GetProfileInt( section, _T("Composed"), 0 );
		if ( _FcValidateSequence( cke ) ) {
			m_ComposeSequences[index++] = cke;
		} else {
			debug( _T("Bad mapping, #%d\n"), n );
		}
	}
}

void COptionsData::_FcSaveKeys( void ) {
	theApp.DelRegTree( theApp.GetAppRegistryKey( ), CString( _T("Mapping") ) );

	CString section;
	int count = 0;
	for ( int n = 0; n < (int) m_ComposeSequences.GetSize( ); n++ ) {
		if ( !m_ComposeSequences[n].chFirst && !m_ComposeSequences[n].chSecond && !m_ComposeSequences[n].chComposed ) {
			continue;
		}
		section.Format( _T("Mapping\\%d"), count++ );
		theApp.WriteProfileInt( section, _T("First"),    (int) m_ComposeSequences[n].chFirst    );
		theApp.WriteProfileInt( section, _T("Second"),   (int) m_ComposeSequences[n].chSecond   );
		theApp.WriteProfileInt( section, _T("Composed"), (int) m_ComposeSequences[n].chComposed );
	}

	theApp.WriteProfileInt( _T("Mapping"), _T("Count"), count );
}

void COptionsData::_UpdateRunKey( void ) {
	LSTATUS rc;
	HKEY hk;
	
	rc = RegOpenKeyEx( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_SET_VALUE, &hk );
	if ( ERROR_SUCCESS != rc ) {
		debug( _T("COptionsData::_UpdateRunKey: RegOpenKeyEx failed: %d\n"), rc );
		return;
	}

	if ( m_fStartWithWindows ) {
		wchar_t lpszImageFilename[1024];
		if ( GetModuleFileNameEx( GetCurrentProcess( ), AfxGetApp( )->m_hInstance, lpszImageFilename, _countof( lpszImageFilename ) ) > 0 ) {
			rc = RegSetValueEx( hk, _T("FreeCompose"), 0, REG_SZ, (LPBYTE) lpszImageFilename, (DWORD) ( sizeof(wchar_t) * ( wcslen( lpszImageFilename ) + 1 ) ) );
			if ( ERROR_SUCCESS != rc ) {
				debug( _T("COptionsData::_UpdateRunKey: RegSetValueEx failed: %d\n"), rc );
			}
		} else {
			debug( _T("COptionsData::_UpdateRunKey: GetModuleFileNameEx failed: %d\n"), GetLastError( ) );
		}
	} else {
		rc = RegDeleteValue( hk, _T("FreeCompose") );
		if ( ERROR_SUCCESS != rc ) {
			debug( _T("COptionsData::_UpdateRunKey: RegDeleteValue failed: %d\n"), rc );
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

	m_fStartActive       = (BOOL)  theApp.GetProfileInt( _T("Startup"),  _T("StartActive"),        TRUE );
	m_fStartWithWindows  = (BOOL)  theApp.GetProfileInt( _T("Startup"),  _T("StartWithWindows"),   FALSE );

	m_fSwapCapsLock      = (BOOL)  theApp.GetProfileInt( _T("Keyboard"), _T("SwapCapsLock"),       FALSE );

	m_CapsLockToggleMode =
		   (CAPS_LOCK_TOGGLE_MODE) theApp.GetProfileInt( _T("Keyboard"), _T("CapsLockToggleMode"), 
		   (CAPS_LOCK_TOGGLE_MODE) theApp.GetProfileInt( _T("Keyboard"), _T("CapsLockMode"),       CLTM_NORMAL ) );
	m_CapsLockSwapMode   =
			 (CAPS_LOCK_SWAP_MODE) theApp.GetProfileInt( _T("Keyboard"), _T("CapsLockSwapMode"),   CLSM_SWAP );

	m_vkCompose          = (DWORD) theApp.GetProfileInt( _T("Keyboard"), _T("ComposeKey"),         VK_APPS );
	m_vkSwapCapsLock     = (DWORD) theApp.GetProfileInt( _T("Keyboard"), _T("SwapCapsLockKey"),    VK_LCONTROL );

	_FcLoadKeys( );
}

void COptionsData::Save( void ) {
	SaveToXml( );
	_UpdateRunKey( );
}
