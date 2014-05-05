#pragma once

#include <winsdkver.h>

// Target Windows XP or later.
// (Support for Windows 2000 is a casualty of MFC no longer supporting it.)

#define _WIN32_WINNT  _WIN32_WINNT_WINXP
#define NTDDI_VERSION NTDDI_WINXP
#define WINVER        _WIN32_WINNT_WINXP
#define _WIN32_IE     _WIN32_IE_WINXP

#include <SDKDDKVer.h>

unsigned short const WindowsMajorVersion     = static_cast<unsigned short>( ( NTDDI_VERSION >> 24 ) & 0x000000FF );
unsigned short const WindowsMinorVersion     = static_cast<unsigned short>( ( NTDDI_VERSION >> 16 ) & 0x000000FF );
unsigned short const WindowsServicePackMajor = static_cast<unsigned short>( ( NTDDI_VERSION >>  8 ) & 0x000000FF );
