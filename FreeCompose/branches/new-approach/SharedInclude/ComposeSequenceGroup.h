#pragma once

#pragma push_macro( "debug" )
#ifdef _NOISY_DEBUG
#define debug(x) OutputDebugString(x)
#else
#define debug(x)
#endif

#include <ComposeSequence.h>

//==============================================================================
// Classes
//==============================================================================

class ComposeSequenceGroup {
public:

	//
	// Rule of Five
	//

	inline ComposeSequenceGroup( ) {
		debug( L"ComposeSequenceGroup::`ctor()\n" );
	}

	inline ComposeSequenceGroup( ComposeSequenceGroup const& rhs ):
		ComposeSequenceGroup( )
	{
		debug( L"ComposeSequenceGroup::`ctor(ComposeSequenceGroup const&)\n" );
		operator=( rhs );
	}

	inline ComposeSequenceGroup( ComposeSequenceGroup&& rhs ):
		ComposeSequenceGroup( )
	{
		debug( L"ComposeSequenceGroup::`ctor(ComposeSequenceGroup&&)\n" );
		operator=( rhs );
	}

	inline ~ComposeSequenceGroup( ) {
		debug( L"ComposeSequenceGroup::`dtor()\n" );
	}

	inline ComposeSequenceGroup& operator=( ComposeSequenceGroup const& rhs ) {
		debug( L"ComposeSequenceGroup::operator=(ComposeSequenceGroup const&)\n" );
		Name = rhs.Name;
		ComposeSequences.RemoveAll( );
		ComposeSequences.Append( rhs.ComposeSequences );
		return *this;
	}

	inline ComposeSequenceGroup& operator=( ComposeSequenceGroup&& rhs ) {
		debug( L"ComposeSequenceGroup::operator=(ComposeSequenceGroup&&)\n" );
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
		debug( L"ComposeSequenceGroup::`ctor(CString const&)\n" );
		Name = name;
	}

	inline ComposeSequenceGroup( CString const& name, ComposeSequenceArray const& composeSequences ):
		ComposeSequenceGroup( name )
	{
		debug( L"ComposeSequenceGroup::`ctor(CString const&, ComposeSequenceArray const&)\n" );
		ComposeSequences.Append( composeSequences );
	}

	CString Name;
	ComposeSequenceArray ComposeSequences;
};

//==============================================================================
// Type aliases
//==============================================================================

using ComposeSequenceGroupArray = CArray<ComposeSequenceGroup>;

#pragma pop_macro( "debug" )
