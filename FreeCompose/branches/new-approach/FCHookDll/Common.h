#pragma once
#pragma warning( disable: 4127 )

#pragma data_seg( push, ".shareddata" )
__declspec(selectany) CRITICAL_SECTION      cs;
__declspec(selectany) HHOOK                 hHook              = NULL;

__declspec(selectany) COMPOSE_SEQUENCE*     ComposeSequences   = NULL;
__declspec(selectany) INT_PTR               cComposeSequences  = 0;

__declspec(selectany) CAPS_LOCK_TOGGLE_MODE clToggleMode       = CLTM_NORMAL;
__declspec(selectany) CAPS_LOCK_SWAP_MODE   clSwapMode         = CLSM_SWAP;
__declspec(selectany) bool                  fSwapCapsLock      = false;

__declspec(selectany) DWORD                 vkCompose          = VK_APPS;
__declspec(selectany) DWORD                 vkCapsLockSwap     = VK_CAPITAL;
#pragma data_seg( pop )

int CompareComposeSequences( const void*, const void* );
void SetUpCapsLockHandling( void );
