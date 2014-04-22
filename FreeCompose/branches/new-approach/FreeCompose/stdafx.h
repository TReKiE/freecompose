#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _ATL_APARTMENT_THREADED 
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _SECURE_ATL 1

#include "targetver.h"

#include <cerrno>

#include <algorithm>
#include <map>

#include <afxwin.h>
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxtempl.h>
#include <atlwin.h>

#include <FCHookDll.h>
#include <FCShared.h>

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

#undef _T
#undef _TEXT
#undef min
#undef max
