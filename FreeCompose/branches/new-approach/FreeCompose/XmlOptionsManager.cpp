#include "stdafx.h"

#include "FreeCompose.h"
#include "OptionsData.h"
#include "StringMapper.h"
#include "Utils.h"

#include <Unicode.h>

//==============================================================================
// Constants
//==============================================================================

int const CONFIGURATION_SCHEMA_VERSION = 1;
wchar_t const XML_NAMESPACE[] = L"http://www.zive.ca/xmlns/FreeCompose/configuration/1";

//==============================================================================
// Local objects
//==============================================================================

static const StringMapper<bool> BoolStringMapper {
	L"false",
	L"true",
};

static const StringMapper<CAPS_LOCK_TOGGLE_MODE> CltmStringMapper {
	static_cast<LPCWSTR>( nullptr ),
	L"normal",
	L"pressTwice",
	L"disabled",
};

static const StringMapper<CAPS_LOCK_SWAP_MODE> ClsmStringMapper {
	static_cast<LPCWSTR>( nullptr ),
	L"normal",
	L"swap",
	L"replace",
};

static XmlMethodMap RootElementsToMethods;
static XmlMethodMap     OptionsElementsToMethods;
static XmlMethodMap         StartupOptionsElementsToMethods;
static XmlMethodMap         KeyboardOptionsElementsToMethods;
static XmlMethodMap     MappingsElementsToMethods;
static XmlMethodMap         GroupMappingsElementsToMethods;

//==============================================================================
// Static initialization object for this translation unit
//==============================================================================

class Initializer_ {
public:
	inline Initializer_( ) {
		RootElementsToMethods           .insert( XmlMethodMapPair( L"SchemaVersion",      &CXmlOptionsManager::_InterpretSchemaVersionNode      ) );
		RootElementsToMethods           .insert( XmlMethodMapPair( L"Options",            &CXmlOptionsManager::_InterpretOptionsNode            ) );
		RootElementsToMethods           .insert( XmlMethodMapPair( L"Mappings",           &CXmlOptionsManager::_InterpretMappingsNode           ) );

		OptionsElementsToMethods        .insert( XmlMethodMapPair( L"Startup",            &CXmlOptionsManager::_InterpretStartupNode            ) );
		OptionsElementsToMethods        .insert( XmlMethodMapPair( L"Keyboard",           &CXmlOptionsManager::_InterpretKeyboardNode           ) );

		StartupOptionsElementsToMethods .insert( XmlMethodMapPair( L"StartActive",        &CXmlOptionsManager::_InterpretStartActiveNode        ) );
		StartupOptionsElementsToMethods .insert( XmlMethodMapPair( L"StartWithWindows",   &CXmlOptionsManager::_InterpretStartWithWindowsNode   ) );

		KeyboardOptionsElementsToMethods.insert( XmlMethodMapPair( L"CapsLockToggleMode", &CXmlOptionsManager::_InterpretCapsLockToggleModeNode ) );
		KeyboardOptionsElementsToMethods.insert( XmlMethodMapPair( L"CapsLockSwapMode",   &CXmlOptionsManager::_InterpretCapsLockSwapModeNode   ) );
		KeyboardOptionsElementsToMethods.insert( XmlMethodMapPair( L"ComposeKey",         &CXmlOptionsManager::_InterpretComposeKeyNode         ) );
		KeyboardOptionsElementsToMethods.insert( XmlMethodMapPair( L"SwapCapsLockKey",    &CXmlOptionsManager::_InterpretSwapCapsLockKeyNode    ) );

		MappingsElementsToMethods       .insert( XmlMethodMapPair( L"Group",              &CXmlOptionsManager::_InterpretGroupNode              ) );

		GroupMappingsElementsToMethods  .insert( XmlMethodMapPair( L"Mapping",            &CXmlOptionsManager::_InterpretMappingNode            ) );
	}
};
static Initializer_ Instance_;

//==============================================================================
// Local functions
//==============================================================================

// 'Coerce' functions

template<typename Tin, typename Tout>
static inline Tout Coerce( Tin const& in ) {
	return static_cast<Tout>( _variant_t( in ) );
}

template<typename Tout>
static inline Tout CoerceXNode( XNode const& value ) {
	return Coerce<_bstr_t, Tout>( value->text );
}

static inline bool SafeXNodeToBool( XNode const& pNode, bool const fDefault = false ) {
	if ( !pNode || !pNode->text ) {
		return fDefault;
	}
	return BoolStringMapper[pNode->text];
}

// 'FromXNode' functions

inline bool CXmlOptionsManager::_ComposeSequenceFromXNode( XNode const& value, ComposeSequence& result ) {
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
			debug( L"CXmlOptionsManager::_ComposeSequenceFromXNode: unknown node '%s'\n", static_cast<LPCWSTR>( node->nodeName ) );
		}
		node = node->nextSibling;
	}

	bool disabled = false;
	bool caseInsensitive = false;
	bool reversible = false;

	if ( nodeFirst && nodeSecond && nodeComposed ) {
		debug( L"CXmlOptionsManager::_ComposeSequenceFromXNode: Translating sequence\n" );
		CString First, Second, Composed;
		First    =   VkToString( static_cast<unsigned>( static_cast<_variant_t>( nodeFirst->text    ) ) );
		Second   =   VkToString( static_cast<unsigned>( static_cast<_variant_t>( nodeSecond->text   ) ) );
		Composed = Utf32ToUtf16( static_cast<unsigned>( static_cast<_variant_t>( nodeComposed->text ) ) );

		Sequence = First + Second;
		Result   = Composed;
	} else if ( nodeSequence && nodeResult ) {
		Sequence = static_cast<LPCWSTR>( nodeSequence->text );
		Result   = static_cast<LPCWSTR>( nodeResult->text );

		try {
			XNamedNodeMap attributes = value->attributes;
			if ( attributes ) {
				disabled        = SafeXNodeToBool( attributes->getNamedItem( L"Disabled"        ) );
				caseInsensitive = SafeXNodeToBool( attributes->getNamedItem( L"CaseInsensitive" ) );
				reversible      = SafeXNodeToBool( attributes->getNamedItem( L"Reversible"      ) );
			}
		}
		catch ( _com_error e ) {
			debug( L"CXmlOptionsManager::_ComposeSequenceFromXNode: Caught COM error exception while parsing configuration, hr=0x%08lX '%s'\n", e.Error( ), e.ErrorMessage( ) );
			return false;
		}
		catch ( ... ) {
			debug( L"CXmlOptionsManager::_ComposeSequenceFromXNode: caught some other kind of exception??\n" );
			return false;
		}
	} else {
		debug( L"CXmlOptionsManager::_ComposeSequenceFromXNode: invalid entry\n" );
		return false;
	}

	result = ComposeSequence( Sequence, Result, disabled, caseInsensitive, reversible );
	return true;
}

// CreateXNode functions

template<typename Tparent>
static inline XNode CreateAndAppendXNode( XDocument& doc, _bstr_t const& tagName, Tparent& parent ) {
	XNode node = doc->createNode( _variant_t( NODE_ELEMENT ), tagName, XML_NAMESPACE );
	parent->appendChild( node );
	return node;
}

template<typename Tparent, typename Tvalue>
static inline XNode CreateAndAppendXNode( XDocument& doc, _bstr_t const& tagName, Tparent& parent, Tvalue const& value ) {
	XNode node = CreateAndAppendXNode( doc, tagName, parent );
	if ( node ) {
		node->text = Coerce<Tvalue, _bstr_t>( value );
	}
	return node;
}

// Other functions

static inline bool CompareNodeName( XNode elt, LPCWSTR name ) {
	return 0 == CString( static_cast<LPCWSTR>( elt->nodeName ) ).Compare( name );
}

static inline XDocument CreateDOMDocument( void ) {
	XDocument doc;
	HRESULT hr = doc.CreateInstance( L"Msxml2.DOMDocument.6.0" );
	if ( FAILED( hr ) ) {
		debug( L"CreateDOMDocument: failed, hr=0x%08lX\n", hr );
		return doc;
	}

	try {
		doc->async = VARIANT_FALSE;
		doc->validateOnParse = VARIANT_FALSE;
		doc->resolveExternals = VARIANT_FALSE;
		doc->preserveWhiteSpace = VARIANT_FALSE;
	}
	catch ( _com_error e ) {
		debug( L"CreateDOMDocument: Caught exception setting up DOMDocument, hr=0x%08lX\n", e.Error( ) );
		doc.Release( );
	}
	return doc;
}

// Resource loading

static inline bool LoadBinaryResource( unsigned uID, void*& pvResource, size_t& cbResource ) {
	HRSRC hrsrc = FindResource( nullptr, MAKEINTRESOURCE( uID ), L"XMLFILE" );
	if ( !hrsrc ) {
		debug( L"LoadBinaryResourceAsBstr: FindResource failed, error is %lu", GetLastError( ) );
		return false;
	}

	HGLOBAL hglob = LoadResource( nullptr, hrsrc );
	if ( !hglob ) {
		debug( L"LoadBinaryResourceAsBstr: LoadResource failed, error is %lu", GetLastError( ) );
		return false;
	}

	DWORD dwSize = SizeofResource( nullptr, hrsrc );
	if ( !dwSize ) {
		debug( L"LoadBinaryResourceAsBstr: SizeofResource failed, error is %lu", GetLastError( ) );
		return false;
	}

	pvResource = LockResource( hglob );
	cbResource = dwSize;
	return true;
}

static inline BSTR LoadBinaryResourceAsBstr( unsigned uID ) {
	void* pvResource = nullptr;
	size_t cbResource = 0;

	if ( !LoadBinaryResource( uID, pvResource, cbResource ) ) {
		debug( L"LoadBinaryResourceAsBstr: LoadBinaryResource(%u) failed: %lu\n", uID, GetLastError( ) );
	}

	BSTR bstrResource = SysAllocStringLen( nullptr, static_cast<unsigned>( cbResource ) + 1 );
	if ( !bstrResource ) {
		return nullptr;
	}

	unsigned cb = static_cast<unsigned>( cbResource );
	unsigned char* pch = static_cast<unsigned char*>( pvResource );
	wchar_t* pwz = bstrResource;

	for ( unsigned n = 0; n < cb; n++ ) {
		pwz[n] = static_cast<wchar_t>( pch[n] );
	}
	pwz[cb] = 0;

	return bstrResource;
}

//==============================================================================
// CXmlOptionsManager implementation
//==============================================================================

bool CXmlOptionsManager::_InterpretSchemaVersionNode( XNode const& node ) {
	int nSchemaVersion = Coerce<_bstr_t, unsigned>( static_cast<LPCWSTR>( node->text ) );
	debug( L"CXmlOptionsManager::_InterpretSchemaVersionNode: version is '%s' => %d\n", static_cast<LPCWSTR>( node->text ), nSchemaVersion );
	if ( CONFIGURATION_SCHEMA_VERSION != nSchemaVersion ) {
		debug( L"CXmlOptionsManager::_InterpretSchemaVersionNode: wrong schema version %d in file, vs. %d, aborting load\n", nSchemaVersion, CONFIGURATION_SCHEMA_VERSION );
		return false;
	}
	return true;
}

bool CXmlOptionsManager::_InterpretOptionsNode( XNode const& node ) {
	return _DispatchChildren( L"Options", node, OptionsElementsToMethods );
}

bool CXmlOptionsManager::_InterpretStartupNode( XNode const& node ) {
	return _DispatchChildren( L"Options\\Startup", node, StartupOptionsElementsToMethods );
}

bool CXmlOptionsManager::_InterpretStartActiveNode( XNode const& node ) {
	_pOptionsData->StartActive = BoolStringMapper[node->text];
	return true;
}

bool CXmlOptionsManager::_InterpretStartWithWindowsNode( XNode const& node ) {
	_pOptionsData->StartWithWindows = BoolStringMapper[node->text];
	return true;
}

bool CXmlOptionsManager::_InterpretKeyboardNode( XNode const& node ) {
	return _DispatchChildren( L"Options\\Keyboard", node, KeyboardOptionsElementsToMethods );
}

bool CXmlOptionsManager::_InterpretCapsLockToggleModeNode( XNode const& node ) {
	_pOptionsData->CapsLockToggleMode = CltmStringMapper[node->text];
	return true;
}

bool CXmlOptionsManager::_InterpretCapsLockSwapModeNode( XNode const& node ) {
	_pOptionsData->CapsLockSwapMode = ClsmStringMapper[node->text];
	return true;
}

bool CXmlOptionsManager::_InterpretComposeKeyNode( XNode const& node ) {
	_pOptionsData->ComposeVk = CoerceXNode<unsigned>( node );
	return true;
}

bool CXmlOptionsManager::_InterpretSwapCapsLockKeyNode( XNode const& node ) {
	_pOptionsData->SwapCapsLockVk = CoerceXNode<unsigned>( node );
	return true;
}

bool CXmlOptionsManager::_InterpretMappingsNode( XNode const& node ) {
	return _DispatchChildren( L"Mappings", node, MappingsElementsToMethods );
}

bool CXmlOptionsManager::_InterpretGroupNode( XNode const& node ) {
	CString groupName;
	if ( node && node->attributes ) {
		XNode Name = node->attributes->getNamedItem( L"Name" );
		if ( Name ) {
			groupName = static_cast<LPCWSTR>( Name->text );
		}
	}

	ComposeSequenceGroup* pCurrentGroup = _pOptionsData->FindComposeSequenceGroup( groupName );
	if ( !pCurrentGroup ) {
		_pOptionsData->ComposeSequenceGroups.Add( ComposeSequenceGroup( groupName ) );
		pCurrentGroup = _pOptionsData->FindComposeSequenceGroup( groupName );
	}

	_pCurrentComposeSequences = &pCurrentGroup->ComposeSequences;
	bool fRet = _DispatchChildren( L"Mappings\\Group", node, GroupMappingsElementsToMethods );
	_pCurrentComposeSequences = nullptr;

	return fRet;
}

bool CXmlOptionsManager::_InterpretMappingNode( XNode const& node ) {
	ComposeSequence sequence;
	if ( _ComposeSequenceFromXNode( node, sequence ) ) {
		_pCurrentComposeSequences->Add( sequence );
		return true;
	} else {
		return false;
	}
}

bool CXmlOptionsManager::_DispatchChildren( wchar_t const* label, XNode const& node, XmlMethodMap& map ) {
	XNode child = node->firstChild;
	while ( child ) {
		if ( !_DispatchNode( label, child, map ) ) {
			debug( L"CXmlOptionsManager::_DispatchChildren: %s: _DispatchNode failed on node '%s'\n", label, static_cast<LPCWSTR>( child->nodeName ) );
		}
		child = child->nextSibling;
	}
	return true;
}

bool CXmlOptionsManager::_DispatchNode( wchar_t const* label, XNode const& node, XmlMethodMap& map ) {
	MethodPtr methodPtr = map[node->nodeName];
	if ( !methodPtr ) {
		debug( L"CXmlOptionsManager::_DispatchNode: %s: No method to call for node '%s'\n", label, static_cast<LPCWSTR>( node->nodeName ) );
		return false;
	}
	return ( this->*methodPtr )( node );
}

//==============================================================================
// CXmlOptionsManager::_InterpretConfiguration
//

bool CXmlOptionsManager::_InterpretConfiguration( XDocument& doc ) {
	try {
		XElement FcConfiguration = doc->documentElement;
		if ( !CompareNodeName( FcConfiguration, L"FcConfiguration" ) ) {
			debug( L"CXmlOptionsManager::_InterpretConfiguration: document element is not <FcConfiguration>, aborting load\n" );
			return false;
		}
		_bstr_t xmlns = FcConfiguration->namespaceURI;
		LPWSTR pwzXmlns = static_cast<LPWSTR>( xmlns );
		if ( 0 != CString( pwzXmlns ).Compare( XML_NAMESPACE ) ) {
			debug( L"CXmlOptionsManager::_InterpretConfiguration: namespace contains unexpected URI\n+ Our namespace:        '%s'\n+ Document's namespace: '%s'\n", XML_NAMESPACE, pwzXmlns );
			return false;
		}

		_DispatchChildren( L"root", FcConfiguration, RootElementsToMethods );
	}
	catch ( _com_error e ) {
		debug( L"CXmlOptionsManager::_InterpretConfiguration: Caught COM error exception while parsing configuration, hr=0x%08lX '%s'\n", e.Error( ), e.ErrorMessage( ) );
		return false;
	}
	catch ( ... ) {
		debug( L"CXmlOptionsManager::_InterpretConfiguration: caught some other kind of exception??\n" );
		return false;
	}

	return true;
}

//==============================================================================
// CXmlOptionsManager::_LoadDefaultConfiguration
//

bool CXmlOptionsManager::LoadDefaultConfiguration( void ) {
	XDocument doc = CreateDOMDocument( );
	if ( !doc ) {
		debug( L"CXmlOptionsManager::_LoadDefaultConfiguration: Can't create instance of DOMDocument\n" );
		return false;
	}

	_bstr_t bstrDefaultConfiguration( LoadBinaryResourceAsBstr( IDX_DEFAULT_CONFIGURATION ), false );

	//
	// Load XML from memory
	//
	try {
		_variant_t result = doc->loadXML( bstrDefaultConfiguration );
		if ( !static_cast<VARIANT_BOOL>( result ) ) {
			debug( L"CXmlOptionsManager::_LoadDefaultConfiguration: doc->loadXML failed: line %ld column %ld: hr=0x%08lX %s", doc->parseError->line, doc->parseError->linepos, doc->parseError->errorCode, static_cast<LPCWSTR>( doc->parseError->reason ) );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"CXmlOptionsManager::_LoadDefaultConfiguration: Caught exception loading default configuration, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	return _InterpretConfiguration( doc );
}

//==============================================================================
// CXmlOptionsManager::_LoadXmlFile
//

bool CXmlOptionsManager::LoadFromFile( void ) {
	if ( !EnsureFreeComposeFolderExists( ) ) {
		debug( L"CXmlOptionsManager::_LoadXmlFile: Can't ensure app data folder exists\n" );
		return false;
	}

	CString str( GetFreeComposeFolderAsCString( ) + L"\\FcConfiguration.xml" );

	XDocument doc = CreateDOMDocument( );
	if ( !doc ) {
		debug( L"CXmlOptionsManager::_LoadXmlFile: Can't create instance of DOMDocument\n" );
		return false;
	}

	//
	// Load XML from disk
	//
	try {
		_variant_t result = doc->load( _variant_t( str ) );
		if ( !static_cast<VARIANT_BOOL>( result ) ) {
			debug( L"CXmlOptionsManager::_LoadXmlFile: doc->load failed: line %ld column %ld: hr=0x%08lX %s", doc->parseError->line, doc->parseError->linepos, doc->parseError->errorCode, static_cast<wchar_t const*>( doc->parseError->reason ) );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"CXmlOptionsManager::_LoadXmlFile: Caught exception loading configuration file, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	return _InterpretConfiguration( doc );
}

//==============================================================================
// CXmlOptionsManager::_SaveXmlFile
//

bool CXmlOptionsManager::SaveToFile( void ) {
	if ( !EnsureFreeComposeFolderExists( ) ) {
		debug( L"CXmlOptionsManager::_SaveXmlFile: Can't ensure app data folder exists\n" );
		return false;
	}

	CString str( GetFreeComposeFolderAsCString( ) + L"\\FcConfiguration.xml" );

	XDocument doc = CreateDOMDocument( );
	if ( !doc ) {
		debug( L"CXmlOptionsManager::_SaveXmlFile: Can't create instance of DOMDocument\n" );
		return false;
	}

	//
	// Create elements
	//
	try {
        doc->appendChild( doc->createProcessingInstruction( L"xml", L"version='1.0' encoding='utf-16le'" ) );
		XNode FcConfiguration = CreateAndAppendXNode( doc, L"FcConfiguration", doc );

			XNode SchemaVersion = CreateAndAppendXNode( doc, L"SchemaVersion", FcConfiguration, CONFIGURATION_SCHEMA_VERSION );

			XNode Options = CreateAndAppendXNode( doc, L"Options", FcConfiguration );

				XNode Startup = CreateAndAppendXNode( doc, L"Startup", Options );

					XNode StartActive      = CreateAndAppendXNode( doc, L"StartActive",      Startup, BoolStringMapper[ !!_pOptionsData->StartActive ] );
					XNode StartWithWindows = CreateAndAppendXNode( doc, L"StartWithWindows", Startup, BoolStringMapper[ !!_pOptionsData->StartWithWindows ] );

				XNode Keyboard = CreateAndAppendXNode( doc, L"Keyboard", Options );

					XNode CapsLockToggleMode = CreateAndAppendXNode( doc, L"CapsLockToggleMode", Keyboard, CltmStringMapper[ _pOptionsData->CapsLockToggleMode ] );
					XNode CapsLockSwapMode   = CreateAndAppendXNode( doc, L"CapsLockSwapMode",   Keyboard, ClsmStringMapper[ _pOptionsData->CapsLockSwapMode ] );
					XNode ComposeKey         = CreateAndAppendXNode( doc, L"ComposeKey",         Keyboard, _pOptionsData->ComposeVk );
					XNode SwapCapsLockKey    = CreateAndAppendXNode( doc, L"SwapCapsLockKey",    Keyboard, _pOptionsData->SwapCapsLockVk );

			XNode Mappings = CreateAndAppendXNode( doc, L"Mappings", FcConfiguration );

			int groupCount = _pOptionsData->ComposeSequenceGroups.GetCount( );
			for ( INT_PTR groupIndex = 0; groupIndex < groupCount; groupIndex++ ) {
				ComposeSequenceGroup& composeSequenceGroup = _pOptionsData->ComposeSequenceGroups[groupIndex];

				XNode Group = CreateAndAppendXNode( doc, L"Group", Mappings );
				XElement GroupElement = Group;
				if ( !composeSequenceGroup.Name.IsEmpty( ) ) {
					GroupElement->setAttribute( L"Name", static_cast<LPCWSTR>( composeSequenceGroup.Name ) );
				}

				int sequenceCount = composeSequenceGroup.ComposeSequences.GetSize( );
				for ( INT_PTR sequenceIndex = 0; sequenceIndex < sequenceCount; sequenceIndex++ ) {
					ComposeSequence& composeSequence = composeSequenceGroup.ComposeSequences[sequenceIndex];

					if ( composeSequence.Sequence.GetLength( ) == 0 || composeSequence.Result.GetLength( ) == 0 ) {
						continue;
					}

					XNode Mapping = CreateAndAppendXNode( doc, L"Mapping", Group );
					XElement MappingElement = Mapping;
					if ( composeSequence.Disabled ) {
						MappingElement->setAttribute( L"Disabled", BoolStringMapper[composeSequence.Disabled] );
					}
					if ( composeSequence.CaseInsensitive ) {
						MappingElement->setAttribute( L"CaseInsensitive", BoolStringMapper[composeSequence.CaseInsensitive] );
					}
					if ( composeSequence.Reversible ) {
						MappingElement->setAttribute( L"Reversible", BoolStringMapper[composeSequence.Reversible] );
					}

						XNode sequence = CreateAndAppendXNode( doc, L"Sequence", Mapping, static_cast<LPCWSTR>( composeSequence.Sequence ) );
						XNode result   = CreateAndAppendXNode( doc, L"Result",   Mapping );
						result->appendChild( doc->createCDATASection( static_cast<LPCWSTR>( composeSequence.Result ) ) );
				}
			}
	}
	catch ( _com_error e ) {
		debug( L"CXmlOptionsManager::_SaveXmlFile: Caught exception creating elements, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	//
	// Save XML to disk
	//
	try {
		HRESULT hr = doc->save( _variant_t( str ) );
		if ( FAILED( hr ) ) {
			debug( L"CXmlOptionsManager::_SaveXmlFile: doc->save failed, hr=0x%08lX\n", hr );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"CXmlOptionsManager::_SaveXmlFile: Caught exception saving configuration, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	return true;
}
