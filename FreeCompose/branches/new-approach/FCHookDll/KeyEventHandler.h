#pragma once

enum DISPOSITION {
	D_NOT_HANDLED,
	D_ACCEPT_KEY,
	D_REJECT_KEY,
	D_REGENERATE_KEY,
};

class KeyEventHandler {
public:
	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* pkb ) = 0;
	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* pkb ) = 0;
};
