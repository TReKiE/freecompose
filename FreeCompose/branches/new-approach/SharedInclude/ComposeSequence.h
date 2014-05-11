#pragma once

//==============================================================================
// Classes
//==============================================================================

class FCHOOKDLL_API ComposeSequence {
public:

	inline ComposeSequence( ):
		Disabled        ( false ),
		CaseInsensitive ( false ),
		Reversible      ( false )
	{

	}

	ComposeSequence( CString& sequence, CString& result ):
		Sequence        ( sequence ),
		Result          ( result ),
		Disabled        ( false ),
		CaseInsensitive ( false ),
		Reversible      ( false )
	{

	}

	ComposeSequence( CString& group, CString& sequence, CString& result ):
		Group           ( group ),
		Sequence        ( sequence ),
		Result          ( result ),
		Disabled        ( false ),
		CaseInsensitive ( false ),
		Reversible      ( false )
	{

	}

	ComposeSequence( CString& sequence, CString& result, bool const disabled, bool const caseInsensitive, bool const reversible ):
		Sequence        ( sequence ),
		Result          ( result ),
		Disabled        ( disabled ),
		CaseInsensitive ( caseInsensitive ),
		Reversible      ( reversible )
	{

	}

	ComposeSequence( CString& group, CString& sequence, CString& result, bool const disabled, bool const caseInsensitive, bool const reversible ):
		Group           ( group ),
		Sequence        ( sequence ),
		Result          ( result ),
		Disabled        ( disabled ),
		CaseInsensitive ( caseInsensitive ),
		Reversible      ( reversible )
	{

	}

	inline bool operator==( ComposeSequence const& b ) const {
		return
		(
			(    Group.Compare( b.Group    ) == 0 )  &&
			( Sequence.Compare( b.Sequence ) == 0 )  &&
			(   Result.Compare( b.Result   ) == 0 )  &&
			(        Disabled == b.Disabled        ) &&
			( CaseInsensitive == b.CaseInsensitive ) &&
			(      Reversible == b.Reversible      )
		);
	}

	inline bool operator!=( ComposeSequence const& b ) const {
		return
		(
			(    Group.Compare( b.Group    ) != 0 )  ||
			( Sequence.Compare( b.Sequence ) != 0 )  ||
			(   Result.Compare( b.Result   ) != 0 )  ||
			(        Disabled != b.Disabled        ) ||
			( CaseInsensitive != b.CaseInsensitive ) ||
			(      Reversible != b.Reversible      )
		);
	}

	CString Group;

	CString Sequence;
	CString Result;

	bool Disabled;
	bool CaseInsensitive;
	bool Reversible;
};
