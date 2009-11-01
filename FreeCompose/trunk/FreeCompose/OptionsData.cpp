#include "StdAfx.h"

#include "FreeCompose.h"
#include "OptionsData.h"
#include "Utils.h"

COptionsData::COptionsData( ) {
	m_fStartActive      = (BOOL) theApp.GetProfileInt( _T("Startup"),  _T("StartActive"),      TRUE  );
	m_fStartWithWindows = (BOOL) theApp.GetProfileInt( _T("Startup"),  _T("StartWithWindows"), FALSE );

	m_fSwapCtrlAndCaps  = (BOOL) theApp.GetProfileInt( _T("Keyboard"), _T("SwapCtrlAndCaps"),  FALSE );
	m_fDisableCapsLock  = (BOOL) theApp.GetProfileInt( _T("Keyboard"), _T("DisableCapsLock"),  FALSE );
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
