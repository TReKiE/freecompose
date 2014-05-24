#include "stdafx.h"

#include "Stringify.h"
#include "KeyUpSink.h"
#include "ComposeKeyHandler.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

DISPOSITION ComposeKeyHandler::KeyDown( KBDLLHOOKSTRUCT* pkb ) {
	debug( L"ComposeKeyHandler@0x%p::KeyDown: pkb=0x%p, ComposeState=%s(%d), KeyEventHandlers[vkCompose(%lu)]=0x%p\n", this, pkb, Stringify::from_COMPOSE_STATE( ComposeState ), ComposeState, vkCompose, KeyEventHandlers[ vkCompose ] );

	KeyEventHandler* keh = KeyEventHandlers[ vkCompose ];
	COMPOSE_STATE oldComposeState = ComposeState;
	
	DISPOSITION result = D_NOT_HANDLED;
	switch ( ComposeState ) {
		case csNORMAL:
			ComposeState = csCOMPOSE;
			KeyEventHandlers[ vkCompose ] = new KeyUpSink( pkb->vkCode, this );
			if ( hwndNotifyWindow ) {
				::PostMessage( hwndNotifyWindow, FCM_PIP, PIP_OK_1, 0 );
			}
			result = D_REJECT_KEY;
			break;

		case csCOMPOSE:
			ComposeState = csNORMAL;
			KeyEventHandlers[ vkCompose ] = new KeyUpSink( pkb->vkCode, this );
			if ( hwndNotifyWindow ) {
				::PostMessage( hwndNotifyWindow, FCM_PIP, PIP_ABORT, 0 );
			}
			result = D_ACCEPT_KEY;
			break;
	}

	if ( KeyEventHandlers[ vkCompose ] != keh ) {
		debug( L"ComposeKeyHandler@0x%p::KeyDown: Setting KeyEventHandlers slot to new KeyUpSink@0x%p\n", this, KeyEventHandlers[ vkCompose ] );
	}
	if ( ComposeState != oldComposeState ) {
		debug( L"ComposeKeyHandler@0x%p::KeyDown: Changing ComposeState from %s(%d) to %s(%d)", this, Stringify::from_COMPOSE_STATE( oldComposeState ), oldComposeState, Stringify::from_COMPOSE_STATE( ComposeState ), ComposeState );
	}

	return result;
}
