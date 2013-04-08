#pragma once

class ComposeKeyHandler: public KeyEventHandler {
public:

	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* /*pkb*/ );
	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* /*pkb*/ );

};
