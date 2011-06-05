#define THIS_FILE_TARGETS_VISTA
#include "stdafx.h"

#include <ShlObj.h>
#include <ShObjIdl.h>
#include <KnownFolders.h>

#include "Utils.h"

#ifndef NDEBUG
bool _GetAppDataFolderFromKfm( CString& str ) {
	IKnownFolderManager* pkfm = NULL;
	IKnownFolder* pkf = NULL;
	LPWSTR path = NULL;
	bool ret = false;
	HRESULT hr;

	hr = CoCreateInstance( CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pkfm ) );
	if ( FAILED(hr) ) {
		debug( L"_GetAppDataFolderFromKfm: CoCreateInstance(CLSID_KnownFolderManager) failed: 0x%08x\n", hr );
		goto out1;
	}

	hr = pkfm->GetFolder( FOLDERID_RoamingAppData, &pkf );
	if ( FAILED( hr ) ) {
		debug( L"_GetAppDataFolderFromKfm: pkfm->GetFolder(FOLDERID_RoamingAppData) failed: 0x%08x\n", hr );
		goto out2;
	}

	hr = pkf->GetPath( 0, &path );
	if ( FAILED( hr ) ) {
		debug( L"_GetAppDataFolderFromKfm: pkf->GetPath failed: 0x%08x\n", hr );
		goto out3;
	}

	str = path;
	CoTaskMemFree( path );

	ret = true;

out3:	pkf->Release( );
out2:	pkfm->Release( );
out1:	return ret;
}
#endif
