#pragma once
#include "resource.h"
#include "FcApi_i.h"

using namespace ATL;

class ATL_NO_VTABLE CFreeComposeController:
	public CComObjectRootEx< CComMultiThreadModel >,
	public CComCoClass< CFreeComposeController, &CLSID_FreeComposeController >,
	public IDispatchImpl< IFreeComposeController, &IID_IFreeComposeController, &LIBID_FcApiLib, 1, 0 >
{
	BEGIN_COM_MAP( CFreeComposeController )
		COM_INTERFACE_ENTRY( IFreeComposeController )
		COM_INTERFACE_ENTRY( IDispatch )
	END_COM_MAP( )
	DECLARE_REGISTRY_RESOURCEID( IDR_FREECOMPOSECONTROLLER )
	DECLARE_PROTECT_FINAL_CONSTRUCT( )

public:
	CFreeComposeController( );

	HRESULT FinalConstruct( );
	void FinalRelease( );
};

OBJECT_ENTRY_AUTO( __uuidof(FreeComposeController), CFreeComposeController )
