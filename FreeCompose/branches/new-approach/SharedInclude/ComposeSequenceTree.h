#pragma once

#include "ComposeTreeNode.h"

class FCHOOKDLL_API CComposeSequenceTree {
public:
	inline CComposeSequenceTree( ):
		_pRoot( nullptr )
	{

	}

	inline ~CComposeSequenceTree( ) {
		ReleaseTree( );
	}

	void BuildTree( ComposeSequence* pSequences, int const cSequences );
	void ReleaseTree( void );
	bool LookUp( CString const& strSequence, CString& strResult );

private:
	enum SEQUENCE_RESULT {
		srUnknown,

		// A sequence comprising a prefix of the input sequence terminates in a leaf.
		srPrefix,

		// The complete sequence exists and terminates in a leaf.
		srAlreadyExists,

		// The complete sequence would terminate on a nonexistent leaf, or we don't
		// even reach that leaf because we run out of nodes on the way down.
		srUnambiguous,

		srMaximumValue,
	};

	CComposeTreeNode* _pRoot;

	bool _ValidateSequence( CString const& strSequence );
	bool _ValidateResult( CString const& strResult );
	bool _IsSequenceUnambiguous( CString const& strSequence, SEQUENCE_RESULT& SequenceResult, int& nIndex );
	bool _AddSequence( CString const& strSequence, CString& strResult );

};
