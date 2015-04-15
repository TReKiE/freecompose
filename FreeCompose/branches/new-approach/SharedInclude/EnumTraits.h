#pragma once

#if defined( _MSC_FULL_VER ) && ( _MSC_FULL_VER <= 180031101 )
#define constexpr
#endif

//
// ==========================
// || INDEXABLE ENUM TRAIT ||
// ==========================
//
// C++11 adds scoped enums ('enum struct' and 'enum class'), which is a
// strongly-scoped and -typed form of enum. It is intentionally not permitted
// to implicitly cast a scoped enumerator to an integral type, so this trait
// overloads the otherwise-useless unary operator+ to cast the enum to its
// underlying integral type.
//
// Usage
// =====
//
//    DECLARE_INDEXABLE_ENUM( Foo ) { X, Y, Z, max };
//
//        -or-
//
//    DECLARE_INDEXABLE_TYPED_ENUM( Foo, short ) { X, Y, Z, max };
//
// Foo's members can now be used to index an array like so:
//
//    SomeArray[+Foo::X]
//
// Sadly, because unary operator+ produces run-time constants, it is not
// possible to use, e.g., +Foo::max as the size of an array.
//
// ======================
// || FLAGS ENUM TRAIT ||
// ======================
//
// Overloads the bitwise operators -- & ^ | ~ -- to allow operations on enums.
//
// Usage
// =====
//
//    DECLARE_FLAGS_ENUM( Foo ) { X = 1, Y = 2, Z = 4 };
//
//        -or-
//
//    DECLARE_FLAGS_TYPED_ENUM( Foo, unsigned ) { X = 1, Y = 2, Z = 4 };
//
// Foo's members can now be used in expressions involving bitwise operators.
//
//
// Indexable enum trait inspired by user Potatoswatter's "Solution 1: Operator
// overloading."
// http://stackoverflow.com/questions/12927951/array-indexing-converting-to-integer-with-scoped-enumeration
//

template<typename Tenum>
struct enumeration_traits;

//==============================================================================
// Enum trait 'Indexable'
//==============================================================================

struct enumeration_trait_indexable {
	static constexpr bool const indexable = true;
};

#define DECLARE_INDEXABLE_ENUM( Tenum ) enum class Tenum; template<> struct enumeration_traits<Tenum>: enumeration_trait_indexable { }; enum class Tenum
#define DECLARE_INDEXABLE_TYPED_ENUM( Tenum, Tunderlying ) enum class Tenum: Tunderlying; template<> struct enumeration_traits<Tenum>: enumeration_trait_indexable { }; enum class Tenum: Tunderlying

template<typename Tenum>
inline constexpr typename std::enable_if<enumeration_traits<Tenum>::indexable, typename std::underlying_type<Tenum>::type>::type operator+( Tenum const value ) {
	return static_cast<typename std::underlying_type<Tenum>::type>( value );
}

template<typename Tenum>
inline typename std::enable_if<enumeration_traits<Tenum>::indexable, Tenum&>::type operator++( Tenum& value ) {
	return ( value = static_cast<Tenum>( +value + 1 ) );
}

template<typename Tenum>
inline typename std::enable_if<enumeration_traits<Tenum>::indexable, Tenum&>::type operator++( Tenum& value, int ) {
	auto temp = value;
	++value;
	return temp;
}

template<typename Tenum>
inline typename std::enable_if<enumeration_traits<Tenum>::indexable, Tenum&>::type operator--( Tenum& value ) {
	return ( value = static_cast<Tenum>( +value - 1 ) );
}

template<typename Tenum>
inline typename std::enable_if<enumeration_traits<Tenum>::indexable, Tenum&>::type operator--( Tenum& value, int ) {
	Tenum temp = value;
	--value;
	return temp;
}

//==============================================================================
// Enum trait 'Flags'
//==============================================================================

struct enumeration_trait_flags {
	static constexpr bool const flags = true;
};

#define DECLARE_FLAGS_ENUM(Tenum) enum class Tenum; template<> struct enumeration_traits<Tenum>: enumeration_trait_flags { }; enum class Tenum
#define DECLARE_FLAGS_TYPED_ENUM(Tenum, Tunderlying) enum class Tenum: Tunderlying; template<> struct enumeration_traits<Tenum>: enumeration_trait_flags { }; enum class Tenum: Tunderlying

template<typename Tenum>
inline constexpr typename std::enable_if<enumeration_traits<Tenum>::flags, Tenum>::type operator&( Tenum lhs, Tenum rhs ) {
	return static_cast<Tenum>( lhs & rhs );
}

template<typename Tenum>
inline constexpr typename std::enable_if<enumeration_traits<Tenum>::flags, Tenum>::type operator^( Tenum lhs, Tenum rhs ) {
	return static_cast<Tenum>( lhs ^ rhs );
}

template<typename Tenum>
inline constexpr typename std::enable_if<enumeration_traits<Tenum>::flags, Tenum>::type operator|( Tenum lhs, Tenum rhs ) {
	return static_cast<Tenum>( lhs | rhs );
}

template<typename Tenum>
inline constexpr typename std::enable_if<enumeration_traits<Tenum>::flags, Tenum>::type operator~( Tenum lhs ) {
	return static_cast<Tenum>( ~lhs );
}
