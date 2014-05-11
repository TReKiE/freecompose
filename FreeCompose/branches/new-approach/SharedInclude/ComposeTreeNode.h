#pragma once

class FCHOOKDLL_API CComposeTreeNode {
	using Tchildren = std::map<int, CComposeTreeNode*>;
	using Tleaves = std::map<int, CString&>;

	Tchildren children;
	Tleaves leaves;

public:
	inline ~CComposeTreeNode( ) {
		for ( auto& item : children ) {
			delete item.second;
		}
		// TODO is this next line the cause of the crash? or the above, rather, maybe?
		children.clear( );
		leaves.clear( );
	}

	inline CComposeTreeNode* GetChild( int character ) {
		auto iter = children.find( character );
		if ( iter == children.end( ) ) {
			return nullptr;
		}
		return iter->second;
	}

	inline bool GetLeaf( int character, CString& result ) {
		result.Empty( );
		auto iter = leaves.find( character );
		if ( iter == leaves.end( ) ) {
			return false;
		}
		result = CString( iter->second );
		return true;
	}

	inline void AddChild( int character, CComposeTreeNode* child ) {
		children.insert( Tchildren::value_type( character, child ) );
	}

	inline void AddLeaf( int character, CString& result ) {
		leaves.insert( Tleaves::value_type( character, result ) );
	}

};
