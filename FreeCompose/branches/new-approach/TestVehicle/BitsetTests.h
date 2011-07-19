#pragma once

#if ( _M_X64 || _M_AMD64 || _M_IA64 || __amd64 || __amd64__ || __x86_64 || __x86_64__ )
#define BITSET_64 1
#endif

namespace zive {

	template< typename value_type, typename element_type, const size_t index_shift = 0 >
	class _bitsetlowlevel {
	protected:
		static const size_t NBITS = std::numeric_limits<element_type>::digits + ( std::numeric_limits<element_type>::is_signed ? 1 : 0 );
		static const size_t NBYTES = ( NBITS + 7 ) / 8;
		static const size_t NELEMENTS = ( 256 + NBITS - 1 ) / NBITS;
		static const size_t INDEXSHIFT = index_shift;
		static const size_t BITMASK = ( 1 << index_shift ) - 1;

		unsigned char test( const value_type value ) const;
		unsigned char testandset( const value_type value );
		unsigned char testandreset( const value_type value );

		element_type bits[ NELEMENTS ];
	};



	template< typename value_type >
	class _bitsetlowlevel_impl_long: public _bitsetlowlevel< value_type, long, 5U > {
	private:
		typedef _bitsetlowlevel< value_type, long, 5U > _base;

	protected:
		inline unsigned char test( const value_type value ) const {
			return _bittest( _base::bits + ( value >> _base::INDEXSHIFT ), value & _base::BITMASK );
		}

		inline unsigned char testandset( const value_type value ) {
			return _bittestandset( _base::bits + ( value >> _base::INDEXSHIFT ), value & _base::BITMASK );
		}

		inline unsigned char testandreset( const value_type value ) {
			return _bittestandreset( _base::bits + ( value >> _base::INDEXSHIFT ), value & _base::BITMASK );
		}
	};

#ifdef BITSET_64
	template< typename value_type >
	class _bitsetlowlevel_impl_longlong: public _bitsetlowlevel< value_type, long long, 6U > {
	private:
		typedef _bitsetlowlevel< value_type, long long, 6U > _base;

	protected:
		inline unsigned char test( const value_type value ) const {
			return _bittest64( _base::bits + ( value >> _base::INDEXSHIFT ), value & _base::BITMASK );
		}

		inline unsigned char testandset( const value_type value ) {
			return _bittestandset64( _base::bits + ( value >> _base::INDEXSHIFT ), value & _base::BITMASK );
		}

		inline unsigned char testandreset( const value_type value ) {
			return _bittestandreset64( _base::bits + ( value >> _base::INDEXSHIFT ), value & _base::BITMASK );
		}
	};
#endif


	template< const int elements, typename value_type, typename element_type, typename _bitsetlowlevel_impl_type >
	class bitset_base: public _bitsetlowlevel_impl_type {
	public:
		inline bitset_base( ) {
			Clear( );
		}

		inline void Clear( void ) {
			memset( bits, 0, sizeof( bits ) );
			count = 0;
		}

		inline void Add( const value_type value ) {
			if ( ! testandset( value ) )
				count++;
		}

		inline void Remove( const value_type value ) {
			if ( testandreset( value ) )
				count--;
		}

		inline bool Contains( const value_type value ) const {
			return 0 != test( value );
		}

		inline bool IsEmpty( void ) const {
			return 0 == count;
		}

		inline int Count( void ) const {
			return count;
		}

	protected:
		int count;
	};



	template< const int elements, typename value_type >
	class bitset32: public bitset_base< elements, value_type, long, _bitsetlowlevel_impl_long< value_type > > { };

#ifdef BITSET_64
	template< const int elements, typename value_type >
	class bitset64: public bitset_base< elements, value_type, long long, _bitsetlowlevel_impl_longlong< value_type > > { };
#endif



	class byteset32: public bitset32< 256, uint8_t > { };

#ifdef BITSET_64
	class byteset64: public bitset64< 256, uint8_t > { };
#endif



#ifdef BITSET_64
	template< const int elements, typename value_type >
	class bitset: public bitset64< elements, value_type > { };
#else
	template< const int elements, typename value_type >
	class bitset: public bitset32< elements, value_type > { };
#endif

	class byteset: public bitset< 256, uint8_t > { };

};
