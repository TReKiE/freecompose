#pragma once

#import <msxml6.dll> exclude( "_FILETIME", "ISequentialStream" )

//==============================================================================
// Forward declarations
//==============================================================================

class COptionsData;
class CXmlOptionsManager;

//==============================================================================
// Type aliases
//==============================================================================

using XAttribute             = MSXML2::IXMLDOMAttributePtr;
using XDocument              = MSXML2::IXMLDOMDocumentPtr;
using XElement               = MSXML2::IXMLDOMElementPtr;
using XNamedNodeMap          = MSXML2::IXMLDOMNamedNodeMapPtr;
using XNode                  = MSXML2::IXMLDOMNodePtr;
using XNodeList              = MSXML2::IXMLDOMNodeListPtr;
using XParseError            = MSXML2::IXMLDOMParseErrorPtr;
using XProcessingInstruction = MSXML2::IXMLDOMProcessingInstructionPtr;
using XText                  = MSXML2::IXMLDOMTextPtr;

using MethodPtr              = bool (CXmlOptionsManager::*)( XNode const& );
using XmlMethodMap           = std::map<_bstr_t, MethodPtr>;
using XmlMethodMapPair       = std::pair<_bstr_t, MethodPtr>;

//==============================================================================
// Class declarations
//==============================================================================

class CXmlOptionsManager {
	// Rule of Five members

	inline CXmlOptionsManager( ):
		_pCurrentComposeSequences( nullptr ),
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
		_pCurrentComposeSequences( nullptr ),
		_pOptionsData( pOptionsData )
	{

	}

	// Overloaded operators

	bool operator==( CXmlOptionsManager const& );
	bool operator!=( CXmlOptionsManager const& );

public:
	bool LoadDefaultConfiguration( void );
	bool LoadFromFile( void );
	bool SaveToFile( void );

private:
	ComposeSequenceArray* _pCurrentComposeSequences;
	COptionsData* _pOptionsData;

	bool _ComposeSequenceFromXNode( XNode const& value, ComposeSequence& result );

	bool _InterpretConfiguration( XDocument& doc );

	bool _DispatchChildren( wchar_t const* label,  XNode const& node, XmlMethodMap& map );
	bool _DispatchNode( wchar_t const* label, XNode const& node, XmlMethodMap& map );

	bool _InterpretSchemaVersionNode( XNode const& node );

	bool _InterpretOptionsNode( XNode const& node );
		bool _InterpretStartupNode( XNode const& node );
			bool _InterpretStartActiveNode( XNode const& node );
			bool _InterpretStartWithWindowsNode( XNode const& node );
		bool _InterpretKeyboardNode( XNode const& node );
			bool _InterpretCapsLockToggleModeNode( XNode const& node );
			bool _InterpretCapsLockSwapModeNode( XNode const& node );
			bool _InterpretComposeKeyNode( XNode const& node );
			bool _InterpretSwapCapsLockKeyNode( XNode const& node );

	bool _InterpretMappingsNode( XNode const& node );
		bool _InterpretGroupNode( XNode const& node );
			bool _InterpretMappingNode( XNode const& node );

	friend class Initializer_;
};
