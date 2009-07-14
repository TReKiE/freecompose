#include "stdafx.h"
#include "FCHookDll.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void debug( LPCTSTR lpszMessage, ... ) {
	TCHAR buf[4096];

	va_list va;
	va_start( va, lpszMessage );
	_vsntprintf_s( buf, countof( buf ), _TRUNCATE, lpszMessage, va );
	va_end( va );
	OutputDebugString( buf );
}
