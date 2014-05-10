#pragma once

#ifndef __cplusplus
#error Sorry, you must compile FCHookDll.h as C++.
#endif

#ifdef FCHOOKDLL_EXPORTS
#define FCHOOKDLL_API __declspec(dllexport)
#else
#define FCHOOKDLL_API __declspec(dllimport)
#endif

//==============================================================================
// Types
//==============================================================================

enum CAPS_LOCK_TOGGLE_MODE {
	CLTM_NORMAL     = 1,
	CLTM_PRESSTWICE,
	CLTM_DISABLED,
};

enum CAPS_LOCK_SWAP_MODE {
	CLSM_NORMAL  = 1,
	CLSM_SWAP,
	CLSM_REPLACE,
};

//==============================================================================
// Classes
//==============================================================================

class FCHOOKDLL_API ComposeSequence {
public:

	inline ComposeSequence( ):
		Disabled        ( false ),
		CaseInsensitive ( false ),
		Reversible      ( false )
	{

	}

	ComposeSequence( CString& sequence, CString& result ):
		Sequence        ( sequence ),
		Result          ( result ),
		Disabled        ( false ),
		CaseInsensitive ( false ),
		Reversible      ( false )
	{

	}

	ComposeSequence( CString& group, CString& sequence, CString& result ):
		Group           ( group ),
		Sequence        ( sequence ),
		Result          ( result ),
		Disabled        ( false ),
		CaseInsensitive ( false ),
		Reversible      ( false )
	{

	}

	ComposeSequence( CString& sequence, CString& result, bool const disabled, bool const caseInsensitive, bool const reversible ):
		Sequence        ( sequence ),
		Result          ( result ),
		Disabled        ( disabled ),
		CaseInsensitive ( caseInsensitive ),
		Reversible      ( reversible )
	{

	}

	ComposeSequence( CString& group, CString& sequence, CString& result, bool const disabled, bool const caseInsensitive, bool const reversible ):
		Group           ( group ),
		Sequence        ( sequence ),
		Result          ( result ),
		Disabled        ( disabled ),
		CaseInsensitive ( caseInsensitive ),
		Reversible      ( reversible )
	{

	}

	inline bool operator==( ComposeSequence const& b ) const {
		return
			(
			(    Group.Compare( b.Group    ) == 0 )  &&
			( Sequence.Compare( b.Sequence ) == 0 )  &&
			(   Result.Compare( b.Result   ) == 0 )  &&
			(        Disabled == b.Disabled        ) &&
			( CaseInsensitive == b.CaseInsensitive ) &&
			(      Reversible == b.Reversible      )
			);
	}

	inline bool operator!=( ComposeSequence const& b ) const {
		return
			(
			(    Group.Compare( b.Group    ) != 0 )  ||
			( Sequence.Compare( b.Sequence ) != 0 )  ||
			(   Result.Compare( b.Result   ) != 0 )  ||
			(        Disabled != b.Disabled        ) ||
			( CaseInsensitive != b.CaseInsensitive ) ||
			(      Reversible != b.Reversible      )
			);
	}

	CString Group;

	CString Sequence;
	CString Result;

	bool Disabled;
	bool CaseInsensitive;
	bool Reversible;
};

//==============================================================================
// Constants
//==============================================================================

DWORD const FCHOOKDLL_API_VERSION = 0x0190;

WPARAM const PIP_OK_1  = 1;
WPARAM const PIP_OK_2  = 2;
WPARAM const PIP_OK_3  = 3;
WPARAM const PIP_ABORT = 4;
WPARAM const PIP_ERROR = 5;
WPARAM const PIP_FAIL  = 6;

int const MAXIMUM_SEQUENCE_LENGTH = 16;

//==============================================================================
// Function prototypes
//==============================================================================

//
// Suite 0: API version
//

FCHOOKDLL_API DWORD FcGetApiVersion( void );

//
// Suite 1: Lifetime management
//

FCHOOKDLL_API BOOL FcInitialize( void );
FCHOOKDLL_API BOOL FcUninitialize( void );

//
// Suite 2: Hook control
//

FCHOOKDLL_API BOOL FcEnableHook( void );
FCHOOKDLL_API BOOL FcDisableHook( void );
FCHOOKDLL_API BOOL FcIsHookEnabled( void );

FCHOOKDLL_API BOOL FcSetNotifyWindowHandle( HWND hwndNotifyWindow );
FCHOOKDLL_API HWND FcGetNotifyWindowHandle( void );

//
// Suite 3: Compose key and sequences
//

FCHOOKDLL_API BOOL FcSetComposeKey( DWORD vkCompose );
FCHOOKDLL_API DWORD FcGetComposeKey( void );

FCHOOKDLL_API BOOL FcSetComposeSequences( ComposeSequence* pSequences, DWORD cSequences );

//
// Suite 4: Caps Lock
//

FCHOOKDLL_API BOOL FcSetSwapCapsLockKey( DWORD vkCapsLockSwap );
FCHOOKDLL_API DWORD FcGetSwapCapsLockKey( void );

FCHOOKDLL_API BOOL FcSetCapsLockToggleMode( CAPS_LOCK_TOGGLE_MODE mode );
FCHOOKDLL_API CAPS_LOCK_TOGGLE_MODE FcGetCapsLockToggleMode( void );

FCHOOKDLL_API BOOL FcSetCapsLockSwapMode( CAPS_LOCK_SWAP_MODE mode );
FCHOOKDLL_API CAPS_LOCK_SWAP_MODE FcGetCapsLockSwapMode( void );
