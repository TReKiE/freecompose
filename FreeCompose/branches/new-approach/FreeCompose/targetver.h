#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include <winsdkver.h>

// Target Windows XP or later.
// (We still support Windows 2000, but we need to target XP in order to make some #defines available.)
// Let's see if the above comment about #defines is actually the case...

#define _WIN32_WINNT  _WIN32_WINNT_WIN2K
#define NTDDI_VERSION NTDDI_WIN2K
#define WINVER        _WIN32_WINNT_WIN2K
#define _WIN32_IE     _WIN32_IE_WIN2K

//#define _WIN32_WINNT _WIN32_WINNT_WINXP
//#define NTDDI_VERSION NTDDI_WINXP
//#define WINVER        _WIN32_WINNT_WINXP
//#define _WIN32_IE     _WIN32_IE_WINXP

#include <SDKDDKVer.h>
