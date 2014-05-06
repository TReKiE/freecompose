#include "stdafx.h"

#include <initguid.h>

#include "FreeCompose.h"

//
// Registered window messages
//

UINT const APP_ACTIVATE    = RegisterWindowMessage( L"ca.zive.FreeCompose.APP_ACTIVATE" );
UINT const APP_NOTIFYICON  = RegisterWindowMessage( L"ca.zive.FreeCompose.APP_NOTIFYICON" );
UINT const APP_RECONFIGURE = RegisterWindowMessage( L"ca.zive.FreeCompose.APP_RECONFIGURE" );

UINT const FCM_KEY         = RegisterWindowMessage( L"ca.zive.FcHookDll.FCM_KEY" );
UINT const FCM_PIP         = RegisterWindowMessage( L"ca.zive.FcHookDll.FCM_PIP" );

//
// Common Controls (COMCTL32) version
//

DWORD g_CommonControlsApiVersion = 0;

unsigned g_CommonControlsDllMajorVersion = 0;
unsigned g_CommonControlsDllMinorVersion = 0;
unsigned g_CommonControlsDllBuildNumber = 0;
unsigned g_CommonControlsDllQfeNumber = 0;
