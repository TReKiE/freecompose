#pragma once

#define countof(x) ( sizeof((x)) / sizeof((x)[0]) )

#pragma data_seg( push, ".shareddata" )
__declspec(selectany) CRITICAL_SECTION cs;
__declspec(selectany) COMPOSE_KEY_ENTRY* ComposeKeyEntries = NULL;
__declspec(selectany) int cComposeKeyEntries = 0;
__declspec(selectany) HHOOK hHook = NULL;
#pragma data_seg( pop )

void debug( LPCTSTR lpszMessage, ... );
