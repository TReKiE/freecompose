#include "stdafx.h"

#include "HookProc.h"

//==============================================================================
// Constants
//==============================================================================

wchar_t const* stringsFor_bool[] = {
	L"false",
	L"TRUE"
};

wchar_t const* stringsFor_enum_DISPOSITION[] = {
	L"D_NOT_HANDLED",
	L"D_ACCEPT_KEY",
	L"D_REJECT_KEY",
	L"D_REGENERATE_KEY",
};

wchar_t const* stringsFor_enum_COMPOSE_STATE[] = {
	L"csNORMAL",
	L"csCOMPOSE",
	L"csCAPS_LOCK",
};

UINT const FCM_PIP = RegisterWindowMessage( L"FcHookDll.FCM_PIP" );
UINT const FCM_KEY = RegisterWindowMessage( L"FcHookDll.FCM_KEY" );

//==============================================================================
// Globals
//==============================================================================

#pragma data_seg( push, ".shareddata" )

AutoCriticalSection        cs;
HHOOK                      hHook                 = NULL;

COMPOSE_SEQUENCE*          ComposeSequences      = NULL;
INT_PTR                    cComposeSequences     = 0;

COMPOSE_SEQUENCE_TREE      ComposeSequenceTree;

CAPS_LOCK_TOGGLE_MODE      clToggleMode          = CLTM_NORMAL;
CAPS_LOCK_SWAP_MODE        clSwapMode            = CLSM_SWAP;
bool                       fSwapCapsLock         = false;

DWORD                      vkCompose             = VK_APPS;
DWORD                      vkCapsLockSwap        = VK_CAPITAL;

KeyEventHandler*           KeyEventHandlers[256] = { NULL, };

COMPOSE_STATE              ComposeState          = csNORMAL;

HINSTANCE                  hDllInst              = NULL;

HWND                       hwndNotifyWindow      = NULL;

#pragma data_seg( pop )
