#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include <winsdkver.h>

// Target Windows XP or later.
// (Support for Windows 2000 is a casualty of MFC no longer supporting it.)

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#define NTDDI_VERSION NTDDI_WINXP
#define WINVER        _WIN32_WINNT_WINXP
#define _WIN32_IE     _WIN32_IE_WINXP

#include <SDKDDKVer.h>
