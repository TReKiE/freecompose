#pragma once

#include <unicode/uchar.h>

namespace ATL {
	inline BOOL AtlIsValidString( UChar32 const* psz, size_t /*nMaxLength*/ ) {
		return ( psz != nullptr );
	}
}

template<>
class ChTraitsBase<UChar32> {

public:
	using XCHAR  = UChar32;
	using PXSTR  = XCHAR*;
	using PCXSTR = XCHAR const*;
	using YCHAR  = wchar_t;
	using PYSTR  = YCHAR*;
	using PCYSTR = YCHAR const*;
};

//class CQString: public CStringT<unsigned, StrTraitMFC_DLL<unsigned>> {
//
//public:
//	using BaseType = unsigned;
//	using StringTraits = StrTraitMFC_DLL<unsigned>;
//	using CThisSimpleString = CSimpleStringT<unsigned, _CSTRING_IMPL_::_MFCDLLTraitsCheck<BaseType, StringTraits>::c_bIsMFCDLLTraits>;
//
//	using XCHAR  = ChTraitsBase<UChar32>::XCHAR;
//	using PXSTR  = ChTraitsBase<UChar32>::PXSTR;
//	using PCXSTR = ChTraitsBase<UChar32>::PCXSTR;
//	using YCHAR  = ChTraitsBase<UChar32>::YCHAR;
//	using PYSTR  = ChTraitsBase<UChar32>::PYSTR;
//	using PCYSTR = ChTraitsBase<UChar32>::PCYSTR;
//
//	inline CQString( ): CStringT( ) {
//		
//	}
//
//	inline CQString( CStringW const& str ): CStringT( str ) {
//
//	}
//
//	inline CQString( CQString const& str ) {
//		*this = str;
//	}
//
//	inline CQString( PCXSTR str ): CStringT( str ) {
//
//	}
//
//	inline CQString( PCYSTR str ): CStringT( str ) {
//
//	}
//
//};

using CQString = CStringT< UChar32, StrTraitMFC_DLL< UChar32, ChTraitsCRT<UChar32> > >;
