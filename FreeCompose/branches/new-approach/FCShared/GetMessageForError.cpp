#include "stdafx.h"

#include "FCShared.h"
#include "GetMessageForError.h"

wchar_t* GetMessageForError( DWORD const dwError ) {
	wchar_t* buf = nullptr;
	DWORD rc = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, dwError, 0, reinterpret_cast<LPWSTR>( &buf ), 0, nullptr );
	if ( !rc ) {
		debug( L"GetMessageForError: FormatMessage for error code %lu failed, error %lu\n", dwError, GetLastError( ) );
		return nullptr;
	}
	return buf;
}
