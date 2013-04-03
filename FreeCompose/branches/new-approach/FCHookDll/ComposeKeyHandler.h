#pragma once

class ComposeKeyHandler: public KeyEventHandler {
public:
	ComposeKeyHandler( ) { }

	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* pkb ) {
		if ( !Key::isCompose( pkb ) ) {
			return D_NOT_HANDLED;
		}

		keyEventHandler[ vkCompose ] = new KeyUpSink( vkCompose, this );
		return D_REJECT_KEY;
	}

	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* pkb ) {
		if ( !Key::isCompose( pkb ) ) {
			return D_NOT_HANDLED;
		}

		return D_NOT_HANDLED;
	}
};
