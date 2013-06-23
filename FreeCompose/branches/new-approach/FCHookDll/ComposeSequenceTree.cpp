#include "stdafx.h"

#include "ComposeSequenceTree.h"

using namespace std;

#if DEBUG_NODES_LEAVES
int COMPOSE_TREE_NODE::_cNodes = 0;
int COMPOSE_TREE_NODE::_cLeaves = 0;
#endif

COMPOSE_TREE_NODE::~COMPOSE_TREE_NODE( ) {
	for_each( children.begin( ), children.end( ), [ ]( Tchildren::value_type item ) { delete item.second; } );

#if DEBUG_NODES_LEAVES
	_cNodes--;
	_cLeaves -= leaves.size( );
#endif
}

void COMPOSE_SEQUENCE_TREE::BuildTree( COMPOSE_SEQUENCE* pSequences, INT_PTR cSequences ) {
	if ( NULL != root ) {
		ReleaseTree( );
	}

	root = new COMPOSE_TREE_NODE;
	for ( INT_PTR n = 0; n < cSequences; n++ ) {
		COMPOSE_TREE_NODE* node = root->GetChild( pSequences[n].chFirst );
		if ( NULL == node ) {
			node = new COMPOSE_TREE_NODE;
			root->AddChild( pSequences[n].chFirst, node );
		}
		node->AddLeaf( pSequences[n].chSecond, pSequences[n].chComposed );
	}
}
