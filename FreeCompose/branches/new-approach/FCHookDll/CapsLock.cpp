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
	return KeyDown( pkb );
}

DISPOSITION CapsLockReplacer::KeyDown( KBDLLHOOKSTRUCT* pkb ) {
	if ( Key::isCapsLock( pkb ) ) {
		pkb->vkCode = vkCapsLockSwap;
		return D_REGENERATE_KEY;
	}
	return D_NOT_HANDLED;
}

DISPOSITION CapsLockReplacer::KeyUp( KBDLLHOOKSTRUCT* pkb ) {
	return KeyDown( pkb );
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
	allowDown = false;
	allowUp = false;
}

DISPOSITION CapsLockPressTwiceToggler::_Implementation( KBDLLHOOKSTRUCT* pkb, bool& allow ) {
	if ( ! Key::isCapsLock( pkb ) ) {
		allow = false;
		return D_NOT_HANDLED;
	}

	DISPOSITION d = allow ? D_ACCEPT_KEY : D_REJECT_KEY;
	allow = !allow;
	return d;
}

DISPOSITION CapsLockPressTwiceToggler::KeyDown( KBDLLHOOKSTRUCT* pkb ) {
	return _Implementation( pkb, allowDown );
}

DISPOSITION CapsLockPressTwiceToggler::KeyUp( KBDLLHOOKSTRUCT* pkb ) {
	return _Implementation( pkb, allowUp );
}

DISPOSITION CapsLockDisabledToggler::KeyDown( KBDLLHOOKSTRUCT* pkb ) {
	return Key::isCapsLock( pkb ) ? D_REJECT_KEY : D_NOT_HANDLED;
}

DISPOSITION CapsLockDisabledToggler::KeyUp( KBDLLHOOKSTRUCT* pkb ) {
	return KeyDown( pkb );
}

CapsLockToggler* CapsLockTogglerFactory::Create( CAPS_LOCK_TOGGLE_MODE clToggleMode ) {
	switch ( clToggleMode ) {
		case CLTM_PRESSTWICE: return new CapsLockPressTwiceToggler;
		case CLTM_DISABLED:   return new CapsLockDisabledToggler;
		default:              return NULL;
	}	
}
