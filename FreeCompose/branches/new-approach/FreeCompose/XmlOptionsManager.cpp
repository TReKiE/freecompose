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

using MethodPtr = bool (CXmlOptionsManager::*)( XNode const& );

static std::map<wchar_t const*, MethodPtr> XmlElementNamesToMethods;
class Initializer_ {
	using pairtype = std::pair<wchar_t const*, MethodPtr>;

public:
	inline Initializer_( ) {
		XmlElementNamesToMethods.insert( pairtype( L"aaa", &CXmlOptionsManager::_InterpretSchemaVersionNode ) );
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
	CString Sequence, Result;

	XNode nodeFirst = value->selectSingleNode( L"First" );
	XNode nodeSecond = value->selectSingleNode( L"Second" );
	XNode nodeComposed = value->selectSingleNode( L"Composed" );

	XNode nodeSequence = value->selectSingleNode( L"Sequence" );
	XNode nodeResult = value->selectSingleNode( L"Result" );

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
	CString text( static_cast<LPCWSTR>( node->text ) );
	debug( L"InterpretSchemaVersionNode: text is %s\n", static_cast<LPCWSTR>( node->text ) );
	return false;
}

bool CXmlOptionsManager::_DispatchNode( XNode const& node ) {
	MethodPtr methodPtr = XmlElementNamesToMethods[node->nodeName];
	( this->*methodPtr )( node );
	return false;
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
			debug( L"CXmlOptionsManager::_InterpretConfiguration: namespace doesn't match\n+ Our namespace:        %s\n+ Document's namespace: %s\n", XML_NAMESPACE, xmlns );
			return false;
		}

		{
			XNode child = FcConfiguration->firstChild;
			while ( child ) {
				debug( L"CXmlOptionsManager::_InterpretConfiguration: next child element:\nBase name: %s\nNode name: %s\n", static_cast<LPCWSTR>( child->baseName ), static_cast<LPCWSTR>( child->nodeName ) );

				
				child = child->nextSibling;
			}
		}

		XNode SchemaVersion = FcConfiguration->selectSingleNode( L"SchemaVersion" );
		int nSchemaVersion = CoerceXNode<unsigned>( SchemaVersion );
		if ( CONFIGURATION_SCHEMA_VERSION != nSchemaVersion ) {
			debug( L"CXmlOptionsManager::_InterpretConfiguration: wrong schema version %d in file, vs. %d, aborting load\n", nSchemaVersion, CONFIGURATION_SCHEMA_VERSION );
			return false;
		}

		XNode Options = FcConfiguration->selectSingleNode( L"Options" );

		XNode Startup = Options->selectSingleNode( L"Startup" );
		_pOptionsData->StartActive = BoolStringMapper[Startup->selectSingleNode( L"StartActive" )->text];
		_pOptionsData->StartWithWindows = BoolStringMapper[ Startup->selectSingleNode( L"StartWithWindows" )->text ];

		XNode Keyboard = Options->selectSingleNode( L"Keyboard" );
		_pOptionsData->CapsLockToggleMode = CltmStringMapper[ Keyboard->selectSingleNode( L"CapsLockToggleMode" )->text ];
		_pOptionsData->CapsLockSwapMode = ClsmStringMapper[ Keyboard->selectSingleNode( L"CapsLockSwapMode" )->text ];
		_pOptionsData->ComposeVk = CoerceXNode<unsigned>( Keyboard->selectSingleNode( L"ComposeKey" ) );
		_pOptionsData->SwapCapsLockVk = CoerceXNode<unsigned>( Keyboard->selectSingleNode( L"SwapCapsLockKey" ) );

		XNode Mappings = FcConfiguration->selectSingleNode( L"Mappings" );
		XNodeList groupNodes = Mappings->selectNodes( L"Group" );
		XNode group;
		while ( group = groupNodes->nextNode( ) ) {
			XNodeList mappingNodes = group->selectNodes( L"Mapping" );
			XNode mapping;
			while ( mapping = mappingNodes->nextNode( ) ) {
				_pOptionsData->ComposeSequences.Add( ComposeSequenceFromXNode( mapping ) );
			}
		}
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
