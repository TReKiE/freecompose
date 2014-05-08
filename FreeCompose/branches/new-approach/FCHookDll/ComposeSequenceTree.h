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
	bool _ValidateSequence( CString const& strSequence ) {
		int cchSequence = strSequence.GetLength( );
		if ( cchSequence < 2 ) {
			debug( L"CComposeSequenceTree::LookUp: character sequence is too short\n" );
			return false;
		} else if ( cchSequence > MAXIMUM_SEQUENCE_LENGTH ) {
			debug( L"CComposeSequenceTree::LookUp: character sequence is too long\n" );
			return false;
		}

		return true;
	}

	bool _ValidateResult( CString const& strResult ) {
		int cchResult = strResult.GetLength( );
		if ( cchResult < 1 ) {
			debug( L"CComposeSequenceTree::_ValidateSequence: result string is too short\n" );
		}
		// TODO
		// add more tests against strResult, like
		// -- is it a regular character or combining character, followed by zero or more combining characters?
		//    -- that is to say, does it produce a single, optionally-modified, glyph?

		return true;
	}

	void _AddSequence( CString const& strSequence, CString const& strResult ) {
		CComposeTreeNode* pNode = _pRoot;
		int n;

		int cchSequence = strSequence.GetLength( );
		for ( n = 0; n < cchSequence - 1; n++ ) {
			wchar_t ch = strSequence[n];

			CComposeTreeNode* pNext = pNode->GetChild( ch );
			if ( !pNext ) {
				pNext = new CComposeTreeNode( );
				pNode->AddChild( ch, pNext );
			}

			pNode = pNext;
		}

		pNode->AddLeaf( strSequence[n], const_cast<CString&>( strResult ) );
	}

public:
	void BuildTree( ComposeSequence const* pSequences, INT_PTR cSequences ) {
		ReleaseTree( );
		_pRoot = new CComposeTreeNode( );

		for ( INT_PTR index = 0; index < cSequences; index++ ) {
			CString const& sequence = pSequences[index].Sequence;
			CString const& result = pSequences[index].Result;
			int cchSequence = sequence.GetLength( );

			if ( !_ValidateSequence( sequence ) ) {
				debug( L"CComposeSequenceTree::BuildTree: compose sequence #%d: key sequence is invalid\n", index );
				continue;
			}
			if ( !_ValidateResult( result ) ) {
				debug( L"CComposeSequenceTree::BuildTree: compose sequence #%d: result is invalid\n", index );
				continue;
			}

			_AddSequence( sequence, result );

			if ( 2 == cchSequence ) {
				// for backwards compatibility, if a sequence is two characters long, add its reverse as well
				wchar_t ch[2] = { sequence[0], sequence[1] };
				CString swapped( ch, 2 );

				_AddSequence( swapped, result );
			}
		}
	}

	bool LookUp( CString const& strSequence, CString& strResult ) {
		debug( L"CComposeSequenceTree::LookUp: length=%d characters='%s'\n", strSequence.GetLength( ), strSequence.GetString( ) );

		strResult.Empty( );

		if ( !_ValidateSequence( strSequence ) ) {
			debug( L"CComposeSequenceTree::LookUp: key sequence is invalid\n" );
			return false;
		}

		int length = strSequence.GetLength( );
		CComposeTreeNode* node = _pRoot;
		int n;

		for ( n = 0; n < length - 1; n++ ) {
			CComposeTreeNode* next = node->GetChild( strSequence[n] );
			if ( !next ) {
				debug( L"CComposeSequenceTree::LookUp: Hit bottom of tree before we should've. node=0x%p length=%d index=%d\n", node, length, n );
				return false;
			}
			node = next;
		}

		if ( !node ) {
			debug( L"CComposeSequenceTree::LookUp: How did we get here?\n" );
			return false;
		}

		if ( !node->GetLeaf( strSequence[n], strResult ) ) {
			debug( L"CComposeSequenceTree::LookUp: Leaf lookup failed. node=0x%p length=%d index=%d\n", node, length, n );
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
