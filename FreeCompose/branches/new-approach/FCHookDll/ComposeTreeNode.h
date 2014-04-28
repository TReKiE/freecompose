#pragma once

class COMPOSE_TREE_NODE {
	using Tchildren = std::map<int, COMPOSE_TREE_NODE*>;
	using Tleaves = std::map<int, CString&>;

public:
#if DEBUG_NODES_LEAVES
	inline COMPOSE_TREE_NODE( ) {
		_cNodes++;
	}
#endif

	inline ~COMPOSE_TREE_NODE( ) {
#if DEBUG_NODES_LEAVES
		_cNodes--;
		_cLeaves -= leaves.size( );
#endif
		for ( auto& item : children ) {
			delete item.second;
		}
		children.clear( );
		leaves.clear( );
	}

	inline COMPOSE_TREE_NODE* GetChild( int character ) {
		auto iter = children.find( character );
		if ( iter == children.end( ) ) {
			return nullptr;
		}
		return (*iter).second;
	}

	inline CString GetLeaf( int character ) {
		auto iter = leaves.find( character );
		if ( iter == leaves.end( ) ) {
			return CString( );
		}
		return (*iter).second;
	}

	inline void AddChild( int character, COMPOSE_TREE_NODE* child ) {
		children.insert( Tchildren::value_type( character, child ) );
	}

	inline void AddLeaf( int character, CString& result ) {
		leaves.insert( Tleaves::value_type( character, result ) );
#if DEBUG_NODES_LEAVES
		_cLeaves++;
#endif
	}

private:
	Tchildren children;
	Tleaves leaves;

#if DEBUG_NODES_LEAVES
private: /*static*/
	static size_t _cNodes;
	static size_t _cLeaves;
#endif
};
