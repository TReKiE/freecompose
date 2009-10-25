#pragma once

#define countof(x) ( sizeof((x)) / sizeof((x)[0]) )

#define LOCK(x) EnterCriticalSection(&(x));
#define UNLOCK(x) LeaveCriticalSection(&(x));

#pragma data_seg( push, ".shareddata" )
__declspec(selectany) CRITICAL_SECTION   cs;
__declspec(selectany) COMPOSE_KEY_ENTRY* ComposeKeyEntries  = NULL;
__declspec(selectany) int                cComposeKeyEntries = 0;
__declspec(selectany) HHOOK              hHook              = NULL;
__declspec(selectany) volatile bool      fDisableCapsLock   = false;
#pragma data_seg( pop )

void debug( LPCTSTR lpszMessage, ... );
int CompareCkes( const void*, const void* );
