#include "stdafx.h"

#import <msxml6.dll>

#include "FreeCompose.h"
#include "OptionsData.h"
#include "Utils.h"

//==============================================================================
// Types and type aliases
//==============================================================================

using XAttribute             = MSXML2::IXMLDOMAttributePtr;
using XDocument              = MSXML2::IXMLDOMDocumentPtr;
using XElement               = MSXML2::IXMLDOMElementPtr;
using XNode                  = MSXML2::IXMLDOMNodePtr;
using XNodeList              = MSXML2::IXMLDOMNodeListPtr;
using XProcessingInstruction = MSXML2::IXMLDOMProcessingInstructionPtr;
using XParseError            = MSXML2::IXMLDOMParseErrorPtr;

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
	L"normal",
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

static XElement ElementFromSequence( XDocument doc, COMPOSE_SEQUENCE& sequence ) {
	XElement mapping = doc->createElement( L"Mapping" );

		XElement first = doc->createElement( L"First" );
		mapping->appendChild( first );
		first->text = _bstr_t( _variant_t( (unsigned) sequence.chFirst ) );

		XElement second = doc->createElement( L"Second" );
		mapping->appendChild( second );
		second->text = _bstr_t( _variant_t( (unsigned) sequence.chSecond ) );

		XElement composed = doc->createElement( L"Composed" );
		mapping->appendChild( composed );
		composed->text = _bstr_t( _variant_t( (unsigned) sequence.chComposed ) );

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

template<typename T> static inline T FromXElement( const XElement& _ ) {
	return (T) _variant_t( _->text );
}

static COMPOSE_SEQUENCE SequenceFromElement( XElement Mapping ) {
	COMPOSE_SEQUENCE sequence;
	memset( &sequence, 0, sizeof( sequence ) );
	sequence.chFirst = FromXElement<unsigned>( Mapping->selectSingleNode( L"First" ) );
	sequence.chSecond = FromXElement<unsigned>( Mapping->selectSingleNode( L"Second" ) );
	sequence.chComposed = FromXElement<unsigned>( Mapping->selectSingleNode( L"Composed" ) );

	return sequence;
}

static inline bool CompareNodeName( XElement elt, LPCWSTR name ) {
	return 0 == ( (CString) (LPCWSTR) elt->nodeName ).Compare( name );
}

static inline XDocument CreateDOMDocument( void ) {
	XDocument doc = nullptr;
	HRESULT hr = doc.CreateInstance( L"Msxml2.DOMDocument.6.0" );
	if ( FAILED( hr ) ) {
		debug( L"CreateDOMDocument: failed, hr=0x%08lX\n", hr );
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
	// Configure DOMDocument object
	//
	try {
		doc->async = VARIANT_FALSE;
		doc->validateOnParse = VARIANT_FALSE;
		doc->resolveExternals = VARIANT_FALSE;
		doc->preserveWhiteSpace = VARIANT_FALSE;
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::_LoadFromXml: Caught exception setting up DOMDocument, hr=0x%08lX\n", e.Error( ) );
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

		XElement Options = FreeCompose->selectSingleNode( L"Options" );

		XElement Startup = Options->selectSingleNode( L"Startup" );
		m_fStartActive = BoolFromXNode( Startup->selectSingleNode( L"StartActive" ) );
		m_fStartWithWindows = BoolFromXNode( Startup->selectSingleNode( L"StartWithWindows" ) );

		XElement Keyboard = Options->selectSingleNode( L"Keyboard" );
		m_CapsLockToggleMode = CapsLockToggleModeFromXElement( Keyboard->selectSingleNode( L"CapsLockToggleMode" ) );
		m_CapsLockSwapMode = CapsLockSwapModeFromXElement( Keyboard->selectSingleNode( L"CapsLockSwapMode" ) );
		m_vkCompose = FromXElement<unsigned>( Keyboard->selectSingleNode( L"ComposeKey" ) );
		m_vkSwapCapsLock = FromXElement<unsigned>( Keyboard->selectSingleNode( L"SwapCapsLockKey" ) );

		XElement Mappings = FreeCompose->selectSingleNode( L"Mappings" );
		XNodeList groupNodes = Mappings->selectNodes( L"Group" );
		for ( XElement group = groupNodes->nextNode( ); NULL != group; group = groupNodes->nextNode( ) ) {
			XNodeList mappingNodes = group->selectNodes( L"Mapping" );
			for ( XElement mapping = mappingNodes->nextNode( ); NULL != mapping; mapping = mappingNodes->nextNode( ) ) {
				m_ComposeSequences.Add( SequenceFromElement( mapping ) );
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
	// Configure DOMDocument object
	//
	try {
		doc->async = VARIANT_FALSE;
		doc->validateOnParse = VARIANT_FALSE;
		doc->resolveExternals = VARIANT_FALSE;
		doc->preserveWhiteSpace = VARIANT_FALSE;
	}
	catch ( _com_error e ) {
		debug( L"COptionsData::_SaveToXml: Caught exception setting up DOMDocument, hr=0x%08lX\n", e.Error( ) );
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

			XElement SyntaxVersion = doc->createElement( L"SyntaxVersion" );
			SyntaxVersion->text = L"2";
			FreeCompose->appendChild( SyntaxVersion );

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
				for ( int n = 0; n < (int) m_ComposeSequences.GetSize( ); n++ ) {
					if ( ! m_ComposeSequences[n].chComposed ) {
						continue;
					}
					Group->appendChild( ElementFromSequence( doc, m_ComposeSequences[n] ) );
					count++;
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
