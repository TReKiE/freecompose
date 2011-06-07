#include "stdafx.h"

#import <msxml6.dll>

#include "FreeCompose.h"
#include "OptionsData.h"
#include "Utils.h"

//==============================================================================
// Type aliases
//==============================================================================

typedef MSXML2::IXMLDOMDocumentPtr XDocument;
typedef MSXML2::IXMLDOMProcessingInstructionPtr XProcessingInstruction;
typedef MSXML2::IXMLDOMElementPtr XElement;
typedef MSXML2::IXMLDOMAttributePtr XAttribute;

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
// Functions for XML type mapping
//==============================================================================

static inline _bstr_t strFromBool( const bool _ ) {
	return mapBoolToString[_];
}

static inline _bstr_t strFromCapsLockToggleMode( const CAPS_LOCK_TOGGLE_MODE _ ) {
	return mapCapsLockToggleModeToString[_];
}

static inline _bstr_t strFromCapsLockSwapMode( const CAPS_LOCK_SWAP_MODE _ ) {
	return _bstr_t( mapCapsLockSwapModeToString[_] );
}

static inline _bstr_t strFromInt( const int _ ) {
	return _bstr_t( _variant_t( _ ) );
}

static XElement CkeToXml( XDocument doc, COMPOSE_KEY_ENTRY& cke ) {
	XElement mapping = doc->createElement( L"Mapping" );

		XElement first = doc->createElement( L"First" );
		mapping->appendChild( first );
		if ( ( cke.vkFirst & 0x80000000 ) != 0 ) {
			first->appendChild( doc->createElement( L"Shift" ) );
		}
		first->appendChild( doc->createTextNode( _bstr_t( _variant_t( (unsigned char) ( cke.vkFirst & 0xFF ) ) ) ) );

		XElement second = doc->createElement( L"Second" );
		mapping->appendChild( second );
		if ( ( cke.vkSecond & 0x80000000 ) != 0 ) {
			second->appendChild( doc->createElement( L"Shift" ) );
		}
		second->appendChild( doc->createTextNode( _bstr_t( _variant_t( (unsigned char) ( cke.vkSecond & 0xFF ) ) ) ) );

		XElement composed = doc->createElement( L"Composed" );
		mapping->appendChild( composed );
		composed->text = _bstr_t( _variant_t( (unsigned) cke.u32Composed ) );

	return mapping;
}

bool COptionsData::LoadFromXml( void ) {
	return false;
}

bool COptionsData::SaveToXml( void ) {
	if ( ! EnsureFreeComposeFolderExists( ) ) {
		debug( L"COptionsData::SaveToXml: Can't make sure app data folder exists\n" );
		return false;
	}

	CString str( GetFreeComposeFolder( ) );
	str.Append( L"\\FreeCompose.xml" );

	XDocument doc;
	HRESULT hr = doc.CreateInstance( __uuidof( MSXML2::DOMDocument60 ), NULL, CLSCTX_INPROC_SERVER );
	if ( FAILED(hr) ) {
		debug( L"COptionsData::SaveToXml: Can't create instance of DOMDocument: hr=0x%08x\n", hr );
		return false;
	}

	//
	// Configure DOMDocument object
	//
	try {
		doc->async = VARIANT_FALSE;
		doc->validateOnParse = VARIANT_FALSE;
		doc->resolveExternals = VARIANT_FALSE;
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
					StartActive->text = strFromBool( !!m_fStartActive );

					XElement StartWithWindows = doc->createElement( L"StartWithWindows" );
					Startup->appendChild( StartWithWindows );
					StartWithWindows->text = strFromBool( !!m_fStartWithWindows );

				XElement Keyboard = doc->createElement( L"Keyboard" );
				Options->appendChild( Keyboard );

					XElement SwapCapsLock = doc->createElement( L"SwapCapsLock" );
					Keyboard->appendChild( SwapCapsLock );
					SwapCapsLock->text = strFromBool( !!m_fSwapCapsLock );

					XElement CapsLockToggleMode = doc->createElement( L"CapsLockToggleMode" );
					Keyboard->appendChild( CapsLockToggleMode );
					CapsLockToggleMode->text = strFromCapsLockToggleMode( m_CapsLockToggleMode );

					XElement CapsLockSwapMode = doc->createElement( L"CapsLockSwapMode" );
					Keyboard->appendChild( CapsLockSwapMode );
					CapsLockSwapMode->text = strFromCapsLockSwapMode( m_CapsLockSwapMode );

					XElement ComposeKey = doc->createElement( L"ComposeKey" );
					Keyboard->appendChild( ComposeKey );
					ComposeKey->text = strFromInt( m_vkCompose );

					XElement SwapCapsLockKey = doc->createElement( L"SwapCapsLockKey" );
					Keyboard->appendChild( SwapCapsLockKey );
					SwapCapsLockKey->text = strFromInt( m_vkSwapCapsLock );

			XElement Mappings = doc->createElement( L"Mappings" );
			FreeCompose->appendChild( Mappings );

				unsigned count = 0;
				for ( int n = 0; n < (int) m_ComposeKeyEntries.GetSize( ); n++ ) {
					if ( ! m_ComposeKeyEntries[n].u32Composed ) {
						continue;
					}
					Mappings->appendChild( CkeToXml( doc, m_ComposeKeyEntries[n] ) );
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
