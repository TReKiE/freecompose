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
	// srPrefix means that a sequence comprising a prefix of strSequence terminates in a leaf.
	// srAlreadyExists means that the complete sequence in strSequence exists and terminates in a leaf.
	// srUnambiguous means that the complete sequence terminates on a nonexistent leaf, or
	//               we don't even reach the leaf because we run out of branches on the way down.
	enum SEQUENCE_RESULT {
		srUnknown,
		srPrefix,
		srAlreadyExists,
		srUnambiguous,
		srMaximumValue,
	};

	bool _IsSequenceUnambiguous( _In_ CString const& strSequence, _Out_ SEQUENCE_RESULT& SequenceResult, _Out_ int& nIndex ) {
		CComposeTreeNode* pNode = _pRoot;
		CString strResult;
		int n;

		SequenceResult = srUnknown;
		nIndex = -1;

		int cchSequence = strSequence.GetLength( );
		for ( n = 0; n < cchSequence - 1; n++ ) {
			wchar_t ch = strSequence[n];

			if ( ( n > 0 ) && pNode->GetLeaf( ch, strResult ) ) {
				debug( L"CComposeSequenceTree::_IsSequenceAmbiguous: sequence '%s' is a prefix at index %d\n", strSequence.GetString( ), n );
				SequenceResult = srPrefix;
				nIndex = n;
				return false;
			}

			CComposeTreeNode* pNext = pNode->GetChild( ch );
			if ( !pNext ) {
				debug( L"CComposeSequenceTree::_IsSequenceAmbiguous: sequence '%s' is unambiguous starting at index %d\n", strSequence.GetString( ), n );
				SequenceResult = srUnambiguous;
				nIndex = n;
				break;
			}

			pNode = pNext;
		}

		if ( ( srUnknown == SequenceResult ) && pNode->GetLeaf( strSequence[n], strResult ) ) {
			debug( L"CComposeSequenceTree::_IsSequenceAmbiguous: whole sequence '%s' already exists\n", strSequence.GetString( ) );
			SequenceResult = srAlreadyExists;
			nIndex = n;
			return false;
		}

		SequenceResult = srUnambiguous;
		if ( -1 == nIndex ) {
			nIndex = n;
		}

		return true;
	}

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

	bool _AddSequence( CString const& strSequence, CString& strResult ) {
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

		pNode->AddLeaf( strSequence[n], strResult );

		return true;
	}

public:
	void BuildTree( ComposeSequence* pSequences, INT_PTR cSequences ) {
		ReleaseTree( );
		_pRoot = new CComposeTreeNode( );

		for ( INT_PTR index = 0; index < cSequences; index++ ) {
			CString& sequence = pSequences[index].Sequence;
			CString& result = pSequences[index].Result;
			int cchSequence = sequence.GetLength( );

			if ( !_ValidateSequence( sequence ) ) {
				debug( L"CComposeSequenceTree::BuildTree: compose sequence #%d: key sequence is invalid\n", index );
				continue;
			}
			if ( !_ValidateResult( result ) ) {
				debug( L"CComposeSequenceTree::BuildTree: compose sequence #%d: result is invalid\n", index );
				continue;
			}

			if ( 2 == cchSequence ) {
				wchar_t ch[2] = { sequence[1], sequence[0] };
				CString reverse( ch, 2 );

				SEQUENCE_RESULT sr1, sr2;
				int index1, index2;
				bool f1, f2;
				f1 = _IsSequenceUnambiguous( sequence, sr1, index1 );
				f2 = _IsSequenceUnambiguous( reverse,  sr2, index2 );

				wchar_t* pwzDirection;
				if ( !f1 ) {
					pwzDirection = L"forward";
				} else if ( !f2 ) {
					pwzDirection = L"reverse";
				} else {
					pwzDirection = L"neither [this means 'everything is okay']";
					_AddSequence( sequence, result );
					_AddSequence( reverse,  result );
				}
				debug( L"CComposeSequenceTree::BuildTree: two-character sequence, and %s sequence is not unambiguous.\n+ sr1=%d index1=%d f1=%s\n+ sr2=%d index2=%d f2=%s\n", pwzDirection, sr1, index1, ( f1 ? L"TRUE" : L"false" ), sr2, index2, ( f2 ? L"TRUE" : L"false" ) );
			} else {
				SEQUENCE_RESULT sr;
				int index;

				bool f = _IsSequenceUnambiguous( sequence, sr, index );
				if ( !f ) {
					debug( L"CComposeSequenceTree::BuildTree: multi-character sequence, and sequence is not unambiguous.\n+ sr=%d index=%d f1%s\n", sr, index, ( f ? L"TRUE" : L"false" ) );
				} else {
					_AddSequence( sequence, result );
				}
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
