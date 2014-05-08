#pragma once

class CComposeTreeNode {
	using Tchildren = std::map<int, CComposeTreeNode*>;
	using Tleaves = std::map<int, CString&>;

	Tchildren children;
	Tleaves leaves;

#if DEBUG_NODES_LEAVES
private: /*static*/
	static size_t _cNodes;
	static size_t _cLeaves;
#endif

public:
#if DEBUG_NODES_LEAVES
	inline CComposeTreeNode( ) {
		_cNodes++;
	}
#endif

	inline ~CComposeTreeNode( ) {
#if DEBUG_NODES_LEAVES
		_cNodes--;
		_cLeaves -= leaves.size( );
#endif

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
#if DEBUG_NODES_LEAVES
		_cLeaves++;
#endif
	}

};
