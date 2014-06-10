#pragma once

#if defined(_MSC_FULL_VER) && _MSC_FULL_VER <= 180021005
#define constexpr
#endif

/**
* Trait to make an enum class usable as an integer
*
* C++11 adds scoped enums ('enum [class|struct]'), which is a strongly-scoped
* and -typed form of enum. It is intentionally not permitted to implicitly
* cast a scoped enumerator to an integral type, so this trait overloads the
* otherwise-useless unary + operator to cast the enum to its underlying
* integral type.
*
* \section Usage
* enum class Bar { X, Y, Z };
* DECLARE_INDEXABLE_TRAIT(Bar);
*
* Bar's members can now be used to index an array like so:
*
* SomeArray[+Bar::X]   -or-   SomeArray[AS_INDEX(Bar::X)]
*
* Sadly there is no way to use Bar as a sizing value. :(
*/

template<typename Tenum>
struct enumeration_traits;

struct enumeration_trait_indexable {
	static constexpr bool const indexable = true;
};

/**
* Use this on a scoped enum after its declaration to enable the indexing trait.
* This allows the use of operator+/AS_INDEX on it, as well as operator++, operator--.
*/
#define DECLARE_INDEXABLE_TRAIT(Tenum) template<> struct enumeration_traits<Tenum>: enumeration_trait_indexable { }

/*
* Use this _in_ a scoped enum's declaration to enable the indexing trait.
*/
#define DECLARE_INDEXABLE_ENUM(Tenum) enum class Tenum; DECLARE_INDEXABLE_TRAIT(Tenum); enum class Tenum

/**
* Use unary + to convert an enum class value into its underlying integral type.
* It may be ugly, but at least it's brief.
*/
template<typename Tenum>
constexpr typename std::enable_if<enumeration_traits<Tenum>::indexable, typename std::underlying_type<Tenum>::type>::type operator+( Tenum const value ) {
	return static_cast<typename std::underlying_type<Tenum>::type>( value );
}

/**
* Function wrapper around operator+.
*/
template<typename Tenum>
inline typename std::underlying_type<Tenum>::type AS_INDEX( Tenum const value ) {
	return +value;
}

/**
* Prefix increment operator. Don't overflow it, or weird things can happen.
*/
template<typename Tenum>
typename std::enable_if<enumeration_traits<Tenum>::indexable, Tenum&>::type operator++( Tenum& value ) {
	return ( value = static_cast<Tenum>( +value + 1 ) );
}

/**
* Postfix increment operator. Don't overflow it, or weird things can happen.
*/
template<typename Tenum>
typename std::enable_if<enumeration_traits<Tenum>::indexable, Tenum&>::type operator++( Tenum& value, int ) {
	auto temp = value;
	++value;
	return temp;
}

/**
* Prefix decrement operator. Don't underflow it, or weird things can happen.
*/
template<typename Tenum>
typename std::enable_if<enumeration_traits<Tenum>::indexable, Tenum&>::type operator--( Tenum& value ) {
	return ( value = static_cast<Tenum>( +value - 1 ) );
}

/**
* Postfix decrement operator. Don't underflow it, or weird things can happen.
*/
template<typename Tenum>
typename std::enable_if<enumeration_traits<Tenum>::indexable, Tenum&>::type operator--( Tenum& value, int ) {
	Tenum temp = value;
	--value;
	return temp;
}
