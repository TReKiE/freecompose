#pragma once

#define _ATL_APARTMENT_THREADED 
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _CRTDBG_MAP_ALLOC
#define  NOMINMAX
#define _SECURE_ATL 1
#define  VC_EXTRALEAN
#define  WIN32_LEAN_AND_MEAN

#include "targetver.h"

// C standard library
#include <crtdbg.h>
#include <malloc.h>

// C++ version of C standard library
#include <cerrno>
#include <cstdlib>
#include <cwchar>

// C++ Standard Template Library
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <locale>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <tuple>	
#include <typeinfo>
#include <vector>

// Microsoft Foundation Classes
#include <afxwin.h>
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxtempl.h>
#include <afxdialogex.h>

// Active Template Library
#include <atlbase.h>
#include <atlwin.h>

// Windows SDK
#include <comdef.h>
#include <comip.h>
#include <comutil.h>
#include <mmsystem.h>
#include <VersionHelpers.h>

// International Components for Unicode
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/brkiter.h>

// FCShared
#include <FCShared.h>
#include <EnumTraits.h>

// FCHookDll
#include <FCHookDll.h>

// FreeCompose resources
#include "resource.h"

#undef _T
#undef _TEXT
#undef PlaySound

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
