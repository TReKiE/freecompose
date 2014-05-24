#include "stdafx.h"
#include "ComposeSequenceTree.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

//
// Implementation
//

bool CComposeSequenceTree::_ValidateSequence( CString const& strSequence ) {
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

bool CComposeSequenceTree::_ValidateResult( CString const& strResult ) {
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

bool CComposeSequenceTree::_IsSequenceUnambiguous( CString const& strSequence, SEQUENCE_RESULT& SequenceResult, int& nIndex ) {
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

bool CComposeSequenceTree::_AddSequence( CString const& strSequence, CString& strResult ) {
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

//
// Interface
//

void CComposeSequenceTree::ReleaseTree( void ) {
	if ( _pRoot ) {
		delete _pRoot;
		_pRoot = nullptr;
	}
}

void CComposeSequenceTree::BuildTree( ComposeSequence* pSequences, int const cSequences ) {
	ReleaseTree( );
	_pRoot = new CComposeTreeNode( );

	for ( int index = 0; index < cSequences; index++ ) {
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
			wchar_t wch[2] = { sequence[1], sequence[0] };
			CString reverse( wch, 2 );

			SEQUENCE_RESULT sr1, sr2;
			int index1, index2;
			bool f1 = _IsSequenceUnambiguous( sequence, sr1, index1 );
			bool f2 = _IsSequenceUnambiguous( reverse,  sr2, index2 );

			if ( !f1 ) {
				debug( L"CComposeSequenceTree::BuildTree: two-character sequence, and forward sequence is ambiguous.\n+ sr1=%d index1=%d f1=%s\n+ sr2=%d index2=%d f2=%s\n", sr1, index1, ( f1 ? L"TRUE" : L"false" ), sr2, index2, ( f2 ? L"TRUE" : L"false" ) );
			} else if ( !f2 ) {
				debug( L"CComposeSequenceTree::BuildTree: two-character sequence, and reverse sequence is ambiguous.\n+ sr1=%d index1=%d f1=%s\n+ sr2=%d index2=%d f2=%s\n", sr1, index1, ( f1 ? L"TRUE" : L"false" ), sr2, index2, ( f2 ? L"TRUE" : L"false" ) );
			} else {
				_AddSequence( sequence, result );
				_AddSequence( reverse,  result );
			}
		} else {
			SEQUENCE_RESULT sr;
			int dummy;

			bool f = _IsSequenceUnambiguous( sequence, sr, dummy );
			if ( !f ) {
				debug( L"CComposeSequenceTree::BuildTree: multi-character sequence, and sequence is ambiguous.\n+ sr=%d index=%d f1%s\n", sr, index, ( f ? L"TRUE" : L"false" ) );
			} else {
				_AddSequence( sequence, result );
			}
		}
	}
}

bool CComposeSequenceTree::LookUp( CString const& strSequence, CString& strResult ) {
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
