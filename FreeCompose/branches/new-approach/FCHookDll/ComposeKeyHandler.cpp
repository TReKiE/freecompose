#include "stdafx.h"

#include "FCHookDll.h"
#include "Common.h"
#include "KeyEventHandler.h"
#include "ComposeKeyHandler.h"

DISPOSITION ComposeKeyHandler::KeyDown( KBDLLHOOKSTRUCT* /*pkb*/ ) {
	switch ( ComposeState ) {
		case csNORMAL:
			ComposeState = csCOMPOSE_FIRST;
			WantedKeys.Add( vkCompose );
			if ( hwndNotifyWindow ) {
				::PostMessage( hwndNotifyWindow, FCM_PIP, PIP_OK_1, 0 );
			}
			return D_REJECT_KEY;

		case csCOMPOSE_FIRST:
			ComposeState = csNORMAL;
			WantedKeys.Remove( vkCompose );
			if ( hwndNotifyWindow ) {
				::PostMessage( hwndNotifyWindow, FCM_PIP, PIP_ABORT, 0 );
			}
			return D_ACCEPT_KEY;
	}

	return D_NOT_HANDLED;
}

DISPOSITION ComposeKeyHandler::KeyUp( KBDLLHOOKSTRUCT* /*pkb*/ ) {
	return D_NOT_HANDLED;
}
