#include "stdafx.h"

#import <msxml6.dll>

#include "FreeCompose.h"
#include "OptionsData.h"
#include "StringMapper.h"
#include "Utils.h"

//==============================================================================
// Type aliases
//==============================================================================

using XAttribute             = MSXML2::IXMLDOMAttributePtr;
using XDocument              = MSXML2::IXMLDOMDocumentPtr;
using XElement               = MSXML2::IXMLDOMElementPtr;
using XNode                  = MSXML2::IXMLDOMNodePtr;
using XNodeList              = MSXML2::IXMLDOMNodeListPtr;
using XParseError            = MSXML2::IXMLDOMParseErrorPtr;
using XProcessingInstruction = MSXML2::IXMLDOMProcessingInstructionPtr;

//==============================================================================
// Constants
//==============================================================================

int const CONFIGURATION_SCHEMA_VERSION = 2;

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

//==============================================================================
// Local functions
//==============================================================================

// 'Coerce' functions

template<typename Tin, typename Tout>
static inline Tout Coerce( Tin const& in ) {
	return static_cast<Tout>( _variant_t( in ) );
}

template<typename Tout>
static inline Tout CoerceXElement( XElement const& value ) {
	return Coerce<_bstr_t, Tout>( value->text );
}

// 'FromXElement' functions

static inline ComposeSequence ComposeSequenceFromXElement( XElement const& value ) {
	CString Sequence( (LPCWSTR) value->selectSingleNode( L"Sequence" )->text );
	CString Result( (LPCWSTR) value->selectSingleNode( L"Result" )->text );
	return ComposeSequence( Sequence, Result );
}

// CreateXElement functions

template<typename Tparent>
static inline XElement CreateAndAppendXElement( XDocument& doc, _bstr_t const& tagName, Tparent& parent ) {
	XElement element = doc->createElement( tagName );
	parent->appendChild( element );
	return element;
}

template<typename Tparent, typename Tvalue>
static inline XElement CreateAndAppendXElement( XDocument& doc, _bstr_t const& tagName, Tparent& parent, Tvalue const& value ) {
	XElement element = doc->createElement( tagName );
	element->text = Coerce<Tvalue, _bstr_t>( value );
	parent->appendChild( element );
	return element;
}

// Other functions

static inline bool CompareNodeName( XElement elt, LPCWSTR name ) {
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

//==============================================================================
// COptionsData implementation
//==============================================================================

bool COptionsData::_InterpretConfiguration( void* pvDoc ) {
	XDocument doc = static_cast<IXMLDOMDocument*>( pvDoc );

	try {
		XElement FcConfiguration = doc->documentElement;
		if ( !CompareNodeName( FcConfiguration, L"FcConfiguration" ) ) {
			debug( L"COptionsData::_InterpretConfiguration: document element is not <FcConfiguration>, aborting load\n" );
			return false;
		}

		XElement SchemaVersion = FcConfiguration->selectSingleNode( L"SchemaVersion" );
		int nSchemaVersion = CoerceXElement<unsigned>( SchemaVersion );
		if ( CONFIGURATION_SCHEMA_VERSION != nSchemaVersion ) {
			debug( L"COptionsData::_InterpretConfiguration: wrong schema version %d in file, vs. %d, aborting load\n", nSchemaVersion, CONFIGURATION_SCHEMA_VERSION );
			return false;
		}

		XElement Options = FcConfiguration->selectSingleNode( L"Options" );

		XElement Startup = Options->selectSingleNode( L"Startup" );
		StartActive = BoolStringMapper[ Startup->selectSingleNode( L"StartActive" )->text ];
		StartWithWindows = BoolStringMapper[ Startup->selectSingleNode( L"StartWithWindows" )->text ];

		XElement Keyboard = Options->selectSingleNode( L"Keyboard" );
		CapsLockToggleMode = CltmStringMapper[ Keyboard->selectSingleNode( L"CapsLockToggleMode" )->text ];
		CapsLockSwapMode = ClsmStringMapper[ Keyboard->selectSingleNode( L"CapsLockSwapMode" )->text ];
		ComposeVk = CoerceXElement<unsigned>( Keyboard->selectSingleNode( L"ComposeKey" ) );
		SwapCapsLockVk = CoerceXElement<unsigned>( Keyboard->selectSingleNode( L"SwapCapsLockKey" ) );

		XElement Mappings = FcConfiguration->selectSingleNode( L"Mappings" );
		XNodeList groupNodes = Mappings->selectNodes( L"Group" );
		XElement group;
		while ( group = groupNodes->nextNode( ) ) {
			XNodeList mappingNodes = group->selectNodes( L"Mapping" );
			XElement mapping;
			while ( mapping = mappingNodes->nextNode( ) ) {
				ComposeSequences.Add( ComposeSequenceFromXElement( mapping ) );
			}
		}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::_InterpretConfiguration: Caught exception parsing configuration, hr=0x%08lX\n", e.Error( ) );
		return false;
	}
	catch ( ... ) {
		debug( L"COptionsData::_InterpretConfiguration: caught some other kind of exception??\n" );
		return false;
	}

	return true;
}

bool COptionsData::_LoadDefaultConfiguration( void ) {
	CString strConfiguration( LoadFromStringTable( IDX_DEFAULT_CONFIGURATION ) );

	XDocument doc = CreateDOMDocument( );
	if ( !doc ) {
		debug( L"COptionsData::_LoadDefaultConfiguration: Can't create instance of DOMDocument\n" );
		return false;
	}

	//
	// Load XML from memory
	//
	try {
		_variant_t result = doc->loadXML( static_cast<LPCWSTR>( strConfiguration ) );
		if ( !static_cast<VARIANT_BOOL>( result ) ) {
			debug( L"COptionsData::_LoadDefaultConfiguration: doc->loadXML failed: line %ld column %ld: hr=0x%08lX %s", doc->parseError->line, doc->parseError->linepos, doc->parseError->errorCode, static_cast<wchar_t const*>( doc->parseError->reason ) );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::_LoadDefaultConfiguration: Caught exception loading default configuration, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	return _InterpretConfiguration( doc.GetInterfacePtr( ) );
}

bool COptionsData::_LoadXmlFile( void ) {
	if ( !EnsureFreeComposeFolderExists( ) ) {
		debug( L"COptionsData::_LoadXmlFile: Can't ensure app data folder exists\n" );
		return false;
	}

	CString str( GetFreeComposeFolderAsCString( ) + L"\\FcConfiguration.xml" );

	XDocument doc = CreateDOMDocument( );
	if ( !doc ) {
		debug( L"COptionsData::_LoadXmlFile: Can't create instance of DOMDocument\n" );
		return false;
	}

	//
	// Load XML from disk
	//
	try {
		_variant_t result = doc->load( _variant_t( str ) );
		if ( !static_cast<VARIANT_BOOL>( result ) ) {
			debug( L"COptionsData::_LoadXmlFile: doc->load failed: line %ld column %ld: hr=0x%08lX %s", doc->parseError->line, doc->parseError->linepos, doc->parseError->errorCode, static_cast<wchar_t const*>( doc->parseError->reason ) );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::_LoadXmlFile: Caught exception loading configuration file, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	return _InterpretConfiguration( doc.GetInterfacePtr( ) );
}

bool COptionsData::_SaveXmlFile( void ) {
	if ( !EnsureFreeComposeFolderExists( ) ) {
		debug( L"COptionsData::_SaveXmlFile: Can't ensure app data folder exists\n" );
		return false;
	}

	CString str( GetFreeComposeFolderAsCString( ) + L"\\FcConfiguration.xml" );

	XDocument doc = CreateDOMDocument( );
	if ( !doc ) {
		debug( L"COptionsData::_SaveXmlFile: Can't create instance of DOMDocument\n" );
		return false;
	}

	//
	// Create elements
	//
	try {
        doc->appendChild( doc->createProcessingInstruction( L"xml", L"version='1.0' encoding='utf-8'" ) );
		XElement FcConfiguration = CreateAndAppendXElement( doc, L"FcConfiguration", doc );
		FcConfiguration->setAttribute( L"xmlns", "http://www.zive.ca/xmlns/FreeCompose/configuration/2" );

			XElement SchemaVersion = CreateAndAppendXElement( doc, L"SchemaVersion", FcConfiguration, CONFIGURATION_SCHEMA_VERSION );

			XElement Options = CreateAndAppendXElement( doc, L"Options", FcConfiguration );

				XElement Startup = CreateAndAppendXElement( doc, L"Startup", Options );

					XElement StartActive      = CreateAndAppendXElement( doc, L"StartActive",      Startup, BoolStringMapper[ !!this->StartActive ] );
					XElement StartWithWindows = CreateAndAppendXElement( doc, L"StartWithWindows", Startup, BoolStringMapper[ !!this->StartWithWindows ] );

				XElement Keyboard = CreateAndAppendXElement( doc, L"Keyboard", Options );

					XElement CapsLockToggleMode = CreateAndAppendXElement( doc, L"CapsLockToggleMode", Keyboard, CltmStringMapper[ this->CapsLockToggleMode ] );
					XElement CapsLockSwapMode   = CreateAndAppendXElement( doc, L"CapsLockSwapMode",   Keyboard, ClsmStringMapper[ this->CapsLockSwapMode ] );
					XElement ComposeKey         = CreateAndAppendXElement( doc, L"ComposeKey",         Keyboard, this->ComposeVk );
					XElement SwapCapsLockKey    = CreateAndAppendXElement( doc, L"SwapCapsLockKey",    Keyboard, this->SwapCapsLockVk );

			XElement Mappings = CreateAndAppendXElement( doc, L"Mappings", FcConfiguration );

				XElement Group = CreateAndAppendXElement( doc, L"Group", Mappings );
				Group->setAttribute( L"Name", L"default" );

				for ( INT_PTR n = 0; n < ComposeSequences.GetSize( ); n++ ) {
					if ( ComposeSequences[n].Sequence.GetLength( ) == 0 || ComposeSequences[n].Result.GetLength( ) == 0 ) {
						continue;
					}

					XElement mapping = CreateAndAppendXElement( doc, L"Mapping", Group );
						XElement sequence = CreateAndAppendXElement( doc, L"Sequence", mapping, static_cast<LPCWSTR>( ComposeSequences[n].Sequence ) );
						XElement result   = CreateAndAppendXElement( doc, L"Result",   mapping, static_cast<LPCWSTR>( ComposeSequences[n].Result ) );
				}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::_SaveXmlFile: Caught exception creating elements, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	//
	// Save XML to disk
	//
	try {
		HRESULT hr = doc->save( _variant_t( str ) );
		if ( FAILED( hr ) ) {
			debug( L"COptionsData::_SaveXmlFile: doc->save failed, hr=0x%08lX\n", hr );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::_SaveXmlFile: Caught exception saving configuration, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	return true;
}
