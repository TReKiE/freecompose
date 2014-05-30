#pragma once

//==============================================================================
// Classes
//==============================================================================

class FCHOOKDLL_API ComposeSequenceGroup {
public:

	//
	// Rule of Five
	//

	inline ComposeSequenceGroup( ) {
		noisydebug( L"ComposeSequenceGroup::`ctor()\n" );
	}

	inline ComposeSequenceGroup( ComposeSequenceGroup const& rhs ):
		ComposeSequenceGroup( )
	{
		noisydebug( L"ComposeSequenceGroup::`ctor(ComposeSequenceGroup const&)\n" );
		operator=( rhs );
	}

	inline ComposeSequenceGroup( ComposeSequenceGroup&& rhs ):
		ComposeSequenceGroup( )
	{
		noisydebug( L"ComposeSequenceGroup::`ctor(ComposeSequenceGroup&&)\n" );
		operator=( rhs );
	}

	inline ~ComposeSequenceGroup( ) {
		noisydebug( L"ComposeSequenceGroup::`dtor()\n" );
	}

	inline ComposeSequenceGroup& operator=( ComposeSequenceGroup const& rhs ) {
		noisydebug( L"ComposeSequenceGroup::operator=(ComposeSequenceGroup const&)\n" );
		Name = rhs.Name;
		ComposeSequences.RemoveAll( );
		ComposeSequences.Append( rhs.ComposeSequences );
		return *this;
	}

	inline ComposeSequenceGroup& operator=( ComposeSequenceGroup&& rhs ) {
		noisydebug( L"ComposeSequenceGroup::operator=(ComposeSequenceGroup&&)\n" );
		operator=( const_cast<ComposeSequenceGroup const&>( rhs ) );
		rhs.Name.Empty( );
		rhs.ComposeSequences.RemoveAll( );
		return *this;
	}

	//
	// User constructors
	//

	inline ComposeSequenceGroup( CString const& name ):
		ComposeSequenceGroup( )
	{
		noisydebug( L"ComposeSequenceGroup::`ctor(CString const&)\n" );
		Name = name;
	}

	inline ComposeSequenceGroup( CString const& name, ComposeSequenceArray const& composeSequences ):
		ComposeSequenceGroup( name )
	{
		noisydebug( L"ComposeSequenceGroup::`ctor(CString const&, ComposeSequenceArray const&)\n" );
		ComposeSequences.Append( composeSequences );
	}

	CString Name;
	ComposeSequenceArray ComposeSequences;
};

//==============================================================================
// Type aliases
//==============================================================================

using ComposeSequenceGroupArray = CArray<ComposeSequenceGroup>;
