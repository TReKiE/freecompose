#include "stdafx.h"

DISPOSITION KeyEventHandler::KeyDown( KBDLLHOOKSTRUCT* /*pkb*/ ) {
	return D_NOT_HANDLED;
}

DISPOSITION KeyEventHandler::KeyUp( KBDLLHOOKSTRUCT* /*pkb*/ ) {
	return D_NOT_HANDLED;
}
