#pragma once

template<typename T>
class StringMapper {
public:
	inline StringMapper( ) {

	}

	inline StringMapper( _bstr_t const* strings ): StringMapper( ) {
		for ( auto s : strings ) {
			_strings.Add( s );
		}
	}

	inline StringMapper( std::initializer_list<_bstr_t> strings ): StringMapper( ) {
		for ( auto s : strings ) {
			_strings.Add( s );
		}
	}

	inline StringMapper( StringMapper const& rhs ): StringMapper( ) {
		operator=( rhs );
	}

	inline StringMapper( StringMapper&& rhs ): StringMapper( ) {
		operator=( rhs );
	}

	inline ~StringMapper( ) {
		_strings.RemoveAll( );
	}

	inline StringMapper& operator=( StringMapper const& rhs ) {
		_strings.RemoveAll( );
		for ( auto s : rhs._strings ) {
			_strings.Add( s );
		}
		return *this;
	}

	inline StringMapper& operator=( StringMapper&& rhs ) {
		_strings.RemoveAll( );
		for ( auto s : rhs._strings ) {
			_strings.Add( s );
		}
		rhs._strings.RemoveAll( );
		return *this;
	}

	inline T operator[]( _bstr_t const& value ) const {
		return Map( value );
	}

	inline _bstr_t operator[]( T const& value ) const {
		return Map( value );
	}

	inline T Map( _bstr_t const& value ) const {
		T ret = static_cast<T>( 0 );
		for ( INT_PTR n = 0; n < _strings.GetCount( ); n++ ) {
			if ( 0 == wcsicmp( _strings[n], value ) ) {
#pragma warning(suppress: 4800)
				ret = static_cast<T>( n );
			}
		}
		return ret;
	}

	inline _bstr_t Map( T const& value ) const {
		if ( static_cast<INT_PTR>( value ) >= _strings.GetCount( ) ) {
			return _bstr_t( );
		}
		return _strings[value];
	}

private:
	CArray<_bstr_t> _strings;
};
