#pragma once

#include <ComposeSequence.h>

class ComposeSequenceGroup;
using ComposeSequenceGroupArray = CArray<ComposeSequenceGroup>;

class ComposeSequenceGroup {
public:

	//
	// Rule of Five
	//

	inline ComposeSequenceGroup( ) {

	}

	inline ComposeSequenceGroup( ComposeSequenceGroup const& rhs ):
		ComposeSequenceGroup( )
	{
		operator=( rhs );
	}

	inline ComposeSequenceGroup( ComposeSequenceGroup&& rhs ):
		ComposeSequenceGroup( )
	{
		operator=( rhs );
	}

	inline ~ComposeSequenceGroup( ) {
		
	}

	inline ComposeSequenceGroup& operator=( ComposeSequenceGroup const& rhs ) {
		Name = rhs.Name;
		ComposeSequences.RemoveAll( );
		ComposeSequences.Copy( rhs.ComposeSequences );
		return *this;
	}

	inline ComposeSequenceGroup& operator=( ComposeSequenceGroup&& rhs ) {
		operator=( const_cast<ComposeSequenceGroup const&>( rhs ) );
		rhs.Name.Empty( );
		rhs.ComposeSequences.RemoveAll( );
		return *this;
	}

	inline ComposeSequenceGroup( CString const& name ):
		ComposeSequenceGroup( )
	{
		Name = name;
	}

	inline ComposeSequenceGroup( CString const& name, ComposeSequenceArray const& composeSequences ):
		ComposeSequenceGroup( name )
	{
		ComposeSequences.Copy( composeSequences );
	}

	CString Name;
	ComposeSequenceArray ComposeSequences;
};
