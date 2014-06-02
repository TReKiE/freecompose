#include "stdafx.h"

#include <Unicode.h>

#include "Utils.h"

#include "OptionsData.h"
#include "XmlOptionsManager.h"
#include "XmlOptionsReader.h"
#include "XmlOptionsHelpers.h"

//==============================================================================
// Type aliases
//==============================================================================

using MethodPtr        = bool (CXmlOptionsReader::*)( XNode const& );
using XmlMethodMap     = std::map<_bstr_t, MethodPtr>;
using XmlMethodMapPair = std::pair<_bstr_t, MethodPtr>;

//==============================================================================
// Local objects
//==============================================================================

static XmlMethodMap RootElementsToMethods;
static XmlMethodMap     OptionsElementsToMethods;
static XmlMethodMap         StartupElementsToMethods;
static XmlMethodMap         KeyboardElementsToMethods;
static XmlMethodMap         SoundsElementsToMethods;
static XmlMethodMap             SchemeElementsToMethods;
static XmlMethodMap                 SoundEventElementsToMethods;
static XmlMethodMap     MappingsElementsToMethods;
static XmlMethodMap         GroupElementsToMethods;

//==============================================================================
// Static initialization object for this translation unit
//==============================================================================

static class XorInitializer {
public:
	inline XorInitializer( ) {
		RootElementsToMethods      .insert( XmlMethodMapPair( L"Options",            &CXmlOptionsReader::_InterpretOptionsNode            ) );
		RootElementsToMethods      .insert( XmlMethodMapPair( L"Mappings",           &CXmlOptionsReader::_InterpretMappingsNode           ) );

		OptionsElementsToMethods   .insert( XmlMethodMapPair( L"Startup",            &CXmlOptionsReader::_InterpretStartupNode            ) );
		OptionsElementsToMethods   .insert( XmlMethodMapPair( L"Keyboard",           &CXmlOptionsReader::_InterpretKeyboardNode           ) );
		OptionsElementsToMethods   .insert( XmlMethodMapPair( L"Sounds",             &CXmlOptionsReader::_InterpretSoundsNode             ) );

		StartupElementsToMethods   .insert( XmlMethodMapPair( L"StartActive",        &CXmlOptionsReader::_InterpretStartActiveNode        ) );
		StartupElementsToMethods   .insert( XmlMethodMapPair( L"StartWithWindows",   &CXmlOptionsReader::_InterpretStartWithWindowsNode   ) );

		KeyboardElementsToMethods  .insert( XmlMethodMapPair( L"CapsLockToggleMode", &CXmlOptionsReader::_InterpretCapsLockToggleModeNode ) );
		KeyboardElementsToMethods  .insert( XmlMethodMapPair( L"CapsLockSwapMode",   &CXmlOptionsReader::_InterpretCapsLockSwapModeNode   ) );
		KeyboardElementsToMethods  .insert( XmlMethodMapPair( L"ComposeKey",         &CXmlOptionsReader::_InterpretComposeKeyNode         ) );
		KeyboardElementsToMethods  .insert( XmlMethodMapPair( L"SwapCapsLockKey",    &CXmlOptionsReader::_InterpretSwapCapsLockKeyNode    ) );

		SoundsElementsToMethods    .insert( XmlMethodMapPair( L"Scheme",             &CXmlOptionsReader::_InterpretSchemeNode             ) );

		SchemeElementsToMethods    .insert( XmlMethodMapPair( L"SoundEvent",         &CXmlOptionsReader::_InterpretSoundEventNode         ) );

		SoundEventElementsToMethods.insert( XmlMethodMapPair( L"NoSound",            &CXmlOptionsReader::_InterpretNoSoundNode            ) );
		SoundEventElementsToMethods.insert( XmlMethodMapPair( L"ApplicationSound",   &CXmlOptionsReader::_InterpretApplicationSoundNode   ) );
		SoundEventElementsToMethods.insert( XmlMethodMapPair( L"Tone",               &CXmlOptionsReader::_InterpretToneNode               ) );

		MappingsElementsToMethods  .insert( XmlMethodMapPair( L"Group",              &CXmlOptionsReader::_InterpretGroupNode              ) );

		GroupElementsToMethods     .insert( XmlMethodMapPair( L"Mapping",            &CXmlOptionsReader::_InterpretMappingNode            ) );
	}
} _XorInitializer_Instance;

//==============================================================================
// Helper methods
//==============================================================================

template<typename Tout>
static inline Tout CoerceXNode( XNode const& value ) {
	return Coerce<_bstr_t, Tout>( value->text );
}

static inline bool CompareNodeName( XNode elt, LPCWSTR name ) {
	return 0 == CString( static_cast<LPCWSTR>( elt->nodeName ) ).Compare( name );
}

static inline bool SafeXNodeToBool( XNode const& pNode, bool const fDefault = false ) {
	if ( !pNode || !pNode->text ) {
		return fDefault;
	}
	return BoolStringMapper[pNode->text];
}

static inline long WcsToL( CString const& input, int const radix ) {
	wchar_t* pEnd = nullptr;
	wchar_t const* pInput = input;

	errno = 0;
	long result = wcstol( pInput, &pEnd, radix );

	if ( !pEnd ) {
		// wtf?
		debug( L"WcsToL: pEnd is nullptr?? (result=%lu)\n", result );
		return result;
	}
	if ( pInput == pEnd ) {
		// no conversion took place at all
		debug( L"WcsToL: conversion totally failed. (result=%lu)\n", result );
		return result;
	}
	if ( *pEnd != L'\0' && *pEnd != L',' ) {
		debug( L"WcsToL: conversion stopped by character %u. (result=0)\n", *pEnd );
		return 0;
	}
	return result;
}

static inline CString HexadecimalToString( CString const& input ) {
	CArray<UChar32> chars;

	int lastindex = 0;
	int index = input.Find( L',', 0 );
	while ( index > -1 ) {
		CString token( input.Mid( lastindex, index ) );
		wchar_t charval = static_cast<wchar_t>( WcsToL( token, 16 ) );
		chars.Add( charval );
		lastindex = index + 1;
		index = input.Find( L',', index + 1 );
	}
	if ( lastindex < input.GetLength( ) - 1 ) {
		CString token( input.Mid( lastindex ) );
		wchar_t charval = static_cast<wchar_t>( WcsToL( token, 16 ) );
		chars.Add( charval );
	}

	return Utf32ToUtf16( chars.GetData( ), chars.GetCount( ) );
}

static inline bool ComposeSequenceFromXNode( XNode const& value, ComposeSequence& result ) {
	XNode nodeFirst, nodeSecond, nodeComposed;
	XNode nodeSequence, nodeResult;
	CString Sequence, Result;

	XNode node = value->firstChild;
	while ( node ) {
		if      ( 0 == wcscmp( L"First",    node->nodeName ) ) { nodeFirst    = node; }
		else if ( 0 == wcscmp( L"Second",   node->nodeName ) ) { nodeSecond   = node; }
		else if ( 0 == wcscmp( L"Composed", node->nodeName ) ) { nodeComposed = node; }
		else if ( 0 == wcscmp( L"Sequence", node->nodeName ) ) { nodeSequence = node; }
		else if ( 0 == wcscmp( L"Result",   node->nodeName ) ) { nodeResult   = node; }
		else {
			debug( L"ComposeSequenceFromXNode: unknown node '%s'\n", static_cast<LPCWSTR>( node->nodeName ) );
		}
		node = node->nextSibling;
	}

	bool disabled = false;
	bool caseInsensitive = false;
	bool reversible = false;

	if ( nodeFirst && nodeSecond && nodeComposed ) {
		debug( L"ComposeSequenceFromXNode: Translating sequence\n" );
		CString First, Second, Composed;
		First    =   VkToString( static_cast<unsigned>( static_cast<_variant_t>( nodeFirst->text    ) ) );
		Second   =   VkToString( static_cast<unsigned>( static_cast<_variant_t>( nodeSecond->text   ) ) );
		Composed = Utf32ToUtf16( static_cast<unsigned>( static_cast<_variant_t>( nodeComposed->text ) ) );

		Sequence = First + Second;
		Result   = Composed;
	} else if ( nodeSequence && nodeResult ) {
		Sequence = static_cast<LPCWSTR>( nodeSequence->text );
		Result   = static_cast<LPCWSTR>( nodeResult->text   );

		Sequence = HexadecimalToString( Sequence );
		Result   = HexadecimalToString( Result   );

		try {
			XNamedNodeMap attributes = value->attributes;
			if ( attributes ) {
				disabled        = SafeXNodeToBool( attributes->getNamedItem( L"Disabled"        ) );
				caseInsensitive = SafeXNodeToBool( attributes->getNamedItem( L"CaseInsensitive" ) );
				reversible      = SafeXNodeToBool( attributes->getNamedItem( L"Reversible"      ) );
			}
		}
		catch ( _com_error e ) {
			debug( L"ComposeSequenceFromXNode: Caught COM error exception while parsing configuration, hr=0x%08lX '%s'\n", e.Error( ), e.ErrorMessage( ) );
			return false;
		}
		catch ( ... ) {
			debug( L"ComposeSequenceFromXNode: caught some other kind of exception??\n" );
			return false;
		}
	} else {
		debug( L"ComposeSequenceFromXNode: invalid entry\n" );
		return false;
	}

	result = ComposeSequence( Sequence, Result, disabled, caseInsensitive, reversible );
	return true;
}

//==============================================================================
// Implementation
//==============================================================================

//
// Dispatch methods
//

bool CXmlOptionsReader::_DispatchChildren( wchar_t const* label, XNode const& node, XmlMethodMap& map ) {
	XNode child = node->firstChild;
	while ( child ) {
		if ( !_DispatchNode( label, child, map ) ) {
			debug( L"CXmlOptionsReader::_DispatchChildren: %s: _DispatchNode failed on node '%s'\n", label, static_cast<LPCWSTR>( child->nodeName ) );
		}
		child = child->nextSibling;
	}
	return true;
}

bool CXmlOptionsReader::_DispatchNode( wchar_t const* label, XNode const& node, XmlMethodMap& map ) {
	MethodPtr methodPtr = map[node->nodeName];
	if ( !methodPtr ) {
		debug( L"CXmlOptionsReader::_DispatchNode: %s: No method to call for node '%s'\n", label, static_cast<LPCWSTR>( node->nodeName ) );
		return false;
	}
	return ( this->*methodPtr )( node );
}

//
// Interpretation methods
//

bool CXmlOptionsReader::InterpretConfiguration( XDocument& doc ) {
	try {
		XElement FcConfiguration = doc->documentElement;
		if ( !CompareNodeName( FcConfiguration, L"FcConfiguration" ) ) {
			debug( L"CXmlOptionsReader::_InterpretConfiguration: document element is not <FcConfiguration>, aborting load\n" );
			return false;
		}
		_bstr_t xmlns = FcConfiguration->namespaceURI;
		LPWSTR pwzXmlns = static_cast<LPWSTR>( xmlns );
		if ( 0 != CString( pwzXmlns ).Compare( XML_NAMESPACE ) ) {
			debug( L"CXmlOptionsReader::_InterpretConfiguration: namespace contains unexpected URI\n+ Our namespace:        '%s'\n+ Document's namespace: '%s'\n", XML_NAMESPACE, pwzXmlns );
			return false;
		}

		_DispatchChildren( L"root", FcConfiguration, RootElementsToMethods );
	}
	catch ( _com_error e ) {
		debug( L"CXmlOptionsReader::_InterpretConfiguration: Caught COM error exception while parsing configuration, hr=0x%08lX '%s'\n", e.Error( ), e.ErrorMessage( ) );
		return false;
	}
	catch ( ... ) {
		debug( L"CXmlOptionsReader::_InterpretConfiguration: caught some other kind of exception??\n" );
		return false;
	}

	return true;
}

bool CXmlOptionsReader::_InterpretOptionsNode( XNode const& node ) {
	return _DispatchChildren( L"Options", node, OptionsElementsToMethods );
}

bool CXmlOptionsReader::_InterpretStartupNode( XNode const& node ) {
	return _DispatchChildren( L"Options\\Startup", node, StartupElementsToMethods );
}

bool CXmlOptionsReader::_InterpretStartActiveNode( XNode const& node ) {
	_pOptionsData->StartActive = BoolStringMapper[node->text];
	return true;
}

bool CXmlOptionsReader::_InterpretStartWithWindowsNode( XNode const& node ) {
	_pOptionsData->StartWithWindows = BoolStringMapper[node->text];
	return true;
}

bool CXmlOptionsReader::_InterpretKeyboardNode( XNode const& node ) {
	return _DispatchChildren( L"Options\\Keyboard", node, KeyboardElementsToMethods );
}

bool CXmlOptionsReader::_InterpretCapsLockToggleModeNode( XNode const& node ) {
	_pOptionsData->CapsLockToggleMode = CltmStringMapper[node->text];
	return true;
}

bool CXmlOptionsReader::_InterpretCapsLockSwapModeNode( XNode const& node ) {
	_pOptionsData->CapsLockSwapMode = ClsmStringMapper[node->text];
	return true;
}

bool CXmlOptionsReader::_InterpretComposeKeyNode( XNode const& node ) {
	_pOptionsData->ComposeVk = CoerceXNode<unsigned>( node );
	return true;
}

bool CXmlOptionsReader::_InterpretSwapCapsLockKeyNode( XNode const& node ) {
	_pOptionsData->SwapCapsLockVk = CoerceXNode<unsigned>( node );
	return true;
}

bool CXmlOptionsReader::_InterpretSoundsNode( XNode const& node ) {
	return _DispatchChildren( L"Sounds", node, SoundsElementsToMethods );
}

bool CXmlOptionsReader::_InterpretSchemeNode( XNode const& node ) {
	CString id, name;
	if ( node ) {
		XNamedNodeMap attributes = node->attributes;
		if ( attributes ) {
			XNode ID = attributes->getNamedItem( L"ID" );
			if ( ID ) {
				id = static_cast<LPCWSTR>( ID->text );
			}
			XNode Name = attributes->getNamedItem( L"Name" );
			if ( Name ) {
				name = static_cast<LPCWSTR>( Name->text );
			}
		}
	}

	_pOptionsData->Sounds.Schemes.push_back( SoundScheme( id, name ) );
	_currentSoundSchemeName = name;
	bool ret = _DispatchChildren( L"Sounds\\Scheme", node, SchemeElementsToMethods );
	_currentSoundSchemeName.Empty( );
	return ret;
}

bool CXmlOptionsReader::_InterpretSoundEventNode( XNode const& node ) {
	CString name;
	if ( node ) {
		XNamedNodeMap attributes = node->attributes;
		if ( attributes ) {
			XNode Name = attributes->getNamedItem( L"Name" );
			if ( Name ) {
				name = static_cast<LPCWSTR>( Name->text );
			}
		}
	}

	_currentSoundEventName = name;
	bool ret = _DispatchChildren( L"Sounds\\Scheme\\SoundEvent", node, SoundEventElementsToMethods );
	_currentSoundEventName.Empty( );
	return ret;
}

bool CXmlOptionsReader::_InterpretNoSoundNode( XNode const& /*node*/ ) {
	if ( _currentSoundSchemeName.IsEmpty( ) || _currentSoundEventName.IsEmpty( ) ) {
		debug( L"CXmlOptionsReader::_InterpretNoSoundNode: problem: one or both of _currentSoundSchemeName ('%s') and _currentSoundEventName ('%s') is empty?\n", static_cast<LPCWSTR>( _currentSoundSchemeName ), static_cast<LPCWSTR>( _currentSoundEventName ) );
		return false;
	}

	SoundScheme* pScheme = _pOptionsData->Sounds.GetSoundScheme( _currentSoundSchemeName );
	if ( pScheme ) {
		pScheme->Events.insert( SoundEventMapPair( _currentSoundEventName, new NoSoundEvent( _currentSoundEventName ) ) );
	}

	return true;
}

bool CXmlOptionsReader::_InterpretApplicationSoundNode( XNode const& /*node*/ ) {
	if ( _currentSoundSchemeName.IsEmpty( ) || _currentSoundEventName.IsEmpty( ) ) {
		debug( L"CXmlOptionsReader::_InterpretApplicationSoundNode: problem: one or both of _currentSoundSchemeName ('%s') and _currentSoundEventName ('%s') is empty?\n", static_cast<LPCWSTR>( _currentSoundSchemeName ), static_cast<LPCWSTR>( _currentSoundEventName ) );
		return false;
	}

	SoundScheme* pScheme = _pOptionsData->Sounds.GetSoundScheme( _currentSoundSchemeName );
	if ( pScheme ) {
		pScheme->Events.insert( SoundEventMapPair( _currentSoundEventName, new ApplicationSoundEvent( _currentSoundEventName ) ) );
	}

	return true;
}

bool CXmlOptionsReader::_InterpretToneNode( XNode const& node ) {
	if ( _currentSoundSchemeName.IsEmpty( ) || _currentSoundEventName.IsEmpty( ) ) {
		debug( L"CXmlOptionsReader::_InterpretToneNode: problem: one or both of _currentSoundSchemeName ('%s') and _currentSoundEventName ('%s') is empty?\n", static_cast<LPCWSTR>( _currentSoundSchemeName ), static_cast<LPCWSTR>( _currentSoundEventName ) );
		return false;
	}

	unsigned frequency = 0;
	unsigned duration = 0;

	if ( node ) {
		XNamedNodeMap attributes = node->attributes;
		if ( attributes ) {
			XNode Frequency = attributes->getNamedItem( L"Frequency" );
			if ( Frequency ) {
				frequency = static_cast<unsigned>( _variant_t( Frequency->text ) );
			}
			XNode Duration = attributes->getNamedItem( L"Duration" );
			if ( Duration ) {
				duration = static_cast<unsigned>( _variant_t( Duration->text ) );
			}
		}
	}

	if ( 0 == frequency && 0 == duration ) {
		debug( L"CXmlOptionsReader::_InterpretToneNode: Frequency and Duration must both be positive values\n" );
		return false;
	}

	SoundScheme* pScheme = _pOptionsData->Sounds.GetSoundScheme( _currentSoundSchemeName );
	if ( pScheme ) {
		pScheme->Events.insert( SoundEventMapPair( _currentSoundEventName, new ToneSoundEvent( _currentSoundEventName, frequency, duration ) ) );
	}

	return true;
}

bool CXmlOptionsReader::_InterpretMappingsNode( XNode const& node ) {
	return _DispatchChildren( L"Mappings", node, MappingsElementsToMethods );
}

bool CXmlOptionsReader::_InterpretGroupNode( XNode const& node ) {
	CString groupName;
	if ( node ) {
		XNamedNodeMap attributes = node->attributes;
		if ( attributes ) {
			XNode Name = attributes->getNamedItem( L"Name" );
			if ( Name ) {
				groupName = static_cast<LPCWSTR>( Name->text );
				if ( groupName.IsEmpty( ) ) {
					groupName = LoadFromStringTable( IDS_DEFAULT_GROUP_NAME );
				}
			}
		}
	}

	ComposeSequenceGroup* pCurrentGroup = _pOptionsData->FindComposeSequenceGroup( groupName );
	if ( !pCurrentGroup ) {
		_pOptionsData->ComposeSequenceGroups.Add( ComposeSequenceGroup( groupName ) );
		pCurrentGroup = _pOptionsData->FindComposeSequenceGroup( groupName );
	}

	_pCurrentComposeSequences = &pCurrentGroup->ComposeSequences;
	bool fRet = _DispatchChildren( L"Mappings\\Group", node, GroupElementsToMethods );
	_pCurrentComposeSequences = nullptr;

	return fRet;
}

bool CXmlOptionsReader::_InterpretMappingNode( XNode const& node ) {
	ComposeSequence sequence;
	if ( ComposeSequenceFromXNode( node, sequence ) ) {
		_pCurrentComposeSequences->Add( sequence );
		return true;
	} else {
		return false;
	}
}
