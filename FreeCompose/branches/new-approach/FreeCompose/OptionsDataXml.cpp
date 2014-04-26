#include "stdafx.h"

#import <msxml6.dll>

#include "FreeCompose.h"
#include "OptionsData.h"
#include "StringMapper.h"
#include "Utils.h"

//==============================================================================
// Types and type aliases
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

static inline COMPOSE_SEQUENCE ComposeSequenceFromXElement( XElement const& value ) {
	return COMPOSE_SEQUENCE(
		CoerceXElement<unsigned>( value->selectSingleNode( L"First" ) ),
		CoerceXElement<unsigned>( value->selectSingleNode( L"Second" ) ),
		CoerceXElement<unsigned>( value->selectSingleNode( L"Composed" ) )
	);
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
	return 0 == CString( (LPCWSTR) elt->nodeName ).Compare( name );
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

bool COptionsData::_LoadFromXml( void ) {
	if ( !EnsureFreeComposeFolderExists( ) ) {
		debug( L"COptionsData::_LoadFromXml: Can't ensure app data folder exists\n" );
		return false;
	}

	CString str( GetFreeComposeFolderAsCString( ) + L"\\FreeCompose.xml" );

	XDocument doc = CreateDOMDocument( );
	if ( !doc ) {
		debug( L"COptionsData::_LoadFromXml: Can't create instance of DOMDocument\n" );
		return false;
	}

	//
	// Load XML from disk
	//
	try {
		_variant_t result = doc->load( _variant_t( str ) );
		if ( !static_cast<VARIANT_BOOL>( result ) ) {
			debug( L"COptionsData::_LoadFromXml: doc->load failed: line %ld column %ld: hr=0x%08lX %s", doc->parseError->line, doc->parseError->linepos, doc->parseError->errorCode, static_cast<wchar_t const*>( doc->parseError->reason ) );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::_LoadFromXml: Caught exception loading configuration file, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	try {
		XElement FreeCompose = doc->documentElement;
		if ( !CompareNodeName( FreeCompose, L"FreeCompose" ) ) {
			debug( L"COptionsData::_LoadFromXml: document element is not <FreeCompose>, aborting load\n" );
			return false;
		}

		XElement SchemaVersion = FreeCompose->selectSingleNode( L"SchemaVersion" );
		int nSchemaVersion = CoerceXElement<unsigned>( SchemaVersion );
		if ( CONFIGURATION_SCHEMA_VERSION != nSchemaVersion ) {
			debug( L"COptionsData::_LoadFromXml: wrong schema version %d in file, vs. %d, aborting load\n", nSchemaVersion, CONFIGURATION_SCHEMA_VERSION );
			return false;
		}

		XElement Options = FreeCompose->selectSingleNode( L"Options" );

		XElement Startup = Options->selectSingleNode( L"Startup" );
		m_fStartActive = BoolStringMapper[ Startup->selectSingleNode( L"StartActive" )->text ];
		m_fStartWithWindows = BoolStringMapper[ Startup->selectSingleNode( L"StartWithWindows" )->text ];

		XElement Keyboard = Options->selectSingleNode( L"Keyboard" );
		m_CapsLockToggleMode = CltmStringMapper[ Keyboard->selectSingleNode( L"CapsLockToggleMode" )->text ];
		m_CapsLockSwapMode = ClsmStringMapper[ Keyboard->selectSingleNode( L"CapsLockSwapMode" )->text ];
		m_vkCompose = CoerceXElement<unsigned>( Keyboard->selectSingleNode( L"ComposeKey" ) );
		m_vkSwapCapsLock = CoerceXElement<unsigned>( Keyboard->selectSingleNode( L"SwapCapsLockKey" ) );

		XElement Mappings = FreeCompose->selectSingleNode( L"Mappings" );
		XNodeList groupNodes = Mappings->selectNodes( L"Group" );
		XElement group;
		while ( group = groupNodes->nextNode( ) ) {
			XNodeList mappingNodes = group->selectNodes( L"Mapping" );
			XElement mapping;
			while ( mapping = mappingNodes->nextNode( ) ) {
				m_ComposeSequences.Add( ComposeSequenceFromXElement( mapping ) );
			}
		}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::_LoadFromXml: Caught exception parsing configuration, hr=0x%08lX\n", e.Error( ) );
		return false;
	}
	catch ( ... ) {
		debug( L"COptionsData::_LoadFromXml: something bad happened??\n" );
		return false;
	}

	return true;
}

bool COptionsData::_SaveToXml( void ) {
	if ( !EnsureFreeComposeFolderExists( ) ) {
		debug( L"COptionsData::_SaveToXml: Can't ensure app data folder exists\n" );
		return false;
	}

	CString str( GetFreeComposeFolderAsCString( ) + L"\\FreeCompose.xml" );

	XDocument doc = CreateDOMDocument( );
	if ( !doc ) {
		debug( L"COptionsData::_SaveToXml: Can't create instance of DOMDocument\n" );
		return false;
	}

	//
	// Create elements
	//
	try {
        doc->appendChild( doc->createProcessingInstruction( L"xml", L"version='1.0' encoding='utf-8'" ) );
		XElement FreeCompose = CreateAndAppendXElement( doc, L"FreeCompose", doc );

			XElement SchemaVersion = CreateAndAppendXElement( doc, L"SchemaVersion", FreeCompose, CONFIGURATION_SCHEMA_VERSION );

			XElement Options = CreateAndAppendXElement( doc, L"Options", FreeCompose );

				XElement Startup = CreateAndAppendXElement( doc, L"Startup", Options );

					XElement StartActive      = CreateAndAppendXElement( doc, L"StartActive",      Startup, BoolStringMapper[ !!m_fStartActive ] );
					XElement StartWithWindows = CreateAndAppendXElement( doc, L"StartWithWindows", Startup, BoolStringMapper[ !!m_fStartWithWindows ] );

				XElement Keyboard = CreateAndAppendXElement( doc, L"Keyboard", Options );

					XElement CapsLockToggleMode = CreateAndAppendXElement( doc, L"CapsLockToggleMode", Keyboard, CltmStringMapper[ m_CapsLockToggleMode ] );
					XElement CapsLockSwapMode   = CreateAndAppendXElement( doc, L"CapsLockSwapMode",   Keyboard, ClsmStringMapper[ m_CapsLockSwapMode ] );
					XElement ComposeKey         = CreateAndAppendXElement( doc, L"ComposeKey",         Keyboard, m_vkCompose );
					XElement SwapCapsLockKey    = CreateAndAppendXElement( doc, L"SwapCapsLockKey",    Keyboard, m_vkSwapCapsLock );

			XElement Mappings = CreateAndAppendXElement( doc, L"Mappings", FreeCompose );

				XElement Group = CreateAndAppendXElement( doc, L"Group", Mappings );
				Group->setAttribute( L"Name", L"default" );

				for ( INT_PTR n = 0; n < m_ComposeSequences.GetSize( ); n++ ) {
					if ( !m_ComposeSequences[n].chComposed ) {
						continue;
					}

					XElement mapping = CreateAndAppendXElement( doc, L"Mapping", Group );
						XElement first    = CreateAndAppendXElement( doc, L"First",    mapping, m_ComposeSequences[n].chFirst );
						XElement second   = CreateAndAppendXElement( doc, L"Second",   mapping, m_ComposeSequences[n].chSecond );
						XElement composed = CreateAndAppendXElement( doc, L"Composed", mapping, m_ComposeSequences[n].chComposed );
				}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::_SaveToXml: Caught exception creating elements, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	//
	// Save XML to disk
	//
	try {
		HRESULT hr = doc->save( _variant_t( str ) );
		if ( FAILED( hr ) ) {
			debug( L"COptionsData::_SaveToXml: doc->save failed, hr=0x%08lX\n", hr );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::_SaveToXml: Caught exception saving configuration, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	return true;
}
