#pragma once

#import <msxml6.dll> exclude( "_FILETIME", "ISequentialStream" )

//==============================================================================
// Forward declarations
//==============================================================================

class COptionsData;
class CXmlOptionsManager;
class CXmlOptionsReader;
class CXmlOptionsWriter;

//==============================================================================
// Type aliases
//==============================================================================

using XAttribute             = MSXML2::IXMLDOMAttributePtr;
using XCDATASection          = MSXML2::IXMLDOMCDATASectionPtr;
using XDocument              = MSXML2::IXMLDOMDocumentPtr;
using XDocument2             = MSXML2::IXMLDOMDocument2Ptr;
using XElement               = MSXML2::IXMLDOMElementPtr;
using XNamedNodeMap          = MSXML2::IXMLDOMNamedNodeMapPtr;
using XNode                  = MSXML2::IXMLDOMNodePtr;
using XNodeList              = MSXML2::IXMLDOMNodeListPtr;
using XParseError            = MSXML2::IXMLDOMParseErrorPtr;
using XProcessingInstruction = MSXML2::IXMLDOMProcessingInstructionPtr;
using XSchemaCollection      = MSXML2::IXMLDOMSchemaCollectionPtr;
using XText                  = MSXML2::IXMLDOMTextPtr;

//==============================================================================
// Constants
//==============================================================================

extern wchar_t const XML_NAMESPACE[];

//==============================================================================
// Class declarations
//==============================================================================

class CXmlOptionsManager {
	// Rule of Five members

	inline CXmlOptionsManager( ):
		_pOptionsData( nullptr )
	{ }

public:
	inline CXmlOptionsManager( CXmlOptionsManager const& options ) {
		operator=( options );
	}

	inline CXmlOptionsManager( CXmlOptionsManager&& options ) {
		operator=( options );
	}

	inline ~CXmlOptionsManager( ) { }

	inline CXmlOptionsManager& operator=( CXmlOptionsManager const& rhs ) {
		_pOptionsData = rhs._pOptionsData;

		return *this;
	}

	inline CXmlOptionsManager& operator=( CXmlOptionsManager&& rhs ) {
		operator=( const_cast<CXmlOptionsManager const&>( rhs ) );

		rhs._pOptionsData = nullptr;

		return *this;
	}

	// User constructors
public:
	inline CXmlOptionsManager( COptionsData* pOptionsData ):
		_pOptionsData( pOptionsData )
	{

	}

	// Overloaded operators

	bool operator==( CXmlOptionsManager const& );
	bool operator!=( CXmlOptionsManager const& );

	// Methods
public:
	bool LoadDefaultConfiguration( void );
	bool LoadFromFile( void );
	bool SaveToFile( void );

private:
	COptionsData* _pOptionsData;
	XSchemaCollection _xmlSchemaCache;
	bool _loadingDefaultConfig;

	XDocument _CreateDomDocument( void );
	bool _LoadSchema( void );

};
