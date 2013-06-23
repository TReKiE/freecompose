#pragma once

class COMPOSE_TREE_NODE {
public:
	~COMPOSE_TREE_NODE( );

	inline COMPOSE_TREE_NODE* GetChild( int character ) {
		auto iter = children.find( character );
		if ( iter == children.end( ) ) {
			return NULL;
		}
		return (*iter).second;
	}

	inline void AddChild( int character, COMPOSE_TREE_NODE* child ) {
		children.insert( Tchildren::value_type( character, child ) );
	}

	inline void AddLeaf( int character, int result ) {
		leaves.insert( Tleaves::value_type( character, result ) );
	}

private:
	typedef std::tr1::unordered_map<int const, COMPOSE_TREE_NODE*> Tchildren;
	typedef std::tr1::unordered_map<int const, int const> Tleaves;

	Tchildren children;
	Tleaves leaves;
};

class COMPOSE_SEQUENCE_TREE {
public:
	inline COMPOSE_SEQUENCE_TREE( ) {
	}

	inline COMPOSE_SEQUENCE_TREE( COMPOSE_SEQUENCE* pSequences, INT_PTR cSequences ) {
		BuildTree( pSequences, cSequences );
	}

private:
	COMPOSE_TREE_NODE root;

	void BuildTree( COMPOSE_SEQUENCE* pSequences, INT_PTR cSequences );
};
