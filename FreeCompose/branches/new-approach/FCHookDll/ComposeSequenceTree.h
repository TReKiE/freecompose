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

	void BuildTree( ComposeSequence* pSequences, INT_PTR cSequences ) {
		ReleaseTree( );
		root = new COMPOSE_TREE_NODE;

		for ( INT_PTR index = 0; index < cSequences; index++ ) {
			CString& sequence = pSequences[index].Sequence;
			int cchSeq = sequence.GetLength( );

			COMPOSE_TREE_NODE* parent = root;
			COMPOSE_TREE_NODE* node;
			int n;
			for ( n = 0; n < cchSeq - 1; n++ ) {
				wchar_t ch = sequence[n];
				node = parent->GetChild( ch );
				if ( !node ) {
					node = new COMPOSE_TREE_NODE;
					parent->AddChild( ch, node );
				}
				parent = node;
			}
			parent->AddLeaf( sequence[n], pSequences[index].Result );
		}
	}

	CString LookUp( int length, int characters[] ) {
		debug( L"COMPOSE_SEQUENCE_TREE::Lookup: length=%d characters={%.*s}\n", length, length, characters );
		if ( 2 != length ) {
			debug( L"COMPOSE_SEQUENCE_TREE::Lookup: length is bad\n" );
			return CString( );
		}

		COMPOSE_TREE_NODE* node;
		debug( L"COMPOSE_SEQUENCE_TREE::Lookup: trying forward lookup of { %d, %d }\n", characters[0], characters[1] );
		node = root->GetChild( characters[0] );
		if ( node ) {
			CString result( node->GetLeaf( characters[1] ) );
			debug( L"COMPOSE_SEQUENCE_TREE::Lookup: success, found %d '%c'\n", result, result );
			return result;
		}

		debug( L"COMPOSE_SEQUENCE_TREE::Lookup: trying reverse lookup of { %d, %d }\n", characters[1], characters[0] );
		node = root->GetChild( characters[1] );
		if ( node ) {
			CString result( node->GetLeaf( characters[0] ) );
			debug( L"COMPOSE_SEQUENCE_TREE::Lookup: success, found %d '%c'\n", result, result );
			return result;
		}

		debug( L"COMPOSE_SEQUENCE_TREE::Lookup: failed\n" );
		return CString( );
	}

private:

	COMPOSE_TREE_NODE* root;
};
