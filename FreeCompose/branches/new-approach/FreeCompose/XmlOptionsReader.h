#pragma once

//==============================================================================
// Class declarations
//==============================================================================

using MethodPtr        = bool (CXmlOptionsReader::*)( XNode const& );
using XmlMethodMap     = std::map<_bstr_t, MethodPtr>;
using XmlMethodMapPair = std::pair<_bstr_t, MethodPtr>;

class CXmlOptionsReader {

public:
	CXmlOptionsReader( ):
		_pOptionsData             ( nullptr ),
		_pCurrentComposeSequences ( nullptr )
	{

	}
	CXmlOptionsReader( COptionsData* pOptionsData ):
		_pOptionsData             ( pOptionsData ),
		_pCurrentComposeSequences ( nullptr )
	{
		
	}

	~CXmlOptionsReader( )
	{
		
	}

	bool InterpretConfiguration( XDocument& doc );

protected:
	COptionsData* _pOptionsData;

	ComposeSequenceArray* _pCurrentComposeSequences;
	CString _currentSoundSchemeName;
	CString _currentSoundEventName;

	bool _DispatchChildren( wchar_t const* label,  XNode const& node, XmlMethodMap& map );
	bool _DispatchNode( wchar_t const* label, XNode const& node, XmlMethodMap& map );

	bool _InterpretOptionsNode( XNode const& node );
		bool _InterpretStartupNode( XNode const& node );
			bool _InterpretStartActiveNode( XNode const& node );
			bool _InterpretStartWithWindowsNode( XNode const& node );
		bool _InterpretKeyboardNode( XNode const& node );
			bool _InterpretCapsLockToggleModeNode( XNode const& node );
			bool _InterpretCapsLockSwapModeNode( XNode const& node );
			bool _InterpretComposeKeyNode( XNode const& node );
			bool _InterpretSwapCapsLockKeyNode( XNode const& node );
		bool _InterpretSoundsNode( XNode const& node );
			bool _InterpretSchemeNode( XNode const& node );
				bool _InterpretSoundEventNode( XNode const& node );
					bool _InterpretNoSoundNode( XNode const& node );
					bool _InterpretApplicationSoundNode( XNode const& node );
					bool _InterpretToneNode( XNode const& node );
	bool _InterpretMappingsNode( XNode const& node );
		bool _InterpretGroupNode( XNode const& node );
			bool _InterpretMappingNode( XNode const& node );

	friend class _StaticInitializer;

};
