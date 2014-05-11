#pragma once

#include "ComposeTreeNode.h"

class FCHOOKDLL_API CComposeSequenceTree {
	CComposeTreeNode* _pRoot;

public:
	inline CComposeSequenceTree( ): _pRoot( nullptr ) {

	}

	inline ~CComposeSequenceTree( ) {
		ReleaseTree( );
	}

	void BuildTree( ComposeSequence* pSequences, INT_PTR cSequences );
	void ReleaseTree( void );
	bool LookUp( CString const& strSequence, CString& strResult );

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

	bool _ValidateSequence( CString const& strSequence );
	bool _ValidateResult( CString const& strResult );
	bool _IsSequenceUnambiguous( CString const& strSequence, SEQUENCE_RESULT& SequenceResult, int& nIndex );
	bool _AddSequence( CString const& strSequence, CString& strResult );

};
