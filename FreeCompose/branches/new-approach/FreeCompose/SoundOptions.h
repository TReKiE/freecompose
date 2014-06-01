#pragma once

//
// Forward declarations
//

class SoundScheme;
class SoundEvent;


//
// SoundOptions: Root of sound config classes
//

class SoundOptions {

public:
	inline SoundOptions( ) { }
	inline ~SoundOptions( ) { }

	std::vector<SoundScheme> Schemes;

};

//
// SoundScheme: contains a collection of event configuration
//

class SoundScheme {

public:
	inline SoundScheme( ) { }
	inline SoundScheme( CString const& id, CString const& name ): ID( id ), Name( name ) { }
	inline ~SoundScheme( ) { }

	CString ID;
	CString Name;

	std::map<CString, SoundEvent> Events;

};

//
// SoundEvent: Base class for event configuration
//

class SoundEvent {

public:
	inline SoundEvent( ) { }
	inline SoundEvent( CString const& name ): Name( name ) { }
	inline virtual ~SoundEvent( ) { }

	inline virtual bool PlaySound( void ) {
		return false;
	}

	CString Name;

};

//
// NoSoundEvent: Silent event.
//

class NoSoundEvent: public SoundEvent {

public:
	inline NoSoundEvent( ): SoundEvent( ) { }
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
	inline ToneSoundEvent( CString const& name, unsigned const frequency, unsigned const duration ): SoundEvent( name ), Frequency( frequency ), Duration( duration ) { }
	inline virtual ~ToneSoundEvent( ) { }

	inline virtual bool PlaySound( void ) {
		return false;
	}

	unsigned Frequency;
	unsigned Duration;

};
