#include "stdafx.h"

#include "FCHookDll.h"
#include "HookProc.h"
#include "Common.h"

#include "Key.h"
#include "KeyEventHandler.h"
#include "CapsLock.h"

//
// Caps Lock mutators.
//

DISPOSITION CapsLockSwapper::KeyDown( KBDLLHOOKSTRUCT* pkb ) {
	if ( Key::isCapsLock( pkb ) ) {
		pkb->vkCode = vkCapsLockSwap;
		return D_REGENERATE_KEY;
	}
	if ( Key::isCapsLockSwap( pkb ) ) {
		pkb->vkCode = VK_CAPITAL;
		return D_REGENERATE_KEY;
	}
	return D_NOT_HANDLED;
}

DISPOSITION CapsLockSwapper::KeyUp( KBDLLHOOKSTRUCT* pkb ) {
	if ( Key::isCapsLock( pkb ) ) {
		pkb->vkCode = vkCapsLockSwap;
		return D_REGENERATE_KEY;
	}
	if ( Key::isCapsLockSwap( pkb ) ) {
		pkb->vkCode = VK_CAPITAL;
		return D_REGENERATE_KEY;
	}
	return D_NOT_HANDLED;
}

DISPOSITION CapsLockReplacer::KeyDown( KBDLLHOOKSTRUCT* pkb ) {
	if ( Key::isCapsLock( pkb ) ) {
		pkb->vkCode = vkCapsLockSwap;
		return D_REGENERATE_KEY;
	}
	return D_NOT_HANDLED;
}

DISPOSITION CapsLockReplacer::KeyUp( KBDLLHOOKSTRUCT* pkb ) {
	if ( Key::isCapsLock( pkb ) ) {
		pkb->vkCode = vkCapsLockSwap;
		return D_REGENERATE_KEY;
	}
	return D_NOT_HANDLED;
}

CapsLockMutator* CapsLockMutatorFactory::Create( CAPS_LOCK_SWAP_MODE clSwapMode ) {
	switch ( clSwapMode ) {
		case CLSM_SWAP:    return new CapsLockReplacer;
		case CLSM_REPLACE: return new CapsLockSwapper;
		default:           return NULL;
	}
}

//
// Caps lock togglers.
//

CapsLockPressTwiceToggler::CapsLockPressTwiceToggler( ) {
	downCount = 0;
	upCount = 0;
}

DISPOSITION CapsLockPressTwiceToggler::KeyDown( KBDLLHOOKSTRUCT* pkb ) {
	if ( ! Key::isCapsLock( pkb ) ) {
		downCount = 0;
		return D_NOT_HANDLED;
	}

	if ( ! downCount ) {
		downCount = 1;
		return D_REJECT_KEY;
	} else {
		downCount = 0;
		return D_ACCEPT_KEY;
	}
}

DISPOSITION CapsLockPressTwiceToggler::KeyUp( KBDLLHOOKSTRUCT* pkb ) {
	if ( ! Key::isCapsLock( pkb ) ) {
		upCount = 0;
		return D_NOT_HANDLED;
	}

	if ( ! upCount ) {
		upCount = 1;
		return D_REJECT_KEY;
	} else {
		upCount = 0;
		return D_ACCEPT_KEY;
	}
}

DISPOSITION CapsLockDisabledToggler::KeyDown( KBDLLHOOKSTRUCT* pkb ) {
	if ( Key::isCapsLock( pkb ) ) {
		return D_REJECT_KEY;
	}
	return D_NOT_HANDLED;
}

DISPOSITION CapsLockDisabledToggler::KeyUp( KBDLLHOOKSTRUCT* pkb ) {
	if ( Key::isCapsLock( pkb ) ) {
		return D_REJECT_KEY;
	}
	return D_NOT_HANDLED;
}

CapsLockToggler* CapsLockTogglerFactory::Create( CAPS_LOCK_TOGGLE_MODE clToggleMode ) {
	switch ( clToggleMode ) {
		case CLTM_PRESSTWICE: return new CapsLockPressTwiceToggler;
		case CLTM_DISABLED:   return new CapsLockDisabledToggler;
		default:              return NULL;
	}	
}
