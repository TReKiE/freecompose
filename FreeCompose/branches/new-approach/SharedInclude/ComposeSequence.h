#pragma once

//==============================================================================
// Classes
//==============================================================================

class FCHOOKDLL_API ComposeSequence {

public:
	//
	// Rule of Five
	//

	inline ComposeSequence( ):
		Disabled        ( false ),
		CaseInsensitive ( false ),
		Reversible      ( false )
	{
		noisydebug( L"ComposeSequence::`ctor()\n" );
	}

	inline ComposeSequence( ComposeSequence const& rhs ) {
		noisydebug( L"ComposeSequence::`copy-ctor()\n" );
		operator=( rhs );
	}

	inline ComposeSequence( ComposeSequence&& rhs ) {
		noisydebug( L"ComposeSequence::`move-ctor()\n" );
		operator=( rhs );
	}

	~ComposeSequence( )
	{
		noisydebug( L"ComposeSequence::`dtor()\n" );
	}

	inline ComposeSequence& operator=( ComposeSequence const& rhs ) {
		noisydebug( L"ComposeSequence::copy-operator=()\n" );
		Sequence = rhs.Sequence;
		Result = rhs.Result;
		Disabled = rhs.Disabled;
		CaseInsensitive = rhs.CaseInsensitive;
		Reversible = rhs.Reversible;
		return *this;
	}

	inline ComposeSequence& operator=( ComposeSequence&& rhs ) {
		noisydebug( L"ComposeSequence::move-operator=()\n" );
		operator=( const_cast<ComposeSequence const&>( rhs ) );
		rhs.Sequence.Empty( );
		rhs.Result.Empty( );
		rhs.Disabled = false;
		rhs.CaseInsensitive = false;
		rhs.Reversible = false;
		return *this;
	}

	//
	// User constructors
	//

	ComposeSequence( CString& sequence, CString& result ):
		Sequence        ( sequence ),
		Result          ( result ),
		Disabled        ( false ),
		CaseInsensitive ( false ),
		Reversible      ( false )
	{
		noisydebug( L"ComposeSequence::`ctor(CString&,CString&)\n" );
	}

	ComposeSequence( CString& sequence, CString& result, bool const disabled, bool const caseInsensitive, bool const reversible ):
		Sequence        ( sequence ),
		Result          ( result ),
		Disabled        ( disabled ),
		CaseInsensitive ( caseInsensitive ),
		Reversible      ( reversible )
	{
		noisydebug( L"ComposeSequence::`ctor(CString&,CString&,bool,bool,bool)\n" );
	}

	inline bool operator==( ComposeSequence const& b ) const {
		noisydebug( L"ComposeSequence::operator==(ComposeSequence const&)\n" );
		return
		(
			( Sequence.Compare( b.Sequence ) == 0 )  &&
			(   Result.Compare( b.Result   ) == 0 )  &&
			(        Disabled == b.Disabled        ) &&
			( CaseInsensitive == b.CaseInsensitive ) &&
			(      Reversible == b.Reversible      )
		);
	}

	inline bool operator!=( ComposeSequence const& b ) const {
		noisydebug( L"ComposeSequence::operator!=(ComposeSequence const&)\n" );
		return
		(
			( Sequence.Compare( b.Sequence ) != 0 )  ||
			(   Result.Compare( b.Result   ) != 0 )  ||
			(        Disabled != b.Disabled        ) ||
			( CaseInsensitive != b.CaseInsensitive ) ||
			(      Reversible != b.Reversible      )
		);
	}

	CString Sequence;
	CString Result;

	bool Disabled;
	bool CaseInsensitive;
	bool Reversible;
};

//==============================================================================
// Type aliases
//==============================================================================

using ComposeSequenceArray = CArray<ComposeSequence>;
