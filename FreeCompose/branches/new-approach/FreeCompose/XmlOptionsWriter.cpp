#include "stdafx.h"

#include <Unicode.h>

#include "Utils.h"

#include "OptionsData.h"

#include "XmlOptionsManager.h"
#include "XmlOptionsReader.h"
#include "XmlOptionsWriter.h"
#include "XmlOptionsHelpers.h"

//==============================================================================
// Helper methods
//==============================================================================

static inline CString StringToHexadecimal( CString const& input ) {
	int index, limit = input.GetLength( );
	if ( 0 == limit ) {
		return CString( );
	}

	int cch = 0;
	UChar32* pqz = Utf16ToUtf32( input, input.GetLength( ), cch );
	if ( !pqz || !cch ) {
		return CString( );
	}

	CString tmp;
	tmp.Format( L"%X", pqz[0] );
	for ( index = 1; index < limit; index++ ) {
		tmp.AppendFormat( L",%X", pqz[index] );
	}
	delete[] pqz;
	return tmp;
}

template<typename Tparent>
static inline XNode CreateAndAppendXNode( XDocument& doc, _bstr_t const& tagName, Tparent& parent ) {
	XNode node = doc->createNode( _variant_t( NODE_ELEMENT ), tagName, XML_NAMESPACE );
	parent->appendChild( node );
	return node;
}

template<typename Tparent, typename Tvalue>
static inline XNode CreateAndAppendXNode( XDocument& doc, _bstr_t const& tagName, Tparent& parent, Tvalue const& value ) {
	XNode node = CreateAndAppendXNode( doc, tagName, parent );
	if ( node ) {
		node->text = Coerce<Tvalue, _bstr_t>( value );
	}
	return node;
}

//==============================================================================
// Implementation
//==============================================================================

XNode CXmlOptionsWriter::_SerializeRoot( void ) {
    _doc->appendChild( _doc->createProcessingInstruction( L"xml", L"version='1.0' encoding='utf-16le'" ) );
	XNode FcConfiguration = CreateAndAppendXNode( _doc, L"FcConfiguration", _doc );
	XElement FcConfigurationElement = FcConfiguration;
	FcConfigurationElement->setAttribute( L"xmlns:xsi", L"http://www.w3.org/2001/XMLSchema-instance" );
	FcConfigurationElement->setAttribute( L"xsi:noNamespaceSchemaLocation", L"FcConfiguration.xsd" );
	return FcConfiguration;
}

void CXmlOptionsWriter::_SerializeOptions( XNode& FcConfiguration ) {
	XNode Options = CreateAndAppendXNode( _doc, L"Options", FcConfiguration );
	_SerializeStartupOptions( Options );
	_SerializeKeyboardOptions( Options );
	_SerializeSoundsOptions( Options );
}

void CXmlOptionsWriter::_SerializeStartupOptions( XNode& Options ) {
	XNode Startup = CreateAndAppendXNode( _doc, L"Startup", Options );

	XNode StartActive      = CreateAndAppendXNode( _doc, L"StartActive",      Startup, BoolStringMapper[ !!_pOptionsData->StartActive ] );
	XNode StartWithWindows = CreateAndAppendXNode( _doc, L"StartWithWindows", Startup, BoolStringMapper[ !!_pOptionsData->StartWithWindows ] );
}

void CXmlOptionsWriter::_SerializeKeyboardOptions( XNode& Options ) {
	XNode Keyboard = CreateAndAppendXNode( _doc, L"Keyboard", Options );

	XNode CapsLockToggleMode = CreateAndAppendXNode( _doc, L"CapsLockToggleMode", Keyboard, CltmStringMapper[ _pOptionsData->CapsLockToggleMode ] );
	XNode CapsLockSwapMode   = CreateAndAppendXNode( _doc, L"CapsLockSwapMode",   Keyboard, ClsmStringMapper[ _pOptionsData->CapsLockSwapMode ] );
	XNode ComposeKey         = CreateAndAppendXNode( _doc, L"ComposeKey",         Keyboard, _pOptionsData->ComposeVk );
	XNode SwapCapsLockKey    = CreateAndAppendXNode( _doc, L"SwapCapsLockKey",    Keyboard, _pOptionsData->SwapCapsLockVk );
}

void CXmlOptionsWriter::_SerializeSoundsOptions( XNode& Options ) {
	XNode Sounds = CreateAndAppendXNode( _doc, L"Sounds", Options );

	for ( auto& scheme : _pOptionsData->Sounds.Schemes ) {
		_SerializeSoundScheme( Sounds, scheme );
	}
}

void CXmlOptionsWriter::_SerializeSoundScheme( XNode& Sounds, SoundScheme const& scheme ) {
	XNode Scheme = CreateAndAppendXNode( _doc, L"Scheme", Sounds );
	XElement SchemeElement = Scheme;
	SchemeElement->setAttribute( L"ID",   static_cast<LPCWSTR>( scheme.ID   ) );
	SchemeElement->setAttribute( L"Name", static_cast<LPCWSTR>( scheme.Name ) );

	for ( SoundEventMapPair const& pair : scheme.Events ) {
		_SerializeSoundEvent( Scheme, pair );
	}
}

void CXmlOptionsWriter::_SerializeSoundEvent( XNode& Scheme, SoundEventMapPair const& pair ) {
	XNode SoundEventNode = CreateAndAppendXNode( _doc, L"SoundEvent", Scheme );
	XElement SoundEventElement = SoundEventNode;
	SoundEventElement->setAttribute( L"Name", static_cast<LPCWSTR>( pair.first ) );

	SoundEvent* pSoundEvent = pair.second;
	if ( typeid( NoSoundEvent ) == typeid( *pSoundEvent ) ) {
		XNode NoSound = CreateAndAppendXNode( _doc, L"NoSound", SoundEventNode );
	} else if ( typeid( ApplicationSoundEvent ) == typeid( *pSoundEvent ) ) {
		XNode ApplicationSound = CreateAndAppendXNode( _doc, L"ApplicationSound", SoundEventNode );
	} else if ( typeid( ToneSoundEvent ) == typeid( *pSoundEvent ) ) {
		ToneSoundEvent* pToneSoundEvent = dynamic_cast<ToneSoundEvent*>( pair.second );
		CString frequency, duration;
		frequency.Format( L"%u", pToneSoundEvent->Frequency );
		duration.Format( L"%u", pToneSoundEvent->Duration );

		XNode Tone = CreateAndAppendXNode( _doc, L"Tone", SoundEventNode );
		XElement ToneElement = Tone;
		ToneElement->setAttribute( L"Frequency", static_cast<LPCWSTR>( frequency ) );
		ToneElement->setAttribute( L"Duration",  static_cast<LPCWSTR>( duration  ) );
	} else {
		debug( L"CXmlOptionsWriter::_SerializeSoundEvent: Unknown sound event type found while saving sound schemas. Name: '%s'\n", static_cast<LPCWSTR>( pair.first ) );
	}
}

void CXmlOptionsWriter::_SerializeMappings( XNode& FcConfiguration ) {
	XNode Mappings = CreateAndAppendXNode( _doc, L"Mappings", FcConfiguration );

	int groupCount = static_cast<int>( _pOptionsData->ComposeSequenceGroups.GetCount( ) );
	for ( int groupIndex = 0; groupIndex < groupCount; groupIndex++ ) {
		ComposeSequenceGroup& composeSequenceGroup = _pOptionsData->ComposeSequenceGroups[groupIndex];
		if ( 0 == composeSequenceGroup.ComposeSequences.GetCount( ) ) {
			debug( L"CXmlOptionsWriter::_SerializeMappings: skipping group #%d '%s'", groupIndex, static_cast<LPCWSTR>( composeSequenceGroup.Name ) );
			continue;
		}

		XNode Group = CreateAndAppendXNode( _doc, L"Group", Mappings );
		XElement GroupElement = Group;
		if ( !composeSequenceGroup.Name.IsEmpty( ) ) {
			GroupElement->setAttribute( L"Name", static_cast<LPCWSTR>( composeSequenceGroup.Name ) );
		}

		int sequenceCount = static_cast<int>( composeSequenceGroup.ComposeSequences.GetSize( ) );
		for ( int sequenceIndex = 0; sequenceIndex < sequenceCount; sequenceIndex++ ) {
			ComposeSequence& composeSequence = composeSequenceGroup.ComposeSequences[sequenceIndex];

			if ( composeSequence.Sequence.GetLength( ) == 0 || composeSequence.Result.GetLength( ) == 0 ) {
				continue;
			}

			XNode Mapping = CreateAndAppendXNode( _doc, L"Mapping", Group );
			XElement MappingElement = Mapping;
			if ( composeSequence.Disabled ) {
				MappingElement->setAttribute( L"Disabled", BoolStringMapper[composeSequence.Disabled] );
			}
			if ( composeSequence.CaseInsensitive ) {
				MappingElement->setAttribute( L"CaseInsensitive", BoolStringMapper[composeSequence.CaseInsensitive] );
			}
			if ( composeSequence.Reversible ) {
				MappingElement->setAttribute( L"Reversible", BoolStringMapper[composeSequence.Reversible] );
			}

			XNode sequence = CreateAndAppendXNode( _doc, L"Sequence", Mapping, StringToHexadecimal( composeSequence.Sequence ) );
			XNode result   = CreateAndAppendXNode( _doc, L"Result",   Mapping, StringToHexadecimal( composeSequence.Result   ) );
		}
	}
}

bool CXmlOptionsWriter::SerializeConfiguration( ) {
	//
	// Create elements
	//
	try {
		XNode FcConfiguration = _SerializeRoot( );
		_SerializeOptions( FcConfiguration );
		_SerializeMappings( FcConfiguration );
	}
	catch ( _com_error e ) {
		debug( L"CXmlOptionsWriter::SerializeConfiguration: Caught exception creating XML elements, hr=0x%08lX\n", e.Error( ) );
		return false;
	}

	return true;
}