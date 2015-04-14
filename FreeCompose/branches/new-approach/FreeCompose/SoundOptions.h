#pragma once

#ifndef _NOISY_DEBUG
#	pragma push_macro( "noisydebug" )
#	undef  noisydebug
#	define noisydebug debug
#endif

//
// Forward declarations
//

class SoundScheme;
class SoundEvent;

//
// Types
//

DECLARE_INDEXABLE_ENUM( SoundEventType ) {
	none             = -1,
	NoSound          =  0,
	ApplicationSound,
	ToneSound,
	max
};

//
// SoundOptions: Root of sound config classes
//

using SoundSchemeVector = std::vector<SoundScheme>;

class SoundOptions {

public:
	inline SoundOptions( ) {
		noisydebug( L"0x%p SoundOptions::`ctor/1\n", this );
	}

	inline SoundOptions( SoundOptions const& rhs ) {
		noisydebug( L"0x%p SoundOptions::`ctor/2\n", this );
		operator=( static_cast<SoundOptions const&>( rhs ) );
	}

	inline SoundOptions( SoundOptions&& rhs ) {
		noisydebug( L"0x%p SoundOptions::`ctor/3\n", this );
		operator=( static_cast<SoundOptions&&>( rhs ) );
	}

	inline ~SoundOptions( ) {
		noisydebug( L"0x%p SoundOptions::`dtor\n", this );
	}

	inline SoundOptions& operator=( SoundOptions const& rhs ) {
		Schemes.clear( );
		for ( auto& scheme : rhs.Schemes ) {
			Schemes.push_back( scheme );
		}
		return *this;
	}

	inline SoundOptions& operator=( SoundOptions&& rhs ) {
		*this = const_cast<SoundOptions const&>( rhs );
		rhs.Schemes.clear( );
		return *this;
	}

	inline bool operator==( SoundOptions const& rhs ) const;

	inline bool operator!=( SoundOptions const& rhs ) const {
		return !( *this == rhs );
	}

	inline SoundScheme* GetSoundScheme( CString const& name ) const;

	SoundSchemeVector Schemes;

};

//
// SoundScheme: contains a collection of event configuration
//

using SoundEventMap = std::map<CString, SoundEvent*>;
using SoundEventMapPair = std::pair<CString, SoundEvent*>;

class SoundScheme {

public:
	inline SoundScheme( ) {
		noisydebug( L"0x%p SoundScheme::`ctor/1\n", this );
	}

	inline SoundScheme( SoundScheme const& rhs ) {
		noisydebug( L"0x%p SoundScheme::`ctor/2\n", this );
		operator=( static_cast<SoundScheme const&>(rhs) );
	}

	inline SoundScheme( SoundScheme&& rhs ) {
		noisydebug( L"0x%p SoundScheme::`ctor/3\n", this );
		operator=( static_cast<SoundScheme&&>(rhs) );
	}

	inline SoundScheme( CString const& id, CString const& name ): ID( id ), Name( name ) {
		noisydebug( L"0x%p SoundScheme::`ctor/4\n", this );
	}

	inline ~SoundScheme( );

	inline SoundScheme& operator=( SoundScheme const& rhs ) {
		ID = rhs.ID;
		Name = rhs.Name;
		for ( auto& pair : rhs.Events ) {
			Events.insert( SoundEventMapPair( pair.first, pair.second ) );
		}
		return *this;
	}

	inline SoundScheme& operator=( SoundScheme&& rhs ) {
		*this = const_cast<SoundScheme const&>( rhs );
		rhs.ID.Empty( );
		rhs.Name.Empty( );
		rhs.Events.clear( );
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
inline SoundScheme* SoundOptions::GetSoundScheme( CString const& name ) const {
	for ( auto& s : Schemes ) {
		if ( 0 == s.Name.CompareNoCase( name ) ) {
			return const_cast<SoundScheme*>( &s );
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
	inline SoundEvent( ) {
		noisydebug( L"0x%p SoundEvent::`ctor/1\n", this );
	}

	inline SoundEvent( SoundEvent const& rhs ) {
		noisydebug( L"0x%p SoundEvent::`ctor/2\n", this );
		operator=( static_cast<SoundEvent const&>( rhs ) );
	}

	inline SoundEvent( SoundEvent&& rhs ) {
		noisydebug( L"0x%p SoundEvent::`ctor/3\n", this );
		operator=( static_cast<SoundEvent&&>( rhs ) );
	}

	inline SoundEvent( CString const& name ): Name( name ) {
		noisydebug( L"0x%p SoundEvent::`ctor/4\n", this );
	}

	inline virtual ~SoundEvent( ) {
		noisydebug( L"0x%p SoundEvent::`dtor\n", this );
	}

	inline virtual SoundEvent& operator=( SoundEvent const& rhs ) {
		Name = rhs.Name;
		return *this;
	}

	inline virtual SoundEvent& operator=( SoundEvent&& rhs ) {
		Name = rhs.Name;
		rhs.Name.Empty( );
		return *this;
	}

	inline virtual bool operator==( SoundEvent const& rhs ) const {
		return ( 0 == Name.Compare( rhs.Name ) );
	}

	inline virtual bool operator!=( SoundEvent const& rhs ) const {
		return !( *this == rhs );
	}

	inline virtual SoundEventType GetType( void ) const {
		return SoundEventType::none;
	}

	inline virtual bool PlaySound( void ) {
		return false;
	}

	CString Name;

};

// The following method is defined here because class SoundEvent is incomplete until this point.
inline SoundScheme::~SoundScheme( ) {
	noisydebug( L"0x%p SoundScheme::`dtor\n", this );
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
	inline NoSoundEvent( ): SoundEvent( ) {
		noisydebug( L"0x%p NoSoundEvent::`ctor/1\n", this );
	}

	inline NoSoundEvent( NoSoundEvent const& rhs ) {
		noisydebug( L"0x%p NoSoundEvent::`ctor/2\n", this );
		operator=( static_cast<NoSoundEvent const&>( rhs ) );
	}

	inline NoSoundEvent( NoSoundEvent&& rhs ) {
		noisydebug( L"0x%p NoSoundEvent::`ctor/3\n", this );
		operator=( static_cast<NoSoundEvent&&>( rhs ) );
	}

	inline NoSoundEvent( CString const& name ): SoundEvent( name ) {
		noisydebug( L"0x%p NoSoundEvent::`ctor/4\n", this );
	}

	inline virtual ~NoSoundEvent( ) {
		noisydebug( L"0x%p NoSoundEvent::`dtor\n", this );
	}

	inline virtual NoSoundEvent& operator=( NoSoundEvent const& rhs ) {
		Name = rhs.Name;
		return *this;
	}

	inline virtual NoSoundEvent& operator=( NoSoundEvent&& rhs ) {
		Name = rhs.Name;
		rhs.Name.Empty( );
		return *this;
	}

	inline virtual SoundEventType GetType( void ) const {
		return SoundEventType::NoSound;
	}

	inline virtual bool PlaySound( void ) {
		return true;
	}

};

//
// ApplicationSoundEvent: Play a WAV file configured in the "Sounds" control panel via PlaySound().
//

class ApplicationSoundEvent: public SoundEvent {

public:
	inline ApplicationSoundEvent( ): SoundEvent( ) {
		noisydebug( L"0x%p ApplicationSoundEvent::`ctor/1\n", this );
	}

	inline ApplicationSoundEvent( ApplicationSoundEvent const& rhs ) {
		noisydebug( L"0x%p ApplicationSoundEvent::`ctor/2\n", this );
		operator=( static_cast<ApplicationSoundEvent const&>( rhs ) );
	}

	inline ApplicationSoundEvent( ApplicationSoundEvent&& rhs ) {
		noisydebug( L"0x%p ApplicationSoundEvent::`ctor/3\n", this );
		operator=( static_cast<ApplicationSoundEvent&&>( rhs ) );
	}

	inline ApplicationSoundEvent( CString const& name ): SoundEvent( name ) {
		noisydebug( L"0x%p ApplicationSoundEvent::`ctor/4\n", this );
	}

	inline virtual ~ApplicationSoundEvent( ) {
		noisydebug( L"0x%p ApplicationSoundEvent::`dtor\n", this );
	}

	inline virtual ApplicationSoundEvent& operator=( ApplicationSoundEvent const& rhs ) {
		Name = rhs.Name;
		return *this;
	}

	inline virtual ApplicationSoundEvent& operator=( ApplicationSoundEvent&& rhs ) {
		Name = rhs.Name;
		rhs.Name.Empty( );
		return *this;
	}

	inline virtual SoundEventType GetType( void ) const {
		return SoundEventType::ApplicationSound;
	}

	inline virtual bool PlaySound( void ) {
		return false;
	}

};

//
// ToneSoundEvent: Play a specified frequency for a specified duration.
//

class ToneSoundEvent: public SoundEvent {

public:
	inline ToneSoundEvent( ): SoundEvent( ), Frequency( 0 ), Duration( 0 ) {
		noisydebug( L"0x%p ToneSoundEvent::`ctor/1\n", this );
	}

	inline ToneSoundEvent( ToneSoundEvent const& rhs ) {
		noisydebug( L"0x%p ToneSoundEvent::`ctor/2\n", this );
		operator=( static_cast<ToneSoundEvent const&>( rhs ) );
	}

	inline ToneSoundEvent( ToneSoundEvent&& rhs ) {
		noisydebug( L"0x%p ToneSoundEvent::`ctor/3\n", this );
		operator=( static_cast<ToneSoundEvent&&>( rhs ) );
	}

	inline ToneSoundEvent( CString const& name, unsigned const frequency, unsigned const duration ): SoundEvent( name ), Frequency( frequency ), Duration( duration ) {
		noisydebug( L"0x%p ToneSoundEvent::`ctor/4\n", this );
	}

	inline virtual ~ToneSoundEvent( ) {
		noisydebug( L"0x%p ToneSoundEvent::`dtor\n", this );
	}

	inline virtual ToneSoundEvent& operator=( ToneSoundEvent const& rhs ) {
		SoundEvent::operator=( rhs );
		Frequency = rhs.Frequency;
		Duration = rhs.Duration;
		return *this;
	}

	inline virtual ToneSoundEvent& operator=( ToneSoundEvent&& rhs ) {
		*this = const_cast<ToneSoundEvent const&>( rhs );
		rhs.Frequency = 0;
		rhs.Duration = 0;
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

	inline virtual SoundEventType GetType( void ) const {
		return SoundEventType::ToneSound;
	}

	inline virtual bool PlaySound( void ) {
		return false;
	}

	unsigned Frequency;
	unsigned Duration;

};

#pragma pop_macro( "noisydebug" )
