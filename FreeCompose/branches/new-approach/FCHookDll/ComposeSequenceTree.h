#pragma once

#define DEBUG_NODES_LEAVES 0

class COMPOSE_TREE_NODE {
public:
#if DEBUG_NODES_LEAVES
	COMPOSE_TREE_NODE( ) {
		_cNodes++;
	}
#endif

	~COMPOSE_TREE_NODE( );

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
	typedef std::map<int const, COMPOSE_TREE_NODE*> Tchildren;
	typedef std::map<int const, int const> Tleaves;

	Tchildren children;
	Tleaves leaves;

#if DEBUG_NODES_LEAVES
private: /*static*/
	static int _cNodes;
	static int _cLeaves;
#endif
};

class COMPOSE_SEQUENCE_TREE {
public:
	inline COMPOSE_SEQUENCE_TREE( ): root ( NULL ) {
	}

	inline COMPOSE_SEQUENCE_TREE( COMPOSE_SEQUENCE* pSequences, INT_PTR cSequences ): root ( NULL ) {
		BuildTree( pSequences, cSequences );
	}

	inline ~COMPOSE_SEQUENCE_TREE( ) {
		ReleaseTree( );
	}

	inline void ReleaseTree( void ) {
		if ( NULL != root ) {
			delete root;
			root = NULL;
		}
	}

	int LookUp( int length, int characters[] );

private:
	void BuildTree( COMPOSE_SEQUENCE* pSequences, INT_PTR cSequences );

	COMPOSE_TREE_NODE* root;
};
