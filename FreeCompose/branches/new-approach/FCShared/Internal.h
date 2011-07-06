#pragma once

// stolen from afxres.h:
// for application title (defaults to EXE name or name in constructor)
#define AFX_IDS_APP_TITLE               0xE000
// company name
#define AFX_IDS_COMPANY_NAME            0xE004

LPWSTR _CoTaskMemDuplicateStringW( LPCWSTR lpwszSource );
bool _GetAppDataFolder( LPWSTR& lpsz );
bool _GetAppDataFolderFromShell( LPWSTR& lpsz );
bool _GetAppDataFolderFromKfm( LPWSTR& lpsz );

extern HINSTANCE hDllInst;
