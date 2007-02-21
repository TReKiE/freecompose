#ifdef FCHOOKDLL_EXPORTS
#define FCHOOKDLL_API __declspec(dllexport)
#else
#define FCHOOKDLL_API __declspec(dllimport)
#endif

#define FCHOOKDLL_API_VERSION 0x0009

struct COMPOSE_KEY_ENTRY {
	DWORD vkFirst;
	DWORD vkSecond;
	wchar_t wchComposed;
};

FCHOOKDLL_API BOOL FcSetComposeKeyEntries( COMPOSE_KEY_ENTRY* rgEntries, DWORD cEntries );
FCHOOKDLL_API BOOL FcEnableHook( void );
FCHOOKDLL_API BOOL FcDisableHook( void );
FCHOOKDLL_API DWORD FcGetApiVersion( void );
