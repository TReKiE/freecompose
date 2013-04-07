#pragma once

enum COMPOSE_STATE {
	csNORMAL,
	csCOMPOSE_FIRST,
	csCOMPOSE_SECOND,
	csCAPS_LOCK_FIRST,
};

class KeyEventHandler;

extern CRITICAL_SECTION           cs;
extern HHOOK                      hHook;

extern COMPOSE_SEQUENCE*          ComposeSequences;
extern INT_PTR                    cComposeSequences;

extern CAPS_LOCK_TOGGLE_MODE      clToggleMode;
extern CAPS_LOCK_SWAP_MODE        clSwapMode;
extern bool                       fSwapCapsLock;

extern DWORD                      vkCompose;
extern DWORD                      vkCapsLockSwap;

extern KeyEventHandler*           KeyEventHandlers[256];
extern COMPOSE_STATE              ComposeState;
extern zive::bitset< 256, DWORD > WantedKeys;

extern HINSTANCE                  hDllInst;

int CompareComposeSequences( const void*, const void* );
