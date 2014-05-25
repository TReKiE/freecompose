#pragma once

#include <Unicode.h>

class CompositeCharacter {

public:

	inline CompositeCharacter( ):
		_pWide   ( nullptr ),
		_cchWide ( 0 ),
		_pQuad   ( nullptr ),
		_cchQuad ( 0 )
	{

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
		_FreeStrings( );
	}

	inline CompositeCharacter& operator=( CompositeCharacter const& rhs ) {
		_FreeStrings( );

		size_t cch = wcslen( rhs._pWide );
		_pWide = new UChar[cch + 1];
		_pQuad = new UChar32[cch + 1];
		memcpy( _pWide, rhs._pWide, cch * sizeof( wchar_t ) );
		memcpy( _pQuad, rhs._pQuad, cch * sizeof( UChar32 ) );
		_pWide[cch] = 0;
		_pQuad[cch] = 0;
		return *this;
	}

	inline CompositeCharacter& operator=( CompositeCharacter&& rhs ) {
		return operator=( static_cast<CompositeCharacter const&>( rhs ) );
		rhs._FreeStrings( );
	}

	inline bool operator==( CompositeCharacter const& rhs ) {
		if ( _cchWide != rhs._cchWide ) { return false; }
		return false;
	}

	inline bool operator!=( CompositeCharacter const& rhs ) {
		return !operator==( rhs );
	}

	inline UChar32 const* GetUtf32( void ) const {
		return _pQuad;
	}

	inline CString GetCString( void ) const {
		return CString( _pWide );
	}

	inline LPCWSTR GetLPCWSTR( void ) const {
		return _pWide;
	}

	inline LPWSTR GetLPWSTR( void ) const {
		return const_cast<LPWSTR>( _pWide );
	}

	inline int GetUtf16Length( void ) const {
		return _cchWide;
	}

	inline int GetUtf32Length( void ) const {
		return _cchQuad;
	}

	inline bool SetContents( CString const& input ) {
		using namespace icu;

		_FreeStrings( );

		if ( input.IsEmpty( ) ) {
			_pWide = new UChar[1];
			_pWide[0] = 0;

			_pQuad = new UChar32[1];
			_pQuad[0] = 0;

			return true;
		}

		UErrorCode errorCode = U_ZERO_ERROR;
		{
			BreakIterator* pBreakIterator = BreakIterator::createCharacterInstance( Locale::getDefault( ), errorCode );
			UnicodeString unicodeString( input );
			pBreakIterator->setText( unicodeString );

			int32_t p1 = pBreakIterator->first( );
			int32_t p2 = BreakIterator::DONE;
			if ( BreakIterator::DONE != p1 ) {
				p2 = pBreakIterator->next( );
				delete pBreakIterator;
				if ( BreakIterator::DONE == p2 ) {
					return false;
				}
			}

			_cchWide = p2 - p1;
		}

		_pWide = new UChar[_cchWide + 1];
		errno_t err = wcsncpy_s( _pWide, _cchWide + 1, static_cast<LPCWSTR>( input ), _cchWide );
		if ( 0 != err ) {
			debug( L"CompositeCharacter::SetContents: WTF, wcsncpy_s() failed? err=%d\n", err );
			_FreeStrings( );
			return false;
		}

		_pQuad = Utf16ToUtf32( _pWide, _cchWide, _cchQuad );
		if ( !_pQuad ) {
			debug( L"CompositeCharacter::SetContents: u_strToUTF32 failed, errorCode=%d\n", errorCode );
			_FreeStrings( );
			return false;
		}

		return true;
	}

private:
	int _cchWide;
	UChar* _pWide;

	int _cchQuad;
	UChar32* _pQuad;

	void _FreeStrings( void ) {
		delete[] _pWide; _pWide = nullptr; _cchWide = 0;
		delete[] _pQuad; _pQuad = nullptr; _cchQuad = 0;
	}

};
