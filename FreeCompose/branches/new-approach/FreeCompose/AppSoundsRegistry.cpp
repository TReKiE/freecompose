#include "stdafx.h"

#include "FreeCompose.h"
#include "AppSoundsRegistry.h"
#include "Utils.h"

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

static wchar_t const* ApplicationSoundDefaultSoundEventFileName[NumberOfApplicationSounds] = {
	/* Starting    */ L"C:/Windows/Media/Windows Notify Calendar.wav",
	/* KeyPressed  */ L"",
	/* Succeeded   */ L"",
	/* Failed      */ L"",
	/* Cancelled   */ L"",
	/* Escape      */ L"C:/Windows/Media/Windows Notify Email.wav",
	/* Activated   */ L"",
	/* Deactivated */ L"",
};

//
// Implementation
//

CString CFcAppSoundsRegistry::_GetCurrentSoundScheme( void ) {
	CString currentSoundScheme;
	LSTATUS ls;

	CRegKey schemeskey;
	ls = schemeskey.Open( HKEY_CURRENT_USER, L"AppEvents\\Schemes", KEY_READ );

	DWORD dwType = 0;
	ULONG cbValue = 0;
	ls = schemeskey.QueryValue( nullptr, &dwType, nullptr, &cbValue );
	if ( ERROR_SUCCESS == ls || ERROR_INSUFFICIENT_BUFFER == ls ) {
		cbValue += sizeof(wchar_t);
		wchar_t* pwz = static_cast<wchar_t*>( alloca( cbValue ) );
		ls = schemeskey.QueryStringValue( nullptr, pwz, &cbValue );
		if ( ERROR_SUCCESS == ls ) {
			debug( L"CFcAppSoundsRegistry::_RegisterApp: Current sound scheme is '%s'\n", pwz );
			currentSoundScheme = pwz;
		} else {
			debug( L"CFcAppSoundsRegistry::_RegisterApp: Couldn't query value of current sound scheme: error=%lu\n", ls );
		}
	} else {
		debug( L"CFcAppSoundsRegistry::_GetCurrentSoundScheme: Failed to query size of current sound scheme's name: error=%lu\n", ls );
	}

	ls = schemeskey.Close( );
	return currentSoundScheme;
}

void CFcAppSoundsRegistry::_RegisterEventLabels( wchar_t const* pwzExeName ) {
	LSTATUS ls;
	CRegKey EventLabels;
	CString tmp;
	DWORD dwDisposition;

	ls = EventLabels.Open( HKEY_CURRENT_USER, L"AppEvents\\EventLabels" );
	for ( int n = 0; n < NumberOfApplicationSounds; n++ ) {
		CRegKey key;
		tmp.Format( L"@%s,-%d", pwzExeName, ApplicationSoundDisplayNameIds[n] );
		dwDisposition = 0;
		ls = key.Create( EventLabels, ApplicationSoundNames[n], nullptr, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, nullptr, &dwDisposition );
		if ( ( ERROR_SUCCESS == ls ) && ( REG_CREATED_NEW_KEY == dwDisposition ) ) {
			ls = key.SetStringValue( nullptr, ApplicationSoundNames[n] );
			ls = key.SetStringValue( L"DispFileName", tmp );
		}
		ls = key.Close( );
	}

	ls = EventLabels.Close( );
}

void CFcAppSoundsRegistry::_RegisterApp( wchar_t const* pwzExeName ) {
	CRegKey FreeCompose;
	CString tmp;
	LSTATUS ls;
	DWORD dwDisposition;

	CString currentSoundScheme = _GetCurrentSoundScheme( );
	bool silentScheme = ( 0 == currentSoundScheme.CompareNoCase( L".None" ) );
	if ( silentScheme ) {
		debug( L"CFcAppSoundsRegistry::_RegisterApp: Silent sound scheme is in effect!\n" );
	}

	tmp.Format( L"@%s,%d", pwzExeName, -AFX_IDS_APP_TITLE );
	dwDisposition = 0;
	ls = FreeCompose.Create( HKEY_CURRENT_USER, L"AppEvents\\Schemes\\Apps\\FreeCompose", nullptr, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, nullptr, &dwDisposition );
	if ( ( ERROR_SUCCESS == ls ) && ( REG_CREATED_NEW_KEY == dwDisposition ) ) {
		ls = FreeCompose.SetStringValue( nullptr, LoadFromStringTable( AFX_IDS_APP_TITLE ) );
		ls = FreeCompose.SetStringValue( L"DispFileName", tmp );

		for ( int n = 0; n < NumberOfApplicationSounds; n++ ) {
			CRegKey soundkey;
			ls = soundkey.Create( FreeCompose, ApplicationSoundNames[n] );
			ls = soundkey.SetStringValue( nullptr, ApplicationSoundNames[n] );

			CRegKey dotdefaultkey;
			ls = dotdefaultkey.Create( soundkey, L".Default" );
			ls = dotdefaultkey.SetStringValue( nullptr, ApplicationSoundDefaultSoundEventFileName[n] );
			ls = dotdefaultkey.Close( );

			CRegKey dotnonekey;
			ls = dotnonekey.Create( soundkey, L".None" );
			ls = dotnonekey.SetStringValue( nullptr, L"" );
			ls = dotnonekey.Close( );

			CRegKey dotcurrentkey;
			ls = dotcurrentkey.Create( soundkey, L".Current" );
			ls = dotcurrentkey.SetStringValue( nullptr, silentScheme ? L"" : ApplicationSoundDefaultSoundEventFileName[n] );
			ls = dotcurrentkey.Close( );

			ls = soundkey.Close( );
		}
	}

	ls = FreeCompose.Close( );
}

void CFcAppSoundsRegistry::_UnregisterEventLabels( void ) {
	CRegKey EventLabels;
	LSTATUS ls = EventLabels.Open( HKEY_CURRENT_USER, L"AppEvents\\EventLabels" );
	if ( ERROR_SUCCESS != ls ) {
		debug( L"CFcAppSoundsRegistry::_UnregisterEventLabels: EventLabels.Open failed: %ld\n", ls );
		return;
	}

	for ( int n = 0; n < NumberOfApplicationSounds; n++ ) {
		ls = EventLabels.RecurseDeleteKey( ApplicationSoundNames[n] );
		debug( L"CFcAppSoundsRegistry::_UnregisterEventLabels: delete of '%s': %ld\n", ApplicationSoundNames[n], ls );
	}
	ls = EventLabels.Close( );
}

void CFcAppSoundsRegistry::_UnregisterApp( void ) {
	CRegKey Apps;
	LSTATUS ls = Apps.Open( HKEY_CURRENT_USER, L"AppEvents\\Schemes\\Apps" );
	if ( ERROR_SUCCESS != ls ) {
		debug( L"CFcAppSoundsRegistry::_UnregisterApp: open of HKCU\\AppEvents\\Schemes\\Apps failed: %ld\n", ls );
		return;
	}

	ls = Apps.RecurseDeleteKey( L"FreeCompose" );
	debug( L"CFcAppSoundsRegistry::_UnregisterApp: delete of 'FreeCompose': %ld\n", ls );
	ls = Apps.Close( );
}

//
// Interface
//

void CFcAppSoundsRegistry::Register( void ) {
	_RegisterEventLabels( _wpgmptr );
	_RegisterApp( _wpgmptr );
}

void CFcAppSoundsRegistry::Unregister( void ) {
	_UnregisterEventLabels( );
	_UnregisterApp( );
}
