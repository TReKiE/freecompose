#pragma once

#include <Bitset.h>

class ComposeKeyHandler: public KeyEventHandler {
public:

	ComposeKeyHandler( ) { }

	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* /*pkb*/ ) {
		WantedKeys.Add( vkCompose );
		return D_REJECT_KEY;
	}

	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* /*pkb*/ ) {
		return D_NOT_HANDLED;
	}

};
