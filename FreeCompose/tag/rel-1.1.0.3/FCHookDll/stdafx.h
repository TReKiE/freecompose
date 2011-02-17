// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

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


#include <windows.h>

#include <stdarg.h>
#include <wtypes.h>
#include <tchar.h>

#include <set>

using namespace std;
