#pragma once

class SoundScheme;
class SoundEvent;



class SoundOptions {

public:
	inline SoundOptions( ) { }
	inline ~SoundOptions( ) { }

	std::vector<SoundScheme> Schemes;

};



class SoundScheme {

public:
	inline SoundScheme( ) { }
	inline SoundScheme( CString const& id, CString const& name ): ID( id ), Name( name ) { }
	inline ~SoundScheme( ) { }

	CString ID;
	CString Name;

	std::map<CString, SoundEvent> Events;

};



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

class NoSoundEvent: public SoundEvent {

public:
	inline NoSoundEvent( ): SoundEvent( ) { }
	inline NoSoundEvent( CString const& name ): SoundEvent( name ) { }
	inline virtual ~NoSoundEvent( ) { }

	inline virtual bool PlaySound( void ) {
		return true;
	}

};

class ApplicationSoundEvent: public SoundEvent {

public:
	inline ApplicationSoundEvent( ): SoundEvent( ) { }
	inline ApplicationSoundEvent( CString const& name ): SoundEvent( name ) { }
	inline virtual ~ApplicationSoundEvent( ) { }

	inline virtual bool PlaySound( void ) {
		return false;
	}

};

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
