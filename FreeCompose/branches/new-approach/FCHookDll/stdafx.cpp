#include "stdafx.h"

#include "FCHookDll.h"
#include "KeyEventHandler.h"
#include "HookProc.h"
#include "Common.h"

//==============================================================================
// Constants
//==============================================================================

wchar_t const* stringsFor_bool[] = {
	L"false",
	L"TRUE"
};

wchar_t const* stringsFor_enum_DISPOSITION[] = {
	L"dNOT_HANDLED",
	L"dACCEPT_KEY",
	L"dREJECT_KEY",
	L"dREGENERATE_KEY",
};

wchar_t const* stringsFor_enum_COMPOSE_STATE[] = {
	L"csNORMAL",
	L"csCOMPOSE_FIRST",
	L"csCOMPOSE_SECOND",
	L"csCAPS_LOCK_FIRST",
};

//==============================================================================
// Globals
//==============================================================================

#pragma data_seg( push, ".shareddata" )

CRITICAL_SECTION           cs;
HHOOK                      hHook                 = NULL;

COMPOSE_SEQUENCE*          ComposeSequences      = NULL;
INT_PTR                    cComposeSequences     = 0;

CAPS_LOCK_TOGGLE_MODE      clToggleMode          = CLTM_NORMAL;
CAPS_LOCK_SWAP_MODE        clSwapMode            = CLSM_SWAP;
bool                       fSwapCapsLock         = false;

DWORD                      vkCompose             = VK_APPS;
DWORD                      vkCapsLockSwap        = VK_CAPITAL;

KeyEventHandler*           KeyEventHandlers[256] = { NULL, };

zive::bitset< 256, DWORD > WantedKeys;

COMPOSE_STATE              ComposeState          = csNORMAL;

HINSTANCE                  hDllInst              = NULL;

#pragma data_seg( pop )
