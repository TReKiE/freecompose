#pragma once

#include <winsdkver.h>

// Target Windows XP or later.
// (Support for Windows 2000 is a casualty of MFC no longer supporting it.)

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#define NTDDI_VERSION NTDDI_WINXP
#define WINVER        _WIN32_WINNT_WINXP
#define _WIN32_IE     _WIN32_IE_WINXP

#include <SDKDDKVer.h>
