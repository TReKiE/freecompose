#pragma once

//==============================================================================
// Class declarations
//==============================================================================

class CXmlOptionsWriter {

public:
	CXmlOptionsWriter( ) = delete;

	CXmlOptionsWriter( COptionsData* pOptionsData, XDocument& doc ):
		_pOptionsData ( pOptionsData ),
		_doc          ( doc )
	{

	}

	~CXmlOptionsWriter( )
	{

	}

	bool SerializeConfiguration( );

protected:
	COptionsData* _pOptionsData;
	XDocument& _doc;

	XNode _SerializeRoot( void );
	void _SerializeOptions( XNode& FcConfiguration );
	void _SerializeStartupOptions( XNode& Options );
	void _SerializeKeyboardOptions( XNode& Options );
	void _SerializeSoundsOptions( XNode& Options );
	void _SerializeSoundScheme( XNode& Sounds, SoundScheme const& scheme );
	void _SerializeSoundEvent( XNode& Scheme, SoundEventMapPair const& pair );
	void _SerializeMappings( XNode& FcConfiguration );

};
