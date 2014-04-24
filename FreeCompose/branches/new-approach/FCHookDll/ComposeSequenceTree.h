#pragma once

#include "ComposeTreeNode.h"

class COMPOSE_SEQUENCE_TREE {
public:
	inline COMPOSE_SEQUENCE_TREE( ): root( nullptr ) {

	}

	inline ~COMPOSE_SEQUENCE_TREE( ) {
		ReleaseTree( );
	}

	inline void ReleaseTree( void ) {
		if ( root ) {
			delete root;
			root = nullptr;
		}
	}

	void BuildTree( COMPOSE_SEQUENCE* pSequences, INT_PTR cSequences ) {
		ReleaseTree( );

		root = new COMPOSE_TREE_NODE;
		for ( INT_PTR n = 0; n < cSequences; n++ ) {
			COMPOSE_TREE_NODE* node = root->GetChild( pSequences[n].chFirst );
			if ( !node ) {
				node = new COMPOSE_TREE_NODE;
				root->AddChild( pSequences[n].chFirst, node );
			}
			node->AddLeaf( pSequences[n].chSecond, pSequences[n].chComposed );
		}
	}

	int LookUp( int length, int characters[] ) {
		debug( L"COMPOSE_SEQUENCE_TREE::Lookup: length=%d characters={%.*s}\n", length, length, characters );
		if ( 2 != length ) {
			debug( L"COMPOSE_SEQUENCE_TREE::Lookup: length is bad\n" );
			return -1;
		}

		COMPOSE_TREE_NODE* node;
		debug( L"COMPOSE_SEQUENCE_TREE::Lookup: trying forward lookup of { %d, %d }\n", characters[0], characters[1] );
		node = root->GetChild( characters[0] );
		if ( node ) {
			int result = node->GetLeaf( characters[1] );
			debug( L"COMPOSE_SEQUENCE_TREE::Lookup: success, found %d '%c'\n", result, result );
			return result;
		}

		debug( L"COMPOSE_SEQUENCE_TREE::Lookup: trying reverse lookup of { %d, %d }\n", characters[1], characters[0] );
		node = root->GetChild( characters[1] );
		if ( node ) {
			int result = node->GetLeaf( characters[0] );
			debug( L"COMPOSE_SEQUENCE_TREE::Lookup: success, found %d '%c'\n", result, result );
			return result;
		}

		debug( L"COMPOSE_SEQUENCE_TREE::Lookup: failed\n" );
		return -1;
	}

private:

	COMPOSE_TREE_NODE* root;
};
