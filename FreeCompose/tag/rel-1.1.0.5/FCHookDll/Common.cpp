#include "stdafx.h"
#include "FCHookDll.h"
#include "HookProc.h"
#include "Common.h"

void debug( LPCWSTR lpszMessage, ... ) {
	wchar_t buf[1024];

	va_list va;
	va_start( va, lpszMessage );
	_vsnwprintf_s( buf, countof( buf ), _TRUNCATE, lpszMessage, va );
	va_end( va );
	OutputDebugString( buf );
}

int CompareCkes( const void* pv1, const void* pv2 ) {
	COMPOSE_KEY_ENTRY* pcke1 = (COMPOSE_KEY_ENTRY*) pv1;
	COMPOSE_KEY_ENTRY* pcke2 = (COMPOSE_KEY_ENTRY*) pv2;

	if ( pcke1->vkFirst < pcke2->vkFirst ) {
		return -1;
	} else if ( pcke1->vkFirst == pcke2->vkFirst ) {
		if ( pcke1->vkSecond < pcke2->vkSecond ) {
			return -1;
		} else if ( pcke1->vkSecond == pcke2->vkSecond ) {
			return 0;
		} else {
			return 1;
		}
	} else {
		return 1;
	}
}
