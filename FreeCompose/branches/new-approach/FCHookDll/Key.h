#pragma once

#include <KeyIsXAlnum.h>

class Key {
public:
	inline static bool isCapsLock( KBDLLHOOKSTRUCT const* pkb ) {
		return pkb->vkCode == VK_CAPITAL;
	}

	inline static bool isCapsLockSwap( KBDLLHOOKSTRUCT const* pkb ) {
		return pkb->vkCode == vkCapsLockSwap;
	}

	inline static bool isCompose( KBDLLHOOKSTRUCT const* pkb ) {
		return pkb->vkCode == vkCompose;
	}

	inline static bool isExtended( KBDLLHOOKSTRUCT const* pkb ) {
		return 0 != ( pkb->flags & LLKHF_EXTENDED );
	}

	inline static bool isInjected( KBDLLHOOKSTRUCT const* pkb ) {
		return 0 != ( pkb->flags & LLKHF_INJECTED );
	}

	inline static bool isKeyDownEvent( KBDLLHOOKSTRUCT const* pkb ) {
		return 0 == ( pkb->flags & LLKHF_UP );
	}

	inline static bool isKeyUpEvent( KBDLLHOOKSTRUCT const* pkb ) {
		return 0 != ( pkb->flags & LLKHF_UP );
	}

private:
	Key( );
	Key( Key& );
	Key& operator=( Key& );
};
