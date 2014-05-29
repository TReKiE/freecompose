#include "stdafx.h"

#include <initguid.h>

#include "FreeCompose.h"
#include "SoundPlayer.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

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

//
// The one and only CWinApp derivative, CFreeComposeApp
//

CFreeComposeApp theApp;

//
// Tone player
//

SoundPlayer* g_pTonePlayer = nullptr;

//
// Composition sound names
//

wchar_t const* CompositionSoundNames[] = {
	L"FC_Composition_Starting",
	L"FC_Composition_KeyPressed",
	L"FC_Composition_Succeeded",
	L"FC_Composition_Failed",
	L"FC_Composition_Cancelled",
	L"FC_Composition_Escape",
};
