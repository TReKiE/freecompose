#include "stdafx.h"

#include "FCHookDll.h"
#include "Common.h"
#include "KeyEventHandler.h"

DISPOSITION KeyEventHandler::KeyDown( KBDLLHOOKSTRUCT* /*pkb*/ ) {
	return D_NOT_HANDLED;
}

DISPOSITION KeyEventHandler::KeyUp( KBDLLHOOKSTRUCT* /*pkb*/ ) {
	return D_NOT_HANDLED;
}
