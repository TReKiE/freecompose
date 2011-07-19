#include "stdafx.h"
#include <Bitset.h>

#include <sstream>

#define TEST(expr) do { if ( ! (expr) ) { zive::wassert( _CRT_WIDE( #expr ), _CRT_WIDE( __FILE__ ), __LINE__ ); } } while ( false )

using namespace std;
using namespace zive;

namespace zive {

	void wassert( const wchar_t* message, const wchar_t* filename, const unsigned lineno ) {
		wostringstream ss;
		ss << L"==== ASSERTION FAILURE\n" << filename << ':' << lineno << L": test expression is false: <" << message << '>' << '\n';
		wstring msg = ss.str( );
		OutputDebugString( msg.c_str( ) );
		wcerr << msg;
		wcerr.flush( );
		__debugbreak( );
	}

}

template<typename T> void Test( T& bs ) {
	wcout << L"Test for type: " << typeid( bs ).name( ) << L" -------------------" << endl;

	TEST(   bs.IsEmpty( ) );
	TEST( 0 == bs.Count( ) );
	TEST( ! bs.Contains( 2 ) );
	TEST( ! bs.Contains( 3 ) );
	TEST( ! bs.Contains( 5 ) );
	TEST( ! bs.Contains( 7 ) );

	bs.Add( 2 );
	TEST( ! bs.IsEmpty( ) );
	TEST( 1 == bs.Count( ) );
	TEST(   bs.Contains( 2 ) );
	TEST( ! bs.Contains( 3 ) );
	TEST( ! bs.Contains( 5 ) );
	TEST( ! bs.Contains( 7 ) );

	bs.Add( 3 );
	TEST( ! bs.IsEmpty( ) );
	TEST( 2 == bs.Count( ) );
	TEST(   bs.Contains( 2 ) );
	TEST(   bs.Contains( 3 ) );
	TEST( ! bs.Contains( 5 ) );
	TEST( ! bs.Contains( 7 ) );

	bs.Add( 5 );
	TEST( ! bs.IsEmpty( ) );
	TEST( 3 == bs.Count( ) );
	TEST(   bs.Contains( 2 ) );
	TEST(   bs.Contains( 3 ) );
	TEST(   bs.Contains( 5 ) );
	TEST( ! bs.Contains( 7 ) );

	bs.Add( 7 );
	TEST( ! bs.IsEmpty( ) );
	TEST( 4 == bs.Count( ) );
	TEST(   bs.Contains( 2 ) );
	TEST(   bs.Contains( 3 ) );
	TEST(   bs.Contains( 5 ) );
	TEST(   bs.Contains( 7 ) );

	TEST( ! bs.Contains( 0 ) );
	TEST( ! bs.Contains( 1 ) );
	TEST( ! bs.Contains( 4 ) );
	TEST( ! bs.Contains( 6 ) );
	TEST( ! bs.Contains( 8 ) );
	TEST( ! bs.Contains( 9 ) );

	bs.Add( 32 );
	TEST( ! bs.IsEmpty( ) );
	TEST( 5 == bs.Count( ) );
	TEST(   bs.Contains( 2 ) );
	TEST(   bs.Contains( 3 ) );
	TEST(   bs.Contains( 5 ) );
	TEST(   bs.Contains( 7 ) );
	TEST( ! bs.Contains( 31 ) );
	TEST(   bs.Contains( 32 ) );
	TEST( ! bs.Contains( 33 ) );

	bs.Add( 64 );
	TEST( ! bs.IsEmpty( ) );
	TEST( 6 == bs.Count( ) );
	TEST(   bs.Contains( 2 ) );
	TEST(   bs.Contains( 3 ) );
	TEST(   bs.Contains( 5 ) );
	TEST(   bs.Contains( 7 ) );
	TEST(   bs.Contains( 32 ) );
	TEST(   bs.Contains( 64 ) );

	bs.Remove( 131 );
	TEST( ! bs.IsEmpty( ) );
	TEST( 6 == bs.Count( ) );
	TEST(   bs.Contains( 2 ) );
	TEST(   bs.Contains( 3 ) );
	TEST(   bs.Contains( 5 ) );
	TEST(   bs.Contains( 7 ) );
	TEST(   bs.Contains( 32 ) );
	TEST(   bs.Contains( 64 ) );

	bs.Remove( 3 );
	TEST( ! bs.IsEmpty( ) );
	TEST( 5 == bs.Count( ) );
	TEST(   bs.Contains( 2 ) );
	TEST( ! bs.Contains( 3 ) );
	TEST(   bs.Contains( 5 ) );
	TEST(   bs.Contains( 7 ) );
	TEST(   bs.Contains( 32 ) );
	TEST(   bs.Contains( 64 ) );

	bs.Remove( 3 );
	TEST( ! bs.IsEmpty( ) );
	TEST( 5 == bs.Count( ) );
	TEST(   bs.Contains( 2 ) );
	TEST( ! bs.Contains( 3 ) );
	TEST(   bs.Contains( 5 ) );
	TEST(   bs.Contains( 7 ) );
	TEST(   bs.Contains( 32 ) );
	TEST(   bs.Contains( 64 ) );

	bs.Remove( 7 );
	TEST( ! bs.IsEmpty( ) );
	TEST( 4 == bs.Count( ) );
	TEST(   bs.Contains( 2 ) );
	TEST( ! bs.Contains( 3 ) );
	TEST(   bs.Contains( 5 ) );
	TEST( ! bs.Contains( 7 ) );
	TEST(   bs.Contains( 32 ) );
	TEST(   bs.Contains( 64 ) );

	bs.Remove( 2 );
	TEST( ! bs.IsEmpty( ) );
	TEST( 3 == bs.Count( ) );
	TEST( ! bs.Contains( 2 ) );
	TEST( ! bs.Contains( 3 ) );
	TEST(   bs.Contains( 5 ) );
	TEST( ! bs.Contains( 7 ) );
	TEST(   bs.Contains( 32 ) );
	TEST(   bs.Contains( 64 ) );

	bs.Clear( );
	TEST(   bs.IsEmpty( ) );
	TEST( 0 == bs.Count( ) );
	TEST( ! bs.Contains( 2 ) );
	TEST( ! bs.Contains( 3 ) );
	TEST( ! bs.Contains( 5 ) );
	TEST( ! bs.Contains( 7 ) );
	TEST( ! bs.Contains( 32 ) );
	TEST( ! bs.Contains( 64 ) );
}

void BitsetTests( void ) {
	TEST( false );

	byteset32 bs32;
	Test( bs32 );

#ifdef BITSET_64
	byteset64 bs64;
	Test( bs64 );
#endif
}
