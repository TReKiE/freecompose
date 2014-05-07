#include "stdafx.h"

#include "AppSoundsRegistry.h"
#include "Utils.h"

static wchar_t* CompositionSoundNames[] = {
	L"FC_Composition_Starting",
	L"FC_Composition_KeyPressed",
	L"FC_Composition_Succeeded",
	L"FC_Composition_Failed",
	L"FC_Composition_Cancelled",
	L"FC_Composition_Escape",
};

static int CompositionDisplayNameIds[] = {
	IDS_APPSOUND_COMPOSITION_STARTING,
	IDS_APPSOUND_COMPOSITION_KEYPRESSED,
	IDS_APPSOUND_COMPOSITION_SUCCEEDED,
	IDS_APPSOUND_COMPOSITION_FAILED,
	IDS_APPSOUND_COMPOSITION_CANCELLED,
	IDS_APPSOUND_COMPOSITION_ESCAPE,
};

void CAppSoundsRegistry::CreateFcAppSoundsRegistry( void ) {
	LSTATUS ls;
	CString tmp;
	DWORD dwDisposition;

	wchar_t wzExeName[1024];
	GetModuleFileName( AfxGetInstanceHandle( ), wzExeName, 1024 );
	
	CRegKey AppEvents;
	ls = AppEvents.Open( HKEY_CURRENT_USER, L"AppEvents" );

	CRegKey EventLabels;
	ls = EventLabels.Open( AppEvents, L"EventLabels" );

	for ( int n = 0; n < _countof( CompositionSoundNames ); n++ ) {
		CRegKey key;
		tmp.Format( L"@%s,%d", wzExeName, -CompositionDisplayNameIds[n] );
		dwDisposition = 0;
		ls = key.Create( EventLabels, CompositionSoundNames[n], nullptr, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE, nullptr, &dwDisposition );
		if ( REG_CREATED_NEW_KEY == dwDisposition ) {
			ls = key.SetStringValue( nullptr, CompositionSoundNames[n], REG_SZ );
			ls = key.SetStringValue( L"DispFileName", tmp );
		}
		ls = key.Close( );
	}

	ls = EventLabels.Close( );

	CRegKey Schemes;
	ls = Schemes.Open( AppEvents, L"Schemes" );

	CRegKey Apps;
	ls = Apps.Open( Schemes, L"Apps" );

	CRegKey FreeCompose;
	tmp.Format( L"@%s,%d", wzExeName, -AFX_IDS_APP_TITLE );
	dwDisposition = 0;
	ls = FreeCompose.Create( Apps, L"FreeCompose", nullptr, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE, nullptr, &dwDisposition );
	if ( REG_CREATED_NEW_KEY == dwDisposition ) {
		ls = FreeCompose.SetStringValue( nullptr, LoadFromStringTable( AFX_IDS_APP_TITLE ) );
		ls = FreeCompose.SetStringValue( L"DispFileName", tmp );

		for ( int n = 0; n < _countof( CompositionSoundNames ); n++ ) {
			CRegKey soundkey;
			ls = soundkey.Create( FreeCompose, CompositionSoundNames[n] );
			ls = soundkey.SetStringValue( nullptr, CompositionSoundNames[n], REG_SZ );

			CRegKey dotdefaultkey;
			ls = dotdefaultkey.Create( soundkey, L".Default" );
			ls = dotdefaultkey.SetStringValue( nullptr, L"", REG_SZ );
			ls = dotdefaultkey.Close( );

			CRegKey dotnonekey;
			ls = dotnonekey.Create( soundkey, L".None" );
			ls = dotnonekey.SetStringValue( nullptr, L"", REG_SZ );
			ls = dotnonekey.Close( );

			ls = soundkey.Close( );
		}
	}

	ls = FreeCompose.Close( );
	ls = Apps.Close( );
	ls = Schemes.Close( );
	ls = AppEvents.Close( );
}
