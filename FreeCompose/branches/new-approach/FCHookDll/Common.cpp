#include "stdafx.h"

int CompareComposeSequences( void const* pv1, void const* pv2 ) {
	return static_cast<ComposeSequence const*>( pv1 )->Sequence.Compare( static_cast<ComposeSequence const*>( pv2 )->Sequence );
}
