#include "stdafx.h"

#include "HookProc.h"
#include "Stringify.h"

#include "Key.h"
#include "CapsLock.h"

//==============================================================================
// Global variables
//==============================================================================

extern CapsLockMutator* capsLockMutator;
extern CapsLockToggler* capsLockToggler;

//==============================================================================
// Functions
//==============================================================================

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

//
// Top-level caps lock handling.
//

DISPOSITION CapsLockKeyHandler::_Implementation( KBDLLHOOKSTRUCT* pkb ) {
	bool isKeyDown = Key::isKeyDownEvent( pkb );

	DISPOSITION dMutator = D_NOT_HANDLED;
	if ( capsLockMutator ) {
		if ( isKeyDown ) {
			dMutator = capsLockMutator->KeyDown( pkb );
		} else {
			dMutator = capsLockMutator->KeyUp( pkb );
		}
	}
	debug( L"CapsLockKeyHandler::KeyDown: CapsLock|capsLockMutator=0x%p dMutator=%s\n", capsLockMutator, Stringify::from_DISPOSITION( dMutator ) );

	DISPOSITION dToggler = D_NOT_HANDLED;
	if ( capsLockToggler ) {
		if ( isKeyDown ) {
			dToggler = capsLockToggler->KeyDown( pkb );
		} else {
			dToggler = capsLockToggler->KeyUp( pkb );
		}
	}
	debug( L"CapsLockKeyHandler::KeyDown: CapsLock|capsLockToggler=0x%p dMutator=%s\n", capsLockMutator, Stringify::from_DISPOSITION( dMutator ) );

	// Result of CapsLockToggler takes precedence over CapsLockMutator.
	switch ( dToggler ) {
		case D_NOT_HANDLED:
			break;
	
		case D_ACCEPT_KEY:
		case D_REJECT_KEY:
			return dToggler;
	
		case D_REGENERATE_KEY:
			debug( L"CapsLockKeyHandler::KeyDown: CapsLockToggler returned D_REGENERATE_KEY??\n" );
			DebugBreak( );
			break;
	}

	switch ( dMutator ) {
		case D_NOT_HANDLED:
			break;
	
		case D_ACCEPT_KEY:
			debug( L"CapsLockKeyHandler::KeyDown: CapsLockMutator returned D_ACCEPT_KEY??\n" );
			DebugBreak( );
			break;
	
		case D_REJECT_KEY:
			debug( L"CapsLockKeyHandler::KeyDown: CapsLockMutator returned D_REJECT_KEY??\n" );
			DebugBreak( );
			break;
	
		case D_REGENERATE_KEY:
			return dMutator;
	}

	return D_NOT_HANDLED;
}
