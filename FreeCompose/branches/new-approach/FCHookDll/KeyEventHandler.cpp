#include "stdafx.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

DISPOSITION KeyEventHandler::KeyDown( KBDLLHOOKSTRUCT* /*pkb*/ ) {
	return D_NOT_HANDLED;
}

DISPOSITION KeyEventHandler::KeyUp( KBDLLHOOKSTRUCT* /*pkb*/ ) {
	return D_NOT_HANDLED;
}
