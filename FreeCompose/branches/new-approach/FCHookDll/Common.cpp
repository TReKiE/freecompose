#include "stdafx.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

int CompareComposeSequences( void const* pv1, void const* pv2 ) {
	return static_cast<ComposeSequence const*>( pv1 )->Sequence.Compare( static_cast<ComposeSequence const*>( pv2 )->Sequence );
}
