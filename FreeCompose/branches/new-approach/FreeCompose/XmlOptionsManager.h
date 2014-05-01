#pragma once

#import <msxml6.dll>

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
using XText                  = MSXML2::IXMLDOMTextPtr;

//==============================================================================
// Forward declarations
//==============================================================================

class COptionsData;

//==============================================================================
// Class declarations
//==============================================================================

class CXmlOptionsManager {
	// Rule of Five
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

	inline ~CXmlOptionsManager( ) {
	}

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
	inline CXmlOptionsManager( COptionsData* pOptionsData ) {
		_pOptionsData = pOptionsData;
	}


	// Overloaded operators

	bool operator==( CXmlOptionsManager const& );
	bool operator!=( CXmlOptionsManager const& );

public:
	bool LoadDefaultConfiguration( void );
	bool LoadXmlFile( void );
	bool SaveXmlFile( void );

private:
	COptionsData* _pOptionsData;

	bool _InterpretConfiguration( XDocument& doc );

	bool _DispatchNode( XNode const& node );
	bool _InterpretSchemaVersionNode( XNode const& node );

	friend class Initializer_;
};
