// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define _ATL_APARTMENT_THREADED 
#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.

#undef NTDDI_VERSION
#undef WINVER
#undef _WIN32_IE
#undef _WIN32_WINDOWS
#undef _WIN32_WINNT

// Allow use of features specific to Windows XP or later.
#define NTDDI_VERSION NTDDI_WINXP
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501

// _WIN32_WINDOWS and _WIN32_IE are set low on purpose
// MFC requires _WIN32_WINDOWS >= 0x0400
// Common Controls on XP requires _WIN32_IE >= 0x0500
#define _WIN32_WINDOWS 0x0400
#define _WIN32_IE 0x0500


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>
#include <afxdlgs.h>

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

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>

#include <limits.h>

#include <FCHookDll.h>

#include "resource.h"

#define countof(x) ( sizeof((x)) / sizeof((x)[0]) )
