#include "stdafx.h"
#include "FCShared.h"
#include "Internal.h"

#include <ShObjIdl.h>
#include <KnownFolders.h>

#ifdef _DEBUG
#	ifndef DBG_NEW
#		define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#		define new DBG_NEW
#	endif
#endif

bool _GetAppDataFolderFromKfm( LPWSTR& lpsz ) {
	IKnownFolderManager* pkfm = nullptr;
	IKnownFolder* pkf = nullptr;
	bool ret = false;
	HRESULT hr;

	hr = CoCreateInstance( CLSID_KnownFolderManager, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pkfm ) );
	if ( FAILED(hr) ) {
		debug( L"_GetAppDataFolderFromKfm: CoCreateInstance(CLSID_KnownFolderManager) failed: 0x%08x\n", hr );
		goto out1;
	}

	hr = pkfm->GetFolder( FOLDERID_RoamingAppData, &pkf );
	if ( FAILED( hr ) ) {
		debug( L"_GetAppDataFolderFromKfm: pkfm->GetFolder(FOLDERID_RoamingAppData) failed: 0x%08x\n", hr );
		goto out2;
	}

	hr = pkf->GetPath( 0, &lpsz );
	if ( FAILED( hr ) ) {
		debug( L"_GetAppDataFolderFromKfm: pkf->GetPath failed: 0x%08x\n", hr );
		goto out3;
	}

	ret = true;

out3:	pkf->Release( );
out2:	pkfm->Release( );
out1:	return ret;
}
