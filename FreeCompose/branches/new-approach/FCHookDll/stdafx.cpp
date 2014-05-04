#include "stdafx.h"

#include "ComposeSequenceTree.h"
#include "HookProc.h"

//==============================================================================
// Constants
//==============================================================================

namespace Stringify {

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

};

UINT const FCM_PIP = RegisterWindowMessage( L"ca.zive.FcHookDll.FCM_PIP" );
UINT const FCM_KEY = RegisterWindowMessage( L"ca.zive.FcHookDll.FCM_KEY" );

//==============================================================================
// Static class members
//==============================================================================

#if DEBUG_NODES_LEAVES
size_t COMPOSE_TREE_NODE::_cNodes = 0;
size_t COMPOSE_TREE_NODE::_cLeaves = 0;
#endif

//==============================================================================
// Globals
//==============================================================================

AutoCriticalSection        cs;
HHOOK                      hHook                 = nullptr;

ComposeSequence*           ComposeSequences      = nullptr;
INT_PTR                    cComposeSequences     = 0;

COMPOSE_SEQUENCE_TREE      ComposeSequenceTree;

CAPS_LOCK_TOGGLE_MODE      clToggleMode          = CLTM_NORMAL;
CAPS_LOCK_SWAP_MODE        clSwapMode            = CLSM_SWAP;
bool                       fSwapCapsLock         = false;

DWORD                      vkCompose             = VK_APPS;
DWORD                      vkCapsLockSwap        = VK_CAPITAL;

KeyEventHandler*           KeyEventHandlers[256] = { nullptr, };

COMPOSE_STATE              ComposeState          = csNORMAL;

HINSTANCE                  hDllInst              = nullptr;

HWND                       hwndNotifyWindow      = nullptr;
