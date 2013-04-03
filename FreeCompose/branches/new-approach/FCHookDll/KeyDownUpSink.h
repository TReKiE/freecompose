#pragma once

class KeyDownUpSink: public KeyEventHandler {
public:
	KeyDownUpSink( DWORD const vkCode_, KeyEventHandler* replacement_ ): vkCode( vkCode_ ), replacement( replacement_ ) { }

	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* pkb ) {
		return _Implementation( pkb );
	}

	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* pkb ) {
		DISPOSITION d = _Implementation( pkb );
		if ( D_REJECT_KEY == d ) {
			keyEventHandler[ vkCode ] = replacement;
			delete this;
		}
		return d;
	}

private:
	DWORD vkCode;
	KeyEventHandler* replacement;

	DISPOSITION _Implementation( KBDLLHOOKSTRUCT* pkb ) {
		if ( vkCode == pkb->vkCode ) {
			return D_REJECT_KEY;
		}
		return D_NOT_HANDLED;
	}
};
