#pragma once

class Utf1632Adaptor {

public:
	//
	// Default constructor
	//

	inline Utf1632Adaptor( ):
		m_cch16 ( 0 ),
		m_cch32 ( 0 ),
		m_p16   ( nullptr ),
		m_p32   ( nullptr )
	{
		
	}

	//
	// Copy and move constructors and assignment operators, for ourself, type Utf1632Adaptor
	//

	inline Utf1632Adaptor( Utf1632Adaptor const& rhs ):
		Utf1632Adaptor( )
	{
		*this = rhs;
	}

	inline Utf1632Adaptor( Utf1632Adaptor&& rhs ):
		Utf1632Adaptor( )
	{
		*this = rhs;
	}

	inline Utf1632Adaptor& operator=( Utf1632Adaptor const& rhs ) {
		_Free( );

		m_cch16 = rhs.m_cch16;
		m_p16 = new UChar[m_cch16];
		memcpy( m_p16, rhs.m_p16, m_cch16 * sizeof( UChar ) );

		m_cch32 = rhs.m_cch32;
		m_p32 = new UChar32[m_cch32];
		memcpy( m_p32, rhs.m_p32, m_cch32 * sizeof( UChar32 ) );

		return *this;
	}

	inline Utf1632Adaptor& operator=( Utf1632Adaptor&& rhs ) {
		operator=( const_cast<Utf1632Adaptor const&>( rhs ) );
		rhs._Free( );

		return *this;
	}

	//
	// Copy and move constructors and assignment operators, for type CStringW
	//

	inline Utf1632Adaptor( CStringW const& rhs ) {
		*this = rhs;
	}

	inline Utf1632Adaptor( CStringW&& rhs ) {
		*this = rhs;
	}

	inline Utf1632Adaptor& operator=( CStringW const& rhs ) {
		_Free( );

		m_cch16 = rhs.GetLength( );
		m_p16 = new UChar[m_cch16];
		memcpy( m_p16, static_cast<LPCWSTR>( rhs ), m_cch16 * sizeof( UChar ) );

		m_p32 = Utf16ToUtf32( static_cast<LPCWSTR>( rhs ), m_cch16, m_cch32 );

		return *this;
	}

	inline Utf1632Adaptor& operator=( CStringW&& rhs ) {
		operator=( const_cast<CStringW const&>( rhs ) );
		rhs.Empty( );

		return *this;
	}

	//
	// Copy constructor and assignment operator, for type UChar*
	//

	inline Utf1632Adaptor( UChar const* rhs ) {
		*this = rhs;
	}

	inline Utf1632Adaptor& operator=( UChar const* rhs ) {
		_Free( );

		m_cch16 = wcslen( rhs );
		m_p16 = new UChar[m_cch16];
		memcpy( m_p16, rhs, m_cch16 * sizeof( UChar ) );

		m_p32 = Utf16ToUtf32( rhs, m_cch16, m_cch32 );

		return *this;
	}

	//
	// Copy constructor and assignment operator, for type UChar32*
	//

	inline Utf1632Adaptor( UChar32 const* rhs ) {
		*this = rhs;
	}

	inline Utf1632Adaptor& operator=( UChar32 const* rhs ) {
		_Free( );

		m_cch32 = _QuadstringLength( rhs );
		m_p32 = new UChar32[m_cch32];
		memcpy( m_p32, rhs, m_cch32 * sizeof( UChar32 ) );

		CStringW tmp( Utf32ToUtf16( rhs, m_cch32 ) );
		m_cch16 = tmp.GetLength( );
		m_p16 = new UChar[m_cch16];
		memcpy( m_p16, static_cast<LPCWSTR>( tmp ), m_cch16 * sizeof( UChar ) );

		return *this;
	}

	//
	// Destructor
	//

	inline ~Utf1632Adaptor( ) {
		_Free( );
	}

	//
	// Cast operators
	//

	inline operator UChar*( ) const {
		return m_p16;
	}

	inline operator UChar const*( ) const {
		return m_p16;
	}

	inline operator UChar32*( ) const {
		return m_p32;
	}

	inline operator UChar32 const*( ) const {
		return m_p32;
	}

	inline operator CStringW( ) const {
		return CStringW( m_p16 );
	}

	inline operator CStringW const( ) const {
		return CStringW( m_p16 );
	}

	inline operator _bstr_t( ) const {
		return _bstr_t( m_p16 );
	}

	inline operator _bstr_t const( ) const {
		return _bstr_t( m_p16 );
	}

	inline operator _variant_t( ) const {
		return _bstr_t( m_p16 );
	}

	inline operator _variant_t const( ) const {
		return _bstr_t( m_p16 );
	}

protected:
	int m_cch16;
	int m_cch32;

	UChar* m_p16;
	UChar32* m_p32;

	inline void _Free( ) {
		if ( m_p16 ) {
			delete[] m_p16;
			m_p16 = nullptr;
			m_cch16 = 0;
		}
		if ( m_p32 ) {
			delete[] m_p32;
			m_p32 = nullptr;
			m_cch32 = 0;
		}
	}

	inline int _QuadstringLength( UChar32 const* pqz ) {
		int len = 0;
		while ( *pqz++ ) {
			len++;
		}
		return len;
	}

};
