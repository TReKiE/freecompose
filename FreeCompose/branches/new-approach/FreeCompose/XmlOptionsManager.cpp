#include "stdafx.h"

#include "FreeCompose.h"
#include "OptionsData.h"
#include "StringMapper.h"
#include "Utils.h"

#include <Unicode.h>

#include "XmlOptionsManager.h"
#include "XmlOptionsReader.h"
#include "XmlOptionsWriter.h"
#include "XmlOptionsHelpers.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

//==============================================================================
// Constants
//==============================================================================

wchar_t const XML_NAMESPACE[] = L"http://www.zive.ca/xmlns/FreeCompose/configuration/1";

//==============================================================================
// Local functions
//==============================================================================

static inline bool LoadBinaryResource( unsigned const uID, void*& pvResource, size_t& cbResource ) {
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

static inline BSTR LoadBinaryResourceAsBstr( unsigned const uID ) {
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

static inline bool _LoadXDocumentFromResource( unsigned const uID, XDocument& doc ) {
	_bstr_t bstrXml( LoadBinaryResourceAsBstr( uID ), false );

	try {
		_variant_t result = doc->loadXML( bstrXml );
		if ( !static_cast<VARIANT_BOOL>( result ) ) {
			debug( L"_LoadXDocumentFromResource: doc->loadXML failed: line %ld column %ld: hr=0x%08lX %s", doc->parseError->line, doc->parseError->linepos, doc->parseError->errorCode, static_cast<LPCWSTR>( doc->parseError->reason ) );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"_LoadXDocumentFromResource: Caught exception loading default configuration, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	return true;
}

//==============================================================================
// CXmlOptionsManager implementation
//==============================================================================

//==============================================================================
// CXmlOptionsManager::_CreateDomDocument
//

inline XDocument CXmlOptionsManager::_CreateDomDocument( void ) {
	XDocument doc;
	HRESULT hr = doc.CreateInstance( L"Msxml2.DOMDocument.6.0" );
	if ( FAILED( hr ) ) {
		debug( L"CXmlOptionsManager::_CreateDomDocument: failed, hr=0x%08lX\n", hr );
		return doc;
	}

	try {
		doc->async = VARIANT_FALSE;
		doc->preserveWhiteSpace = VARIANT_FALSE;
		doc->resolveExternals = VARIANT_FALSE;

		if ( _xmlSchemaCache ) {
			XDocument2 doc2 = doc;
			doc2->schemas = _xmlSchemaCache.GetInterfacePtr( );
			doc->validateOnParse = VARIANT_TRUE;
		} else {
			doc->validateOnParse = VARIANT_FALSE;
		}
	}
	catch ( _com_error e ) {
		debug( L"CXmlOptionsManager::_CreateDomDocument: Caught exception setting up DOMDocument, hr=0x%08lX\n", e.Error( ) );
		doc.Release( );
	}
	return doc;
}

//==============================================================================
// CXmlOptionsManager::_LoadSchema
//

bool CXmlOptionsManager::_LoadSchema( void ) {
	_xmlSchemaCache = nullptr;

	XDocument schemaDoc = _CreateDomDocument( );
	if ( !schemaDoc ) {
		debug( L"CXmlOptionsManager::_LoadSchema: Can't create instance of DOMDocument\n" );
		return false;
	}

	if ( !_LoadXDocumentFromResource( IDX_FCCONFIGURATION_SCHEMA, schemaDoc ) ) {
		debug( L"CXmlOptionsManager::_LoadSchema: _LoadXDocumentFromResource failed\n" );
		return false;
	}

	HRESULT hr = _xmlSchemaCache.CreateInstance( L"Msxml2.XMLSchemaCache.6.0" );
	if ( FAILED( hr ) ) {
		debug( L"CXmlOptionsManager::_LoadSchema: Can't create instance of XMLSchemaCache, hr=0x%08lX\n", hr );
		return false;
	}

	try {
		_xmlSchemaCache->add( _bstr_t( XML_NAMESPACE ), _variant_t( schemaDoc.GetInterfacePtr( ) ) );
	}
	catch ( _com_error e ) {
		debug( L"CXmlOptionsManager::_LoadSchema: Caught exception creating elements, hr=0x%08lX\n", e.Error( ) );
		IErrorInfoPtr pErrorInfo = e.ErrorInfo( );
		if ( pErrorInfo ) {
			BSTR bstrDescription, bstrSource;
			hr = pErrorInfo->GetDescription( &bstrDescription );
			hr = pErrorInfo->GetSource( &bstrSource );
			debug( L"+ Description: %s\n+ Source: %s\n", bstrDescription, bstrSource );
		}
		return false;
	}
	return true;
}

//==============================================================================
// CXmlOptionsManager::LoadDefaultConfiguration
//

bool CXmlOptionsManager::LoadDefaultConfiguration( void ) {
	_loadingDefaultConfig = true;

	if ( !_xmlSchemaCache && !_LoadSchema( ) ) {
		debug( L"CXmlOptionsManager::LoadDefaultConfiguration: Can't load schema, but carrying on\n" );
	}

	XDocument doc = _CreateDomDocument( );
	if ( !doc ) {
		debug( L"CXmlOptionsManager::LoadDefaultConfiguration: Can't create instance of DOMDocument\n" );
		return false;
	}

	if ( !_LoadXDocumentFromResource( IDX_DEFAULT_CONFIGURATION, doc ) ) {
		debug( L"CXmlOptionsManager::LoadDefaultConfiguration: _LoadXDocumentFromResource failed\n" );
		return false;
	}

	bool ret = CXmlOptionsReader( _pOptionsData ).InterpretConfiguration( doc );
	doc = nullptr;
	_loadingDefaultConfig = false;
	return ret;
}

//==============================================================================
// CXmlOptionsManager::LoadFromFile
//

bool CXmlOptionsManager::LoadFromFile( void ) {
	_loadingDefaultConfig = false;

	if ( !EnsureFreeComposeFolderExists( ) ) {
		debug( L"CXmlOptionsManager::LoadFromFile: Can't ensure app data folder exists\n" );
		return false;
	}

	if ( !_xmlSchemaCache && !_LoadSchema( ) ) {
		debug( L"CXmlOptionsManager::LoadFromFile: Can't load schema, but carrying on\n" );
	}

	CString str( GetFreeComposeFolder( ) + L"\\FcConfiguration.xml" );
	XDocument doc;
	try {
		doc = _CreateDomDocument( );
		if ( !doc ) {
			debug( L"CXmlOptionsManager::LoadFromFile: Can't create instance of DOMDocument\n" );
			return false;
		}
		if ( !_LoadSchema( ) ) {
			debug( L"CXmlOptionsManager::LoadDefaultConfiguration: Can't load schema, but carrying on\n" );
		}

		//
		// Load XML from disk
		//
		_variant_t result = doc->load( _variant_t( str ) );
		if ( !static_cast<VARIANT_BOOL>( result ) ) {
			debug( L"CXmlOptionsManager::LoadFromFile: doc->load failed: line %ld column %ld: hr=0x%08lX %s", doc->parseError->line, doc->parseError->linepos, doc->parseError->errorCode, static_cast<wchar_t const*>( doc->parseError->reason ) );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"CXmlOptionsManager::LoadFromFile: Caught exception loading configuration file, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	return CXmlOptionsReader( _pOptionsData ).InterpretConfiguration( doc );
}

//==============================================================================
// CXmlOptionsManager::SaveToFile
//

bool CXmlOptionsManager::SaveToFile( void ) {
	if ( !EnsureFreeComposeFolderExists( ) ) {
		debug( L"CXmlOptionsManager::SaveToFile: Can't ensure app data folder exists\n" );
		return false;
	}

	CString str( GetFreeComposeFolder( ) + L"\\FcConfiguration.xml" );

	XDocument doc = _CreateDomDocument( );
	if ( !doc ) {
		debug( L"CXmlOptionsManager::SaveToFile: Can't create instance of DOMDocument\n" );
		return false;
	}

	if ( !CXmlOptionsWriter( _pOptionsData, doc ).SerializeConfiguration( ) ) {
		debug( L"CXmlOptionsManager::SaveToFile: CXmlOptionsWriter::SerializeConfiguration() failed\n" );
		return false;
	}

	//
	// Save XML to disk
	//
	try {
		HRESULT hr = doc->save( _variant_t( str ) );
		if ( FAILED( hr ) ) {
			debug( L"CXmlOptionsManager::SaveToFile: doc->save failed, hr=0x%08lX\n", hr );
			return false;
		}
	}
	catch ( _com_error e ) {
		debug( L"CXmlOptionsManager::SaveToFile: Caught exception saving configuration, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	return true;
}
