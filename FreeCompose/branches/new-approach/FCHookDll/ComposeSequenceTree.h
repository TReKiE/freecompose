#pragma once

#include "ComposeTreeNode.h"

class CComposeSequenceTree {
	CComposeTreeNode* _pRoot;

public:
	inline CComposeSequenceTree( ): _pRoot( nullptr ) {

	}

	inline ~CComposeSequenceTree( ) {
		ReleaseTree( );
	}

private:
	void _AddOneSequence( int const cchSequence, CString& strSequence, CString& strResult ) {
		CComposeTreeNode* pNode = _pRoot;
		int n;

		for ( n = 0; n < cchSequence - 1; n++ ) {
			wchar_t ch = strSequence[n];

			CComposeTreeNode* pNext = pNode->GetChild( ch );
			if ( !pNext ) {
				pNext = new CComposeTreeNode( );
				pNode->AddChild( ch, pNext );
			}

			pNode = pNext;
		}

		pNode->AddLeaf( strSequence[n], strResult );
	}

public:
	void BuildTree( ComposeSequence* pSequences, INT_PTR cSequences ) {
		ReleaseTree( );
		_pRoot = new CComposeTreeNode( );

		for ( INT_PTR index = 0; index < cSequences; index++ ) {
			CString& sequence = pSequences[index].Sequence;
			CString& result = pSequences[index].Result;
			int cchSequence = sequence.GetLength( );

			_AddOneSequence( cchSequence, sequence, result );

			if ( 2 == cchSequence ) {
				// for backwards compatibility, if a sequence is two characters long, add its reverse as well
				wchar_t ch[2] = { sequence[0], sequence[1] };
				CString swapped( ch, 2 );

				_AddOneSequence( cchSequence, swapped, result );
			}
		}
	}

	bool LookUp( int cchSequence, wchar_t const wchSequence[], CString& strResult ) {
		debug( L"CComposeSequenceTree::LookUp: length=%d characters={%.*s}\n", cchSequence, cchSequence, wchSequence );
		strResult.Empty( );

		if ( cchSequence < 2 ) {
			debug( L"CComposeSequenceTree::LookUp: character sequence is too short\n" );
			return false;
		}
		if ( cchSequence > MAXIMUM_SEQUENCE_LENGTH ) {
			debug( L"CComposeSequenceTree::LookUp: character sequence is too long\n" );
			return false;
		}

		CComposeTreeNode* node = _pRoot;
		int n;

		for ( n = 0; n < cchSequence - 1; n++ ) {
			if ( !wchSequence[n] ) {
				debug( L"CComposeSequenceTree::LookUp: Run out of characters before we should've. node=0x%p length=%d index=%d\n", node, cchSequence, n );
				return false;
			}

			CComposeTreeNode* next = node->GetChild( wchSequence[n] );
			if ( !next ) {
				debug( L"CComposeSequenceTree::LookUp: Hit bottom of tree before we should've. node=0x%p length=%d index=%d\n", node, cchSequence, n );
				return false;
			}
			node = next;
		}

		if ( !node ) {
			debug( L"CComposeSequenceTree::LookUp: How did we get here?\n" );
			return false;
		}

		if ( !node->GetLeaf( wchSequence[n], strResult ) ) {
			debug( L"CComposeSequenceTree::LookUp: Leaf lookup failed. node=0x%p length=%d index=%d\n", node, cchSequence, n );
			return false;
		}

		return true;
	}

	inline void ReleaseTree( void ) {
		if ( _pRoot ) {
			delete _pRoot;
			_pRoot = nullptr;
		}
	}

};
