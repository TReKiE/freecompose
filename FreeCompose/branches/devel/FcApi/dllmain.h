#pragma once

class CFcApiModule: public ATL::CAtlDllModuleT< CFcApiModule > {
public:
	DECLARE_LIBID( LIBID_FcApiLib )
	DECLARE_REGISTRY_APPID_RESOURCEID( IDR_FCAPI, "{F76B3E97-2B8F-4202-86DE-C71F5980C6F0}" )
};

extern class CFcApiModule _AtlModule;
