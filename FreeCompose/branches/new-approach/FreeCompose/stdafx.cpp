#include "stdafx.h"

#include <initguid.h>

#include "FreeCompose.h"
#include "FcAppSoundsRegistry.h"
#include "FcCommandLineInfo.h"
#include "SoundPlayer.h"
#include "StringMapper.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

//
// The one and only CWinApp derivative, CFreeComposeApp
//

CFreeComposeApp theApp;

//
// Source of configuration information
//

ConfigurationSource g_ConfigurationSource = ConfigurationSource::User;

//
// Registered window messages
//

UINT const APP_ACTIVATE          = RegisterWindowMessage( L"ca.zive.FreeCompose.APP_ACTIVATE" );
UINT const APP_NOTIFYICON        = RegisterWindowMessage( L"ca.zive.FreeCompose.APP_NOTIFYICON" );
UINT const APP_RECONFIGURE       = RegisterWindowMessage( L"ca.zive.FreeCompose.APP_RECONFIGURE" );

UINT const FCM_KEY               = RegisterWindowMessage( L"ca.zive.FcHookDll.FCM_KEY" );
UINT const FCM_COMPOSITION_SOUND = RegisterWindowMessage( L"ca.zive.FcHookDll.FCM_COMPOSITION_SOUND" );

//
// Common Controls (COMCTL32) version
//

DWORD g_CommonControlsApiVersion = 0;

unsigned g_CommonControlsDllMajorVersion = 0;
unsigned g_CommonControlsDllMinorVersion = 0;
unsigned g_CommonControlsDllBuildNumber = 0;
unsigned g_CommonControlsDllQfeNumber = 0;

//
// Composition sound names
//

wchar_t const* ApplicationSoundNames[NumberOfApplicationSounds] = {
	L"FC_Composition_Starting",
	L"FC_Composition_KeyPressed",
	L"FC_Composition_Succeeded",
	L"FC_Composition_Failed",
	L"FC_Composition_Cancelled",
	L"FC_Composition_Escape",
	L"FC_Application_Activated",
	L"FC_Application_Deactivated",
};

int const ApplicationSoundDisplayNameIds[NumberOfApplicationSounds] = {
	IDS_APPSOUND_COMPOSITION_STARTING,
	IDS_APPSOUND_COMPOSITION_KEYPRESSED,
	IDS_APPSOUND_COMPOSITION_SUCCEEDED,
	IDS_APPSOUND_COMPOSITION_FAILED,
	IDS_APPSOUND_COMPOSITION_CANCELLED,
	IDS_APPSOUND_COMPOSITION_ESCAPE,
	IDS_APPSOUND_APPLICATION_ACTIVATED,
	IDS_APPSOUND_APPLICATION_DEACTIVATED,
};

//
// Sound player object
//

SoundPlayer* g_pSoundPlayer;

//
// StringMappers
//

extern StringMapper<bool> const BoolStringMapper {
	L"false",
	L"true",
};

extern StringMapper<CAPS_LOCK_TOGGLE_MODE> const CltmStringMapper {
	static_cast<LPCWSTR>( nullptr ),
	L"normal",
	L"pressTwice",
	L"disabled",
};

extern StringMapper<CAPS_LOCK_SWAP_MODE> const ClsmStringMapper {
	static_cast<LPCWSTR>( nullptr ),
	L"normal",
	L"swap",
	L"replace",
};
