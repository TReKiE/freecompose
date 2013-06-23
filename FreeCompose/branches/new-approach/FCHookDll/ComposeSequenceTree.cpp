#include "stdafx.h"

#include "ComposeSequenceTree.h"

using namespace std;

COMPOSE_TREE_NODE::~COMPOSE_TREE_NODE( ) {
	for_each( children.begin( ), children.end( ), [ ]( std::pair<int, COMPOSE_TREE_NODE*> item ) { delete item.second; } );
}

void COMPOSE_SEQUENCE_TREE::BuildTree( COMPOSE_SEQUENCE* pSequences, INT_PTR cSequences ) {
	for ( INT_PTR n = 0; n < cSequences; n++ ) {
		COMPOSE_TREE_NODE* node = root.GetChild( pSequences[n].chFirst );
		if ( NULL == node ) {
			node = new COMPOSE_TREE_NODE;
			root.AddChild( pSequences[n].chFirst, node );
		}
		node->AddLeaf( pSequences[n].chSecond, pSequences[n].chComposed );
	}
}
