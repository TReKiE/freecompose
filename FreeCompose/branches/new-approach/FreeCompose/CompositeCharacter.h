#pragma once

class CompositeCharacter {

public:

	inline CompositeCharacter( ):
		m_pwz   ( nullptr ),
		m_pqz   ( nullptr ),
		m_error ( U_ZERO_ERROR )
	{
		m_pBreakIterator = icu::BreakIterator::createCharacterInstance( icu::Locale::getDefault( ), m_error );
	}

	inline CompositeCharacter( CompositeCharacter const& rhs ):
		CompositeCharacter ( )
	{
		operator=( rhs );
	}

	inline CompositeCharacter( CompositeCharacter&& rhs ):
		CompositeCharacter ( )

	{
		operator=( rhs );
	}

	inline ~CompositeCharacter( )
	{
		if ( m_pwz ) {
			delete m_pwz;
			m_pwz = nullptr;
		}
		if ( m_pqz ) {
			delete m_pqz;
			m_pqz = nullptr;
		}
		if ( m_pBreakIterator ) {
			delete m_pBreakIterator;
			m_pBreakIterator = nullptr;
		}
	}

	inline CompositeCharacter& operator=( CompositeCharacter const& rhs ) {
		if ( m_pwz ) {
			free( m_pwz );
		}
		if ( m_pqz ) {
			free( m_pqz );
		}
		size_t cch = wcslen( rhs.m_pwz );
		m_pwz = static_cast<UChar*>( malloc( cch * sizeof( wchar_t ) ) );
		m_pqz = static_cast<UChar32*>( malloc( cch * sizeof( UChar32 ) ) );
		memcpy( m_pwz, rhs.m_pwz, cch * sizeof( wchar_t ) );
		memcpy( m_pqz, rhs.m_pqz, cch * sizeof( UChar32 ) );
		m_unicodeString = rhs.m_unicodeString;
		return *this;
	}

	inline CompositeCharacter& operator=( CompositeCharacter&& rhs ) {
		return operator=( static_cast<CompositeCharacter const&>( rhs ) );
	}

	inline UChar const* GetUtf16( void ) {
		return m_pwz;
	}

	inline UChar32 const* GetUtf32( void ) {
		return m_pqz;
	}

	inline icu::UnicodeString GetUnicodeString( void ) {
		return icu::UnicodeString( );
	}

	inline _bstr_t GetBstrT( void ) {
		return _bstr_t( m_pwz );
	}

	inline _variant_t GetVariantT( void ) {
		return _variant_t( _bstr_t( m_pwz ) );
	}

	inline CString GetCString( void ) {
		return CString( m_pwz );
	}

	inline std::wstring GetWstring( void ) {
		return std::wstring( m_pwz );
	}

	inline LPCWSTR GetLPCWSTR( void ) {
		return m_pwz;
	}

	inline LPWSTR GetLPWSTR( void ) {
		return const_cast<LPWSTR>( m_pwz );
	}

	inline bool SetContents( CString const& contents ) {
		if ( m_pwz ) {
			delete[] m_pwz;
			m_pwz = nullptr;
		}
		if ( m_pqz ) {
			delete[] m_pqz;
			m_pqz = nullptr;
		}

		if ( contents.IsEmpty( ) ) {
			m_pwz = new UChar[1];
			m_pwz[0] = 0;

			m_pqz = new UChar32[1];
			m_pqz[0] = 0;

			m_unicodeString = icu::UnicodeString( );

			return true;
		}

		m_unicodeString = icu::UnicodeString( contents );
		m_pBreakIterator->setText( m_unicodeString );

		int32_t p1 = m_pBreakIterator->first( );
		int32_t p2 = icu::BreakIterator::DONE;
		if ( icu::BreakIterator::DONE != p1 ) {
			p2 = m_pBreakIterator->next( );
			if ( icu::BreakIterator::DONE == p2 ) {
				return false;
			}
		}

		int32_t cchStr = p2 - p1;
		m_unicodeString = icu::UnicodeString( contents, cchStr );

		m_error = U_ZERO_ERROR;
		m_pqz = new UChar32[cchStr];
		m_unicodeString.toUTF32( m_pqz, cchStr, m_error );
		if ( U_ZERO_ERROR != m_error && U_STRING_NOT_TERMINATED_WARNING != m_error ) {
			debug( L"CompositeCharacter::SetContents: result.toUTF32 failed, m_error=%d\n", m_error );
			delete[] m_pqz;
			m_pqz = nullptr;
			return false;
		}

		m_pwz = new UChar[cchStr + 1];
		memset( m_pwz, 0, sizeof(UChar) * ( cchStr + 1 ) );
		memcpy( m_pwz, m_unicodeString.getTerminatedBuffer( ), sizeof(UChar) * cchStr );

		return true;
	}

private:
	UChar* m_pwz;
	UChar32* m_pqz;
	icu::UnicodeString m_unicodeString;
	icu::BreakIterator* m_pBreakIterator;
	UErrorCode m_error;

};
