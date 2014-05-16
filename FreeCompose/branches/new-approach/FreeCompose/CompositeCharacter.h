#pragma once

class CompositeCharacter {

public:

	inline CompositeCharacter( ):
		m_pwzUtf16 ( nullptr ),
		m_pqzUtf32 ( nullptr ),
		m_error    ( U_ZERO_ERROR )
	{
		m_pBreakIterator = icu::BreakIterator::createCharacterInstance( icu::Locale::getDefault( ), m_error );
	}

	inline CompositeCharacter( CompositeCharacter const& /*rhs*/ ):
		CompositeCharacter ( )
	{
		
	}

	inline CompositeCharacter( CompositeCharacter&& /*rhs*/ ):
		CompositeCharacter ( )

	{
		
	}

	inline ~CompositeCharacter( )
	{
		if ( m_pBreakIterator ) {
			delete m_pBreakIterator;
			m_pBreakIterator = nullptr;
		}
	}

	inline CompositeCharacter& operator=( CompositeCharacter const& /*rhs*/ ) {

	}

	inline CompositeCharacter& operator=( CompositeCharacter&& /*rhs*/ ) {

	}

	inline UChar const* GetUtf16( void ) const {
		return nullptr;
	}

	inline UChar32 const* GetUtf32( void ) const {
		return nullptr;
	}

	inline icu::UnicodeString GetUnicodeString( void ) const {
		return icu::UnicodeString( );
	}

	inline _bstr_t GetBstrT( void ) const {
		return _bstr_t( );
	}

	inline _variant_t GetVariantT( void ) const {
		return _variant_t( );
	}

	inline CString GetCString( void ) const {
		return CString( );
	}

	inline std::wstring GetWstring( void ) const {
		return std::wstring( );
	}

	inline PCWSTR GetPCWSTR( void ) const {
		return nullptr;
	}

	inline PWSTR GetPWSTR( void ) const {
		return nullptr;
	}

	inline bool SetContents( icu::UnicodeString const& contents ) {
		if ( contents.isEmpty( ) ) {
			return false;
		}

		if ( m_pwzUtf16 ) {
			delete[] m_pwzUtf16;
			m_pwzUtf16 = nullptr;
		}
		if ( m_pqzUtf32 ) {
			delete[] m_pqzUtf32;
			m_pqzUtf32 = nullptr;
		}

		m_unicodeString = contents;
		m_pBreakIterator->setText( m_unicodeString );

		int32_t p1 = m_pBreakIterator->first( );
		int32_t p2 = icu::BreakIterator::DONE;
		if ( icu::BreakIterator::DONE != p1 ) {
			p2 = m_pBreakIterator->next( );
		}

		if ( icu::BreakIterator::DONE == p1 ) {
			return false;
		}

		int32_t cchStr = p2 - p1;
		m_unicodeString = icu::UnicodeString( contents, cchStr );

		m_error = U_ZERO_ERROR;
		m_pqzUtf32 = new UChar32[cchStr];
		m_unicodeString.toUTF32( m_pqzUtf32, cchStr, m_error );
		if ( U_ZERO_ERROR != m_error ) {
			debug( L"CompositeCharacter::SetContents: result.toUTF32 failed, m_error=%d\n", m_error );
			delete[] m_pqzUtf32;
			m_pqzUtf32 = nullptr;
			return false;
		}

		m_pwzUtf16 = new UChar[cchStr];
		memcpy( m_pwzUtf16, m_unicodeString.getBuffer( ), sizeof(wchar_t) * cchStr );

		return true;
	}

private:
	UChar* m_pwzUtf16;
	UChar32* m_pqzUtf32;
	icu::UnicodeString m_unicodeString;
	icu::BreakIterator* m_pBreakIterator;
	UErrorCode m_error;

};
