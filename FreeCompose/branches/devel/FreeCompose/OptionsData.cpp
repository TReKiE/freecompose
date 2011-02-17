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
	m_ComposeKeyEntries.RemoveAll( );
}

COptionsData& COptionsData::operator=( const COptionsData& options ) {
	m_fStartActive      = options.m_fStartActive;
	m_fStartWithWindows = options.m_fStartWithWindows;

	m_fSwapCtrlAndCaps  = options.m_fSwapCtrlAndCaps;
	m_fDisableCapsLock  = options.m_fDisableCapsLock;

	m_ComposeKeyEntries.RemoveAll( );
	m_ComposeKeyEntries.Copy( options.m_ComposeKeyEntries );

	return *this;
}

bool COptionsData::operator==( const COptionsData& options ) {
	if ( m_fStartActive      != options.m_fStartActive      ) return false;
	if ( m_fStartWithWindows != options.m_fStartWithWindows ) return false;
	if ( m_fSwapCtrlAndCaps  != options.m_fSwapCtrlAndCaps  ) return false;
	if ( m_fDisableCapsLock  != options.m_fDisableCapsLock  ) return false;

	if ( m_ComposeKeyEntries.GetCount( ) != options.m_ComposeKeyEntries.GetCount( ) )
		return false;
	for ( int n = 0; n < m_ComposeKeyEntries.GetCount( ); n++ ) {
		if ( m_ComposeKeyEntries[n] != options.m_ComposeKeyEntries[n] )
			return false;
	}

	return true;
}

bool COptionsData::operator!=( const COptionsData& options ) {
	return ! operator==( options );
}

bool COptionsData::_FcValidateCke( const COMPOSE_KEY_ENTRY& cke ) {
	if ( 0 == ( cke.vkFirst &  0xFFUL ) )
		return false;
	if ( 0 != ( cke.vkFirst & ~0x800000FFUL ) )
		return false;

	if ( 0 == ( cke.vkSecond &  0xFFUL ) )
		return false;
	if ( 0 != ( cke.vkSecond & ~0x800000FFUL ) )
		return false;

	if ( 0 == cke.u32Composed )
		return false;
	if ( cke.u32Composed >= 0xD800U && cke.u32Composed < 0xE000U )
		return false;
	if ( cke.u32Composed > 0x10FFFFU )
		return false;

	return true;
}

void COptionsData::_FcLoadKeys( void ) {
	m_ComposeKeyEntries.RemoveAll( );

	int count = theApp.GetProfileInt( _T("Mapping"), _T("Count"), 0 );
	if ( count < 1 ) {
		count = _countof( DefaultComposeKeyEntries );
		m_ComposeKeyEntries.SetSize( count );
		for ( int n = 0; n < count; n++ ) {
			m_ComposeKeyEntries[n] = DefaultComposeKeyEntries[n];
		}
		return;
	}

	COMPOSE_KEY_ENTRY cke;
	CString section;
	m_ComposeKeyEntries.SetSize( count );
	int index = 0;
	for ( int n = 0; n < count; n++ ) {
		section.Format( _T("Mapping\\%d"), n );
		cke.vkFirst     = (DWORD)    theApp.GetProfileInt( section, _T("First"),    0 );
		cke.vkSecond    = (DWORD)    theApp.GetProfileInt( section, _T("Second"),   0 );
		cke.u32Composed = (unsigned) theApp.GetProfileInt( section, _T("Composed"), 0 );
		if ( _FcValidateCke( cke ) ) {
			m_ComposeKeyEntries[index++] = cke;
		} else {
			debug( _T("Bad mapping, #%d\n"), n );
		}
	}
}

void COptionsData::_FcSaveKeys( void ) {
	theApp.DelRegTree( theApp.GetAppRegistryKey( ), CString( _T("Mapping") ) );

	CString section;
	int count = 0;
	for ( int n = 0; n < (int) m_ComposeKeyEntries.GetSize( ); n++ ) {
		if ( !m_ComposeKeyEntries[n].vkFirst && !m_ComposeKeyEntries[n].vkSecond && !m_ComposeKeyEntries[n].u32Composed ) {
			continue;
		}
		section.Format( _T("Mapping\\%d"), count++ );
		theApp.WriteProfileInt( section, _T("First"),    (int) m_ComposeKeyEntries[n].vkFirst     );
		theApp.WriteProfileInt( section, _T("Second"),   (int) m_ComposeKeyEntries[n].vkSecond    );
		theApp.WriteProfileInt( section, _T("Composed"), (int) m_ComposeKeyEntries[n].u32Composed );
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
		TCHAR lpszImageFilename[4096];
		if ( GetModuleFileNameEx( GetCurrentProcess( ), AfxGetApp( )->m_hInstance, lpszImageFilename, 4096 ) > 0 ) {
			rc = RegSetValueEx( hk, _T("Zive Compose"), 0, REG_SZ, (LPBYTE) lpszImageFilename, sizeof(TCHAR) * ( _tcslen( lpszImageFilename ) + 1 ) );
			if ( ERROR_SUCCESS != rc ) {
				debug( _T("COptionsData::_UpdateRunKey: RegSetValueEx failed: %d\n"), rc );
			}
		} else {
			debug( _T("COptionsData::_UpdateRunKey: GetModuleFileNameEx failed: %d\n"), GetLastError( ) );
		}
	} else {
		rc = RegDeleteValue( hk, _T("Zive Compose") );
		if ( ERROR_SUCCESS != rc ) {
			debug( _T("COptionsData::_UpdateRunKey: RegDeleteValue failed: %d\n"), rc );
		}
	}

	RegCloseKey( hk );
}

void COptionsData::Load( void ) {
	m_fStartActive      = (BOOL) theApp.GetProfileInt( _T("Startup"),  _T("StartActive"),      TRUE  );
	m_fStartWithWindows = (BOOL) theApp.GetProfileInt( _T("Startup"),  _T("StartWithWindows"), FALSE );

	m_fSwapCtrlAndCaps  = (BOOL) theApp.GetProfileInt( _T("Keyboard"), _T("SwapCtrlAndCaps"),  FALSE );
	m_fDisableCapsLock  = (BOOL) theApp.GetProfileInt( _T("Keyboard"), _T("DisableCapsLock"),  FALSE );

	_FcLoadKeys( );
}

void COptionsData::Save( void ) {
	theApp.WriteProfileInt( _T("Startup"), _T("StartActive"),      (int) m_fStartActive );
	theApp.WriteProfileInt( _T("Startup"), _T("StartWithWindows"), (int) m_fStartWithWindows );

	theApp.WriteProfileInt( _T("Keyboard"), _T("SwapCtrlAndCaps"), (int) m_fSwapCtrlAndCaps );
	theApp.WriteProfileInt( _T("Keyboard"), _T("DisableCapsLock"), (int) m_fDisableCapsLock );

	_FcSaveKeys( );

	_UpdateRunKey( );
}
