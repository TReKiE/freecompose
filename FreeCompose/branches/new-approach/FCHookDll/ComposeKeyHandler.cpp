#include "stdafx.h"

#include "FCHookDll.h"
#include "Common.h"
#include "KeyEventHandler.h"
#include "ComposeKeyHandler.h"

DISPOSITION ComposeKeyHandler::KeyDown( KBDLLHOOKSTRUCT* /*pkb*/ ) {
	WantedKeys.Add( vkCompose );
	ComposeState = csCOMPOSE_FIRST;
	return D_REJECT_KEY;
}

DISPOSITION ComposeKeyHandler::KeyUp( KBDLLHOOKSTRUCT* /*pkb*/ ) {
	return D_NOT_HANDLED;
}
