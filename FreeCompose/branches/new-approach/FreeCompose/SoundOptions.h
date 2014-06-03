#pragma once

//
// Forward declarations
//

class SoundScheme;
class SoundEvent;

//
// SoundOptions: Root of sound config classes
//

using SoundSchemeVector = std::vector<SoundScheme>;

class SoundOptions {

public:
	inline SoundOptions( ) { }
	inline SoundOptions( SoundOptions const& rhs ) { *this = rhs; }
	inline ~SoundOptions( ) { }

	inline SoundOptions& operator=( SoundOptions const& rhs ) {
		std::copy( rhs.Schemes.begin( ), rhs.Schemes.end( ), Schemes.begin( ) );
		return *this;
	}

	inline bool operator==( SoundOptions const& rhs ) const;
	inline bool operator!=( SoundOptions const& rhs ) const {
		return !( *this == rhs );
	}

	inline SoundScheme* GetSoundScheme( CString const& name );

	SoundSchemeVector Schemes;

};

//
// SoundScheme: contains a collection of event configuration
//

using SoundEventMap = std::map<CString, SoundEvent*>;
using SoundEventMapPair = std::pair<CString, SoundEvent*>;

class SoundScheme {

public:
	inline SoundScheme( ) { }
	inline SoundScheme( SoundScheme const& rhs ) { *this = rhs; }
	inline SoundScheme( CString const& id, CString const& name ): ID( id ), Name( name ) { }
	inline ~SoundScheme( );

	inline SoundScheme& operator=( SoundScheme const& rhs ) {
		ID = rhs.ID;
		Name = rhs.Name;
		for ( auto& pair : rhs.Events ) {
			Events.insert( SoundEventMapPair( pair.first, pair.second ) );
		}
		return *this;
	}

	inline bool operator==( SoundScheme const& rhs ) const {
		return
			( 0 == ID.Compare( rhs.ID ) ) &&
			( 0 == Name.Compare( rhs.Name ) ) &&
			( Events == rhs.Events );
	}

	inline bool operator!=( SoundScheme const& rhs ) const {
		return !( *this == rhs );
	}

	CString ID;
	CString Name;

	SoundEventMap Events;

};

// The following method is defined here because class SoundScheme is incomplete until this point.
inline SoundScheme* SoundOptions::GetSoundScheme( CString const& name ) {
	for ( auto& s : Schemes ) {
		if ( 0 == s.Name.CompareNoCase( name ) ) {
			return &s;
		}
	}
	return nullptr;
}

// The following method is defined here because class SoundScheme is incomplete until this point.
inline bool SoundOptions::operator==( SoundOptions const& rhs ) const {
	std::vector<bool> foundIndex;
	int n = 0;
	for ( auto& schemeL : Schemes ) {
		bool found = false;
		for ( auto& schemeR : rhs.Schemes ) {
			if ( schemeL == schemeR ) {
				found = true;
				break;
			}
		}
		foundIndex[n] = found;
		n++;
	}

	SoundSchemeVector::size_type size = 0;
	for ( auto flag : foundIndex ) {
		if ( flag ) {
			size++;
		}
	}
	return ( size == Schemes.size( ) );
}

//
// SoundEvent: Base class for event configuration
//

class SoundEvent {

public:
	inline SoundEvent( ) { }
	inline SoundEvent( CString const& name ): Name( name ) { }
	inline SoundEvent( SoundEvent const& rhs ) { *this = rhs; }
	inline virtual ~SoundEvent( ) { }

	inline virtual SoundEvent& operator=( SoundEvent const& rhs ) {
		Name = rhs.Name;
		return *this;
	}

	inline virtual bool operator==( SoundEvent const& rhs ) const {
		return ( 0 == Name.Compare( rhs.Name ) );
	}

	inline virtual bool operator!=( SoundEvent const& rhs ) const {
		return !( *this == rhs );
	}

	inline virtual bool PlaySound( void ) {
		return false;
	}

	CString Name;

};

// The following method is defined here because class SoundEvent is incomplete until this point.
inline SoundScheme::~SoundScheme( ) {
	for ( auto p : Events ) {
		delete p.second;
	}
	Events.clear( );
}

//
// NoSoundEvent: Silent event.
//

class NoSoundEvent: public SoundEvent {

public:
	inline NoSoundEvent( ): SoundEvent( ) { }
	inline NoSoundEvent( NoSoundEvent const& rhs ) { *this = rhs; }
	inline NoSoundEvent( CString const& name ): SoundEvent( name ) { }
	inline virtual ~NoSoundEvent( ) { }

	inline virtual bool PlaySound( void ) {
		return true;
	}

};

//
// ApplicationSoundEvent: Play a WAV file configured in the "Sounds" control panel via PlaySound().
//

class ApplicationSoundEvent: public SoundEvent {

public:
	inline ApplicationSoundEvent( ): SoundEvent( ) { }
	inline ApplicationSoundEvent( ApplicationSoundEvent const& rhs ) { *this = rhs; }
	inline ApplicationSoundEvent( CString const& name ): SoundEvent( name ) { }
	inline virtual ~ApplicationSoundEvent( ) { }

	inline virtual bool PlaySound( void ) {
		return false;
	}

};

//
// ToneSoundEvent: Play a specified frequency for a specified duration.
//

class ToneSoundEvent: public SoundEvent {

public:
	inline ToneSoundEvent( ): SoundEvent( ), Frequency( 0 ), Duration( 0 ) { }
	inline ToneSoundEvent( ToneSoundEvent const& rhs ) { *this = rhs; }
	inline ToneSoundEvent( CString const& name, unsigned const frequency, unsigned const duration ): SoundEvent( name ), Frequency( frequency ), Duration( duration ) { }
	inline virtual ~ToneSoundEvent( ) { }

	inline virtual ToneSoundEvent& operator=( ToneSoundEvent const& rhs ) {
		SoundEvent::operator=( rhs );
		Frequency = rhs.Frequency;
		Duration = rhs.Duration;
		return *this;
	}

	inline virtual bool operator==( ToneSoundEvent const& rhs ) const {
		return
			SoundEvent::operator==( rhs ) &&
			( Frequency == rhs.Frequency ) &&
			( Duration == rhs.Duration );
	}

	inline virtual bool operator!=( ToneSoundEvent const& rhs ) const {
		return !( *this == rhs );
	}

	inline virtual bool PlaySound( void ) {
		return false;
	}

	unsigned Frequency;
	unsigned Duration;

};
