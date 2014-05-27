#pragma once

#define _ATL_APARTMENT_THREADED 
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _CRTDBG_MAP_ALLOC
#define  NOMINMAX
#define _SECURE_ATL 1
#define  VC_EXTRALEAN
#define  WIN32_LEAN_AND_MEAN

#include "targetver.h"

#include <crtdbg.h>
#include <malloc.h>

#include <cerrno>
#include <cstdlib>
#include <cwchar>
#include <locale>

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include <afxwin.h>
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxtempl.h>

#include <atlbase.h>
#include <atlwin.h>

#include <comdef.h>
#include <comip.h>
#include <comutil.h>
#include <VersionHelpers.h>

#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/brkiter.h>

#include <FCShared.h>
#include <FCHookDll.h>

#include "resource.h"

#undef _T
#undef _TEXT
#undef min
#undef max

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
