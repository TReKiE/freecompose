#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include <winsdkver.h>

#define NTDDI_VERSION  NTDDI_WIN2K
#define WINVER         _WIN32_WINNT_WIN2K
#define _WIN32_WINNT   _WIN32_WINNT_WIN2K
#define _WIN32_IE      _WIN32_IE_IE50

#include <SDKDDKVer.h>
