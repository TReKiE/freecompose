// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

// Allow use of features specific to Windows XP or later.
#define NTDDI_VERSION  NTDDI_WINXP
#define WINVER         _WIN32_WINNT_WINXP
#define _WIN32_WINNT   _WIN32_WINNT_WINXP
#define _WIN32_IE      _WIN32_IE_IE60

#define NOMINMAX
#define _ATL_APARTMENT_THREADED 
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _SECURE_ATL 1

#include <sdkddkver.h>

#include <afxwin.h>
#include <afxcmn.h>
#include <afxdlgs.h>

#include <atlwin.h>

#include <cstdlib>

#include <FCHookDll.h>

#include "resource.h"

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
