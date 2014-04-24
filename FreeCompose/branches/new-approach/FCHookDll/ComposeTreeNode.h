#pragma once

class COMPOSE_TREE_NODE {
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
			return NULL;
		}
		return (*iter).second;
	}

	inline int GetLeaf( int character ) {
		auto iter = leaves.find( character );
		if ( iter == leaves.end( ) ) {
			return -1;
		}
		return (*iter).second;
	}

	inline void AddChild( int character, COMPOSE_TREE_NODE* child ) {
		children.insert( Tchildren::value_type( character, child ) );
	}

	inline void AddLeaf( int character, int result ) {
		leaves.insert( Tleaves::value_type( character, result ) );
#if DEBUG_NODES_LEAVES
		_cLeaves++;
#endif
	}

private:
	using Tchildren = std::map<int const, COMPOSE_TREE_NODE*>;
	using Tleaves = std::map<int const, int const>;

	Tchildren children;
	Tleaves leaves;

#if DEBUG_NODES_LEAVES
private: /*static*/
	static size_t _cNodes;
	static size_t _cLeaves;
#endif
};
