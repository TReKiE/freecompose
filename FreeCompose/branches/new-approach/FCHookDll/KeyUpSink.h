#pragma once

class KeyUpSink: public KeyEventHandler {
public:
	KeyUpSink( DWORD const vkCode_, KeyEventHandler* replacement_ ): vkCode( vkCode_ ), replacement( replacement_ ) { }

	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* /*pkb*/ ) {
		return D_NOT_HANDLED;
	}

	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* pkb ) {
		if ( vkCode == pkb->vkCode ) {
			KeyEventHandlers[ vkCode ] = replacement;
			delete this;
			return D_REJECT_KEY;
		}
		return D_NOT_HANDLED;
	}

private:
	DWORD vkCode;
	KeyEventHandler* replacement;
};
