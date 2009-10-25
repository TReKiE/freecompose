#include "stdafx.h"
#include "Utils.h"

void debug( LPCTSTR lpsz, ... ) {
	TCHAR szbuf[1024];
	va_list va;

	va_start( va, lpsz );
	_vsntprintf_s( szbuf, 1024, _TRUNCATE, lpsz, va );
	va_end( va );
	OutputDebugString( szbuf );
}
