#pragma once

template<typename Tvalue, typename Tstring = _bstr_t>
class StringMapper {
public:
	inline StringMapper( ) {

	}

	inline StringMapper( Tstring const strings[] ): StringMapper( ) {
		for ( auto s : strings ) {
			_strings.Add( s );
		}
	}

	inline StringMapper( wchar_t const* strings[] ): StringMapper( ) {
		for ( auto s : strings ) {
			_strings.Add( s );
		}
	}

	inline StringMapper( std::initializer_list<Tstring> strings ): StringMapper( ) {
		for ( auto s : strings ) {
			_strings.Add( s );
		}
	}

	inline StringMapper( std::initializer_list<wchar_t const*> strings ): StringMapper( ) {
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

	inline Tvalue operator[]( Tstring const& value ) const {
		return Map( value );
	}

	inline Tvalue operator[]( wchar_t const* value ) const {
		return Map( value );
	}

	inline Tstring operator[]( Tvalue const& value ) const {
		return Map( value );
	}

	inline Tvalue Map( Tstring const& value ) const {
		Tvalue ret = static_cast<Tvalue>( 0 );
		for ( INT_PTR n = 0; n < _strings.GetCount( ); n++ ) {
			if ( static_cast<LPCWSTR>( _strings[n] ) && 0 == wcsicmp( _strings[n], value ) ) {
#pragma warning(suppress: 4800)
				ret = static_cast<Tvalue>( n );
				break;
			}
		}
		return ret;
	}

	inline Tstring Map( Tvalue const& value ) const {
		if ( static_cast<INT_PTR>( value ) >= _strings.GetCount( ) ) {
			return Tstring( );
		}
		return _strings[value];
	}

private:
	CArray<Tstring> _strings;
};
