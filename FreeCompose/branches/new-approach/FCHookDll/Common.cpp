#include "stdafx.h"
#include "FCHookDll.h"
#include "HookProc.h"
#include "Common.h"

int CompareComposeSequences( const void* pv1, const void* pv2 ) {
	COMPOSE_SEQUENCE* pcs1 = (COMPOSE_SEQUENCE*) pv1;
	COMPOSE_SEQUENCE* pcs2 = (COMPOSE_SEQUENCE*) pv2;

	if ( pcs1->chFirst < pcs2->chFirst ) {
		return -1;
	} else if ( pcs1->chFirst == pcs2->chFirst ) {
		if ( pcs1->chSecond < pcs2->chSecond ) {
			return -1;
		} else if ( pcs1->chSecond == pcs2->chSecond ) {
			return 0;
		} else {
			return 1;
		}
	} else {
		return 1;
	}
}
