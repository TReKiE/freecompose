#include "stdafx.h"

#include <psapi.h>

#include "FreeCompose.h"
#include "OptionsData.h"

#include "ComposeDefaults.h"
#include "Utils.h"

COptionsData::COptionsData( ) {
	m_fStartActive      = (BOOL) theApp.GetProfileInt( _T("Startup"),  _T("StartActive"),      TRUE  );
	m_fStartWithWindows = (BOOL) theApp.GetProfileInt( _T("Startup"),  _T("StartWithWindows"), FALSE );

	m_fSwapCtrlAndCaps  = (BOOL) theApp.GetProfileInt( _T("Keyboard"), _T("SwapCtrlAndCaps"),  FALSE );
	m_fDisableCapsLock  = (BOOL) theApp.GetProfileInt( _T("Keyboard"), _T("DisableCapsLock"),  FALSE );

	_FcLoadKeys( );
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
	for ( int n = 0; n < count; n++ ) {
		section.Format( _T("Mapping\\%d"), n );
		cke.vkFirst     = (DWORD)   theApp.GetProfileInt( section, _T("First"),    0 );
		cke.vkSecond    = (DWORD)   theApp.GetProfileInt( section, _T("Second"),   0 );
		cke.wchComposed = (wchar_t) theApp.GetProfileInt( section, _T("Composed"), 0 );
		m_ComposeKeyEntries[n] = cke;
	}
}

void COptionsData::_FcSaveKeys( void ) {
	theApp.DelRegTree( theApp.GetAppRegistryKey( ), CString( _T("Mapping") ) );

	CString section;
	int count = 0;
	for ( int n = 0; n < (int) m_ComposeKeyEntries.GetSize( ); n++ ) {
		if ( !m_ComposeKeyEntries[n].vkFirst && !m_ComposeKeyEntries[n].vkSecond && !m_ComposeKeyEntries[n].wchComposed ) {
			continue;
		}
		section.Format( _T("Mapping\\%d"), count++ );
		theApp.WriteProfileInt( section, _T("First"),    (int) m_ComposeKeyEntries[n].vkFirst     );
		theApp.WriteProfileInt( section, _T("Second"),   (int) m_ComposeKeyEntries[n].vkSecond    );
		theApp.WriteProfileInt( section, _T("Composed"), (int) m_ComposeKeyEntries[n].wchComposed );
	}

	theApp.WriteProfileInt( _T("Mapping"), _T("Count"), count );
}

void COptionsData::Save( void ) {
	theApp.WriteProfileInt( _T("Startup"), _T("StartActive"),      (int) m_fStartActive );
	theApp.WriteProfileInt( _T("Startup"), _T("StartWithWindows"), (int) m_fStartWithWindows );

	theApp.WriteProfileInt( _T("Keyboard"), _T("SwapCtrlAndCaps"), (int) m_fSwapCtrlAndCaps );
	theApp.WriteProfileInt( _T("Keyboard"), _T("DisableCapsLock"), (int) m_fDisableCapsLock );

	_FcSaveKeys( );

	_UpdateRunKey( );
}

void COptionsData::_UpdateRunKey( void ) {
	LSTATUS rc;
	HKEY hk;
	
	rc = RegOpenKeyEx( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_SET_VALUE, &hk );
	if ( ERROR_SUCCESS != rc ) {
		debug( _T("RegOpenKeyEx failed: %d\n"), rc );
		return;
	}

	if ( m_fStartWithWindows ) {
		TCHAR lpszImageFilename[4096];
		//if ( 0 == GetProcessImageFileName( GetCurrentProcess( ), lpszImageFilename, 4096 ) ) {
		if ( GetModuleFileNameEx( GetCurrentProcess( ), AfxGetApp( )->m_hInstance, lpszImageFilename, 4096 ) > 0 ) {
			rc = RegSetValueEx( hk, _T("Zive Compose"), 0, REG_SZ, (LPBYTE) lpszImageFilename, sizeof(TCHAR) * ( _tcslen( lpszImageFilename ) + 1 ) );
			if ( ERROR_SUCCESS != rc ) {
				debug( _T("RegSetValueEx failed: %d\n"), rc );
			}
		} else {
			debug( _T("GetProcessImageFileName failed: %d\n"), GetLastError( ) );
		}
	} else {
		rc = RegDeleteValue( hk, _T("Zive Compose") );
		if ( ERROR_SUCCESS != rc ) {
			debug( _T("RegDeleteValue failed: %d\n"), rc );
		}
	}

	RegCloseKey( hk );
}
