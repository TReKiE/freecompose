#include "stdafx.h"

#include "FreeCompose.h"
#include "OptionsData.h"
#include "StringMapper.h"
#include "Utils.h"

#include <Unicode.h>

//==============================================================================
// Constants
//==============================================================================

int const CONFIGURATION_SCHEMA_VERSION = 2;
wchar_t const XML_NAMESPACE[] = L"http://www.zive.ca/xmlns/FreeCompose/configuration/2";

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
	static XmlMethodMap OptionsElementsToMethods;
		static XmlMethodMap StartupOptionsElementsToMethods;
		static XmlMethodMap KeyboardOptionsElementsToMethods;
	static XmlMethodMap MappingsElementsToMethods;
		static XmlMethodMap GroupMappingsElementsToMethods;

//==============================================================================
// Static initialization object for this translation unit
//==============================================================================

class Initializer_ {
public:
	inline Initializer_( ) {
		RootElementsToMethods.insert( XmlMethodMapPair( L"SchemaVersion", &CXmlOptionsManager::_InterpretSchemaVersionNode ) );
		RootElementsToMethods.insert( XmlMethodMapPair( L"Options",       &CXmlOptionsManager::_InterpretOptionsNode       ) );
		RootElementsToMethods.insert( XmlMethodMapPair( L"Mappings",      &CXmlOptionsManager::_InterpretMappingsNode      ) );

		OptionsElementsToMethods.insert( XmlMethodMapPair( L"Startup",  &CXmlOptionsManager::_InterpretStartupNode  ) );
		OptionsElementsToMethods.insert( XmlMethodMapPair( L"Keyboard", &CXmlOptionsManager::_InterpretKeyboardNode ) );

		StartupOptionsElementsToMethods.insert( XmlMethodMapPair( L"StartActive",      &CXmlOptionsManager::_InterpretStartActiveNode      ) );
		StartupOptionsElementsToMethods.insert( XmlMethodMapPair( L"StartWithWindows", &CXmlOptionsManager::_InterpretStartWithWindowsNode ) );

		KeyboardOptionsElementsToMethods.insert( XmlMethodMapPair( L"CapsLockToggleMode", &CXmlOptionsManager::_InterpretCapsLockToggleModeNode ) );
		KeyboardOptionsElementsToMethods.insert( XmlMethodMapPair( L"CapsLockSwapMode",   &CXmlOptionsManager::_InterpretCapsLockSwapModeNode   ) );
		KeyboardOptionsElementsToMethods.insert( XmlMethodMapPair( L"ComposeKey",         &CXmlOptionsManager::_InterpretComposeKeyNode         ) );
		KeyboardOptionsElementsToMethods.insert( XmlMethodMapPair( L"SwapCapsLockKey",    &CXmlOptionsManager::_InterpretSwapCapsLockKeyNode    ) );

		MappingsElementsToMethods.insert( XmlMethodMapPair( L"Group", &CXmlOptionsManager::_InterpretGroupNode ) );

		GroupMappingsElementsToMethods.insert( XmlMethodMapPair( L"Mapping", &CXmlOptionsManager::_InterpretMappingNode ) );
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

// 'FromXNode' functions

static inline ComposeSequence ComposeSequenceFromXNode( XNode const& value ) {
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

	if ( nodeFirst && nodeSecond && nodeComposed ) {
		CString First, Second, Composed;
		First    =   VkToString( static_cast<unsigned>( static_cast<_variant_t>( nodeFirst->text ) ) );
		Second   =   VkToString( static_cast<unsigned>( static_cast<_variant_t>( nodeSecond->text ) ) );
		Composed = Utf32ToUtf16( static_cast<unsigned>( static_cast<_variant_t>( nodeComposed->text ) ) );

		Sequence = First + Second;
		Result   = Composed;
	} else if ( nodeSequence && nodeResult ) {
		debug( L"ComposeSequenceFromNode: new format\n" );

		Sequence = static_cast<LPCWSTR>( nodeSequence->text );
		Result   = static_cast<LPCWSTR>( nodeResult->text );
	} else {
		debug( L"ComposeSequenceFromNode: unknown format???\n" );
	}

	return ComposeSequence( Sequence, Result );
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

static inline BSTR LoadBinaryResourceAsBstr( unsigned uID ) {
	HRSRC hrsrc = FindResource( nullptr, MAKEINTRESOURCE( uID ), L"XMLFILE" );
	if ( !hrsrc ) {
		debug( L"LoadBinaryResourceAsBstr: FindResource failed, error is %lu", GetLastError( ) );
		return _bstr_t( );
	}

	HGLOBAL hglob = LoadResource( nullptr, hrsrc );
	if ( !hglob ) {
		debug( L"LoadBinaryResourceAsBstr: LoadResource failed, error is %lu", GetLastError( ) );
		return _bstr_t( );
	}

	DWORD dwSize = SizeofResource( nullptr, hrsrc );
	if ( !dwSize ) {
		debug( L"LoadBinaryResourceAsBstr: SizeofResource failed, error is %lu", GetLastError( ) );
		return _bstr_t( );
	}

	LPCSTR pszConf = static_cast<LPSTR>( LockResource( hglob ) );
	if ( !pszConf ) {
		debug( L"LoadBinaryResourceAsBstr: LockResource failed, error is %lu", GetLastError( ) );
		return _bstr_t( );
	}

	// hglob is not a real handle to a global object! Do not call GlobalFree() on it! This is not a leak, honest!

	return SysAllocStringByteLen( pszConf, dwSize );
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
	return _DispatchChildren( L"Mappings\\Group", node, GroupMappingsElementsToMethods );
}

bool CXmlOptionsManager::_InterpretMappingNode( XNode const& node ) {
	_pOptionsData->ComposeSequences.Add( ComposeSequenceFromXNode( node ) );
	return true;
}

bool CXmlOptionsManager::_DispatchChildren( wchar_t const* label, XNode const& node, XmlMethodMap& map ) {
	XNode child = node->firstChild;
	while ( child ) {
		debug( L"CXmlOptionsManager::_DispatchChildren: %s: child element: '%s'\n", label, static_cast<LPCWSTR>( child->nodeName ) );
		if ( !_DispatchNode( child, map ) ) {
			debug( L"CXmlOptionsManager::_DispatchChildren: %s: _DispatchNode failed\n", label );
			// TODO return false
		}
		child = child->nextSibling;
	}
	return true;
}

bool CXmlOptionsManager::_DispatchNode( XNode const& node, XmlMethodMap& map ) {
	MethodPtr methodPtr = map[node->nodeName];
	if ( !methodPtr ) {
		debug( L"CXmlOptionsManager::_DispatchNode: No method to call for node '%s'\n", static_cast<LPCWSTR>( node->nodeName ) );
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
		if ( 0 != CString( static_cast<LPCWSTR>( xmlns ) ).Compare( XML_NAMESPACE ) ) {
			debug( L"CXmlOptionsManager::_InterpretConfiguration: namespace contains unexpected URI\n+ Our namespace:        %s\n+ Document's namespace: %s\n", XML_NAMESPACE, xmlns );
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
	_bstr_t bstrDefaultConfiguration( LoadBinaryResourceAsBstr( IDX_DEFAULT_CONFIGURATION ), false );

	XDocument doc = CreateDOMDocument( );
	if ( !doc ) {
		debug( L"CXmlOptionsManager::_LoadDefaultConfiguration: Can't create instance of DOMDocument\n" );
		return false;
	}

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

bool CXmlOptionsManager::LoadXmlFile( void ) {
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

bool CXmlOptionsManager::SaveXmlFile( void ) {
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

				XNode Group = CreateAndAppendXNode( doc, L"Group", Mappings );
				XElement( Group )->setAttribute( L"Name", L"default" );

				auto& composeSequences = _pOptionsData->ComposeSequences;
				for ( INT_PTR n = 0; n < composeSequences.GetSize( ); n++ ) {
					CString& strSequence = composeSequences[n].Sequence;
					CString& strResult   = composeSequences[n].Result;

					if ( strSequence.GetLength( ) == 0 || strResult.GetLength( ) == 0 ) {
						continue;
					}

					XNode mapping = CreateAndAppendXNode( doc, L"Mapping", Group );
						XNode sequence = CreateAndAppendXNode( doc, L"Sequence", mapping, static_cast<LPCWSTR>( strSequence ) );
						XNode result   = CreateAndAppendXNode( doc, L"Result",   mapping, static_cast<LPCWSTR>( strResult ) );
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
