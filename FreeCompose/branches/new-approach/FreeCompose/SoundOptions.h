#pragma once

class SoundScheme;
class SoundEvent;


class SoundOptions {

public:
	std::vector<SoundScheme> Schemes;

};


class SoundScheme {

public:
	CString ID;
	CString Name;

	std::map<CString, SoundEvent> Events;

};


class SoundEvent {

public:
	inline SoundEvent( ) { }
	inline virtual ~SoundEvent( ) { }

	virtual bool PlaySound( void ) = 0;

	CString Name;

};

class NoSoundEvent: public SoundEvent {

public:
	inline NoSoundEvent( ) { }
	inline virtual ~NoSoundEvent( ) { }

	inline virtual bool PlaySound( void ) {
		return false;
	}

};

class ApplicationSoundEvent: public SoundEvent {

public:
	inline ApplicationSoundEvent( ) { }
	inline virtual ~ApplicationSoundEvent( ) { }

	inline virtual bool PlaySound( void ) {
		return false;
	}

};

class ToneSoundEvent: public SoundEvent {

public:
	inline ToneSoundEvent( ) { }
	inline virtual ~ToneSoundEvent( ) { }

	inline virtual bool PlaySound( void ) {
		return false;
	}

	unsigned Frequency;
	unsigned Duration;

};
