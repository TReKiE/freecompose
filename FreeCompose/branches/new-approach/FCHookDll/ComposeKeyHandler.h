#pragma once

#include <Bitset.h>

class ComposeKeyHandler: public KeyEventHandler {
public:

	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* /*pkb*/ ) {
		WantedKeys.Add( vkCompose );
		ComposeState = csCOMPOSE_FIRST;
		return D_REJECT_KEY;
	}

	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* /*pkb*/ ) {
		return D_NOT_HANDLED;
	}

};
