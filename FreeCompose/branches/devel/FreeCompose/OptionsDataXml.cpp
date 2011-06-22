#include "stdafx.h"

#import <msxml6.dll>

#include "FreeCompose.h"
#include "OptionsData.h"
#include "Utils.h"

//==============================================================================
// Types and type aliases
//==============================================================================

typedef MSXML2::IXMLDOMAttributePtr XAttribute;
typedef MSXML2::IXMLDOMDocumentPtr XDocument;
typedef MSXML2::IXMLDOMElementPtr XElement;
typedef MSXML2::IXMLDOMNodePtr XNode;
typedef MSXML2::IXMLDOMNodeListPtr XNodeList;
typedef MSXML2::IXMLDOMProcessingInstructionPtr XProcessingInstruction;

//==============================================================================
// Constants for XML type mapping
//==============================================================================

static const _bstr_t mapBoolToString[ ] = {
	L"false",
	L"true",
};

static const _bstr_t mapCapsLockToggleModeToString[ ] = {
	(LPCWSTR) NULL,
	L"normal",
	L"pressTwice",
	L"disabled",
};

static const _bstr_t mapCapsLockSwapModeToString[ ] = {
	(LPCWSTR) NULL,
	L"swap",
	L"replace",
};

//==============================================================================
// Local functions
//==============================================================================

static inline _bstr_t StrFromBool( const bool _ ) {
	return mapBoolToString[_];
}

static inline _bstr_t StrFromCapsLockToggleMode( const CAPS_LOCK_TOGGLE_MODE _ ) {
	return mapCapsLockToggleModeToString[_];
}

static inline _bstr_t StrFromCapsLockSwapMode( const CAPS_LOCK_SWAP_MODE _ ) {
	return _bstr_t( mapCapsLockSwapModeToString[_] );
}

static inline _bstr_t StrFromInt( const int _ ) {
	return _bstr_t( _variant_t( _ ) );
}

static XElement ElementFromCke( XDocument doc, COMPOSE_KEY_ENTRY& cke ) {
	XElement mapping = doc->createElement( L"Mapping" );

		XElement first = doc->createElement( L"First" );
		mapping->appendChild( first );
		if ( ( cke.vkFirst & 0x80000000 ) != 0 ) {
			first->setAttribute( L"Shifted", L"true" );
		}
		first->appendChild( doc->createTextNode( _bstr_t( _variant_t( (unsigned char) ( cke.vkFirst & 0xFF ) ) ) ) );

		XElement second = doc->createElement( L"Second" );
		mapping->appendChild( second );
		if ( ( cke.vkSecond & 0x80000000 ) != 0 ) {
			second->setAttribute( L"Shifted", L"true" );
		}
		second->appendChild( doc->createTextNode( _bstr_t( _variant_t( (unsigned char) ( cke.vkSecond & 0xFF ) ) ) ) );

		XElement composed = doc->createElement( L"Composed" );
		mapping->appendChild( composed );
		composed->text = _bstr_t( _variant_t( (unsigned) cke.u32Composed ) );

	return mapping;
}

static inline bool BoolFromXNode( const XNode& _ ) {
	for ( int n = 0; n < _countof( mapBoolToString ); n++ ) {
		if ( 0 == wcscmp( _->text, mapBoolToString[n] ) ) {
			return !!n;
		}
	}
	return false;
}

static inline CAPS_LOCK_TOGGLE_MODE CapsLockToggleModeFromXElement( const XElement& _ ) {
	for ( int n = 1; n < _countof( mapCapsLockToggleModeToString ); n++ ) {
		if ( 0 == wcscmp( _->text, mapCapsLockToggleModeToString[n] ) ) {
			return (CAPS_LOCK_TOGGLE_MODE) n;
		}
	}
	return (CAPS_LOCK_TOGGLE_MODE) 0;
}

static inline CAPS_LOCK_SWAP_MODE CapsLockSwapModeFromXElement( const XElement& _ ) {
	for ( int n = 1; n < _countof( mapCapsLockSwapModeToString ); n++ ) {
		if ( 0 == wcscmp( _->text, mapCapsLockSwapModeToString[n] ) ) {
			return (CAPS_LOCK_SWAP_MODE) n;
		}
	}
	return (CAPS_LOCK_SWAP_MODE) 0;
}

static inline int IntFromXElement( const XElement& _ ) {
	return (int) _variant_t( _->text );
}

static COMPOSE_KEY_ENTRY CkeFromElement( XElement Mapping ) {
	COMPOSE_KEY_ENTRY cke;
	memset( &cke, 0, sizeof( cke ) );
	cke.vkFirst = IntFromXElement( Mapping->selectSingleNode( L"First" ) );
	cke.vkSecond = IntFromXElement( Mapping->selectSingleNode( L"Second" ) );
	cke.u32Composed = IntFromXElement( Mapping->selectSingleNode( L"Composed" ) );

	XAttribute shifted;
	shifted = Mapping->selectSingleNode( L"First/@Shifted" );
	if ( shifted != NULL && BoolFromXNode( shifted ) ) {
		cke.vkFirst |= 0x80000000;
	}
	shifted = Mapping->selectSingleNode( L"Second/@Shifted" );
	if ( shifted != NULL && BoolFromXNode( shifted ) ) {
		cke.vkSecond |= 0x80000000;
	}

	return cke;
}

static inline bool CompareNodeName( XElement elt, LPCWSTR name ) {
	return 0 == ( (CString) (LPCWSTR) elt->nodeName ).Compare( name );
}

static inline XDocument CreateDOMDocument( void ) {
	XDocument doc;
	HRESULT hr = doc.CreateInstance( __uuidof( MSXML2::DOMDocument60 ), NULL, CLSCTX_INPROC_SERVER );
	if ( FAILED( hr ) ) {
		debug( L"CreateDOMDocument: failed, hr 0x%08x\n", hr );
		return NULL;
	}
	return doc;
}

//==============================================================================
// COptionsData implementation
//==============================================================================

bool COptionsData::LoadFromXml( void ) {
	if ( ! EnsureFreeComposeFolderExists( ) ) {
		debug( L"COptionsData::LoadFromXml: Can't ensure app data folder exists\n" );
		return false;
	}

	CString str( GetFreeComposeFolder( ) );
	str.Append( L"\\FreeCompose.xml" );

	XDocument doc = CreateDOMDocument( );
	if ( ! doc ) {
		debug( L"COptionsData::LoadFromXml: Can't create instance of DOMDocument\n" );
		return false;
	}

	//
	// Configure DOMDocument object
	//
	try {
		doc->async = VARIANT_FALSE;
		doc->validateOnParse = VARIANT_FALSE;
		doc->resolveExternals = VARIANT_FALSE;
		doc->preserveWhiteSpace = VARIANT_FALSE;
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::LoadFromXml: Caught exception setting up DOMDocument, hr=0x%08x\n", e.Error( ) );
		return false;
	}

	//
	// Load XML from disk
	//
	try {
		_variant_t result = doc->load( (LPCWSTR) str );
		if ( ! (bool) result ) {
			debug( L"COptionsData::LoadFromXML: doc->load failed: line %d column %d: %s\n", doc->parseError->line, doc->parseError->linepos, doc->parseError->reason );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::LoadFromXml: Caught exception setting up DOMDocument, hr=0x%08x\n", e.Error( ) );
		return false;
	}

	try {
		XElement FreeCompose = doc->documentElement;
		if ( ! CompareNodeName( FreeCompose, L"FreeCompose" ) ) {
			debug( L"COptionsData::LoadFromXml: document element is not <FreeCompose>, aborting load\n" );
			return false;
		}

		XElement Options = FreeCompose->selectSingleNode( L"Options" );

		XElement Startup = Options->selectSingleNode( L"Startup" );
		m_fStartActive = BoolFromXNode( Startup->selectSingleNode( L"StartActive" ) );
		m_fStartWithWindows = BoolFromXNode( Startup->selectSingleNode( L"StartWithWindows" ) );

		XElement Keyboard = Options->selectSingleNode( L"Keyboard" );
		m_fSwapCapsLock = BoolFromXNode( Keyboard->selectSingleNode( L"SwapCapsLock" ) );
		m_CapsLockToggleMode = CapsLockToggleModeFromXElement( Keyboard->selectSingleNode( L"CapsLockToggleMode" ) );
		m_CapsLockSwapMode = CapsLockSwapModeFromXElement( Keyboard->selectSingleNode( L"CapsLockSwapMode" ) );
		m_vkCompose = IntFromXElement( Keyboard->selectSingleNode( L"ComposeKey" ) );
		m_vkSwapCapsLock = IntFromXElement( Keyboard->selectSingleNode( L"SwapCapsLockKey" ) );

		XElement Mappings = FreeCompose->selectSingleNode( L"Mappings" );
		XNodeList groupNodes = Mappings->selectNodes( L"Group" );
		for ( XElement group = groupNodes->nextNode( ); NULL != group; group = groupNodes->nextNode( ) ) {
			XNodeList mappingNodes = group->selectNodes( L"Mapping" );
			for ( XElement mapping = mappingNodes->nextNode( ); NULL != mapping; mapping = mappingNodes->nextNode( ) ) {
				m_ComposeKeyEntries.Add( CkeFromElement( mapping ) );
			}
		}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::LoadFromXml: Caught exception parsing configuration, hr=0x%08x\n", e.Error( ) );
		return false;
	}
	catch ( ... ) {
		debug( L"COptionsData::LoadFromXml: something bad happened??\n" );
		return false;
	}

	return true;
}

bool COptionsData::SaveToXml( void ) {
	if ( ! EnsureFreeComposeFolderExists( ) ) {
		debug( L"COptionsData::SaveToXml: Can't ensure app data folder exists\n" );
		return false;
	}

	CString str( GetFreeComposeFolder( ) );
	str.Append( L"\\FreeCompose.xml" );

	XDocument doc = CreateDOMDocument( );
	if ( ! doc ) {
		debug( L"COptionsData::SaveToXml: Can't create instance of DOMDocument\n" );
		return false;
	}

	//
	// Configure DOMDocument object
	//
	try {
		doc->async = VARIANT_FALSE;
		doc->validateOnParse = VARIANT_FALSE;
		doc->resolveExternals = VARIANT_FALSE;
		doc->preserveWhiteSpace = VARIANT_FALSE;
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::SaveToXml: Caught exception setting up DOMDocument, hr=0x%08x\n", e.Error( ) );
		return false;
	}

	//
	// Create elements
	//
	try {
		XProcessingInstruction pi = doc->createProcessingInstruction( L"xml", L"version='1.0' encoding='utf-8'" );
        doc->appendChild( pi );

		XElement FreeCompose = doc->createElement( L"FreeCompose" );
		doc->appendChild( FreeCompose );

			XElement Options = doc->createElement( L"Options" );
			FreeCompose->appendChild( Options );

				XElement Startup = doc->createElement( L"Startup" );
				Options->appendChild( Startup );

					XElement StartActive = doc->createElement( L"StartActive" );
					Startup->appendChild( StartActive );
					StartActive->text = StrFromBool( !!m_fStartActive );

					XElement StartWithWindows = doc->createElement( L"StartWithWindows" );
					Startup->appendChild( StartWithWindows );
					StartWithWindows->text = StrFromBool( !!m_fStartWithWindows );

				XElement Keyboard = doc->createElement( L"Keyboard" );
				Options->appendChild( Keyboard );

					XElement SwapCapsLock = doc->createElement( L"SwapCapsLock" );
					Keyboard->appendChild( SwapCapsLock );
					SwapCapsLock->text = StrFromBool( !!m_fSwapCapsLock );

					XElement CapsLockToggleMode = doc->createElement( L"CapsLockToggleMode" );
					Keyboard->appendChild( CapsLockToggleMode );
					CapsLockToggleMode->text = StrFromCapsLockToggleMode( m_CapsLockToggleMode );

					XElement CapsLockSwapMode = doc->createElement( L"CapsLockSwapMode" );
					Keyboard->appendChild( CapsLockSwapMode );
					CapsLockSwapMode->text = StrFromCapsLockSwapMode( m_CapsLockSwapMode );

					XElement ComposeKey = doc->createElement( L"ComposeKey" );
					Keyboard->appendChild( ComposeKey );
					ComposeKey->text = StrFromInt( m_vkCompose );

					XElement SwapCapsLockKey = doc->createElement( L"SwapCapsLockKey" );
					Keyboard->appendChild( SwapCapsLockKey );
					SwapCapsLockKey->text = StrFromInt( m_vkSwapCapsLock );

			XElement Mappings = doc->createElement( L"Mappings" );
			FreeCompose->appendChild( Mappings );

				XElement Group = doc->createElement( L"Group" );
				Mappings->appendChild( Group );
				Group->setAttribute( L"Name", L"" );

				unsigned count = 0;
				for ( int n = 0; n < (int) m_ComposeKeyEntries.GetSize( ); n++ ) {
					if ( ! m_ComposeKeyEntries[n].u32Composed ) {
						continue;
					}
					Group->appendChild( ElementFromCke( doc, m_ComposeKeyEntries[n] ) );
					count++;
				}
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::SaveToXml: Caught exception creating elements, hr=0x%08x\n", e.Error( ) );
		return false;
	}

	//
	// Save XML to disk
	//
	try {
		doc->save( (LPCWSTR) str );
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::SaveToXml: Caught exception saving configuration, hr=0x%08x\n", e.Error( ) );
		return false;
	}

	return true;
}
