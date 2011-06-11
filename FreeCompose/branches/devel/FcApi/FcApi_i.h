

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Jun 11 13:40:53 2011
 */
/* Compiler settings for FcApi.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __FcApi_i_h__
#define __FcApi_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFreeComposeController_FWD_DEFINED__
#define __IFreeComposeController_FWD_DEFINED__
typedef interface IFreeComposeController IFreeComposeController;
#endif 	/* __IFreeComposeController_FWD_DEFINED__ */


#ifndef __FreeComposeController_FWD_DEFINED__
#define __FreeComposeController_FWD_DEFINED__

#ifdef __cplusplus
typedef class FreeComposeController FreeComposeController;
#else
typedef struct FreeComposeController FreeComposeController;
#endif /* __cplusplus */

#endif 	/* __FreeComposeController_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IFreeComposeController_INTERFACE_DEFINED__
#define __IFreeComposeController_INTERFACE_DEFINED__

/* interface IFreeComposeController */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IFreeComposeController;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9C56F46A-B80A-4FC9-AF98-3182C7CFFDF5")
    IFreeComposeController : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IFreeComposeControllerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFreeComposeController * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFreeComposeController * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFreeComposeController * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFreeComposeController * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFreeComposeController * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFreeComposeController * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFreeComposeController * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IFreeComposeControllerVtbl;

    interface IFreeComposeController
    {
        CONST_VTBL struct IFreeComposeControllerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFreeComposeController_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFreeComposeController_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFreeComposeController_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFreeComposeController_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFreeComposeController_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFreeComposeController_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFreeComposeController_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFreeComposeController_INTERFACE_DEFINED__ */



#ifndef __FcApiLib_LIBRARY_DEFINED__
#define __FcApiLib_LIBRARY_DEFINED__

/* library FcApiLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_FcApiLib;

EXTERN_C const CLSID CLSID_FreeComposeController;

#ifdef __cplusplus

class DECLSPEC_UUID("A0509518-58D8-4D74-9CF4-5BB942FBC542")
FreeComposeController;
#endif
#endif /* __FcApiLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


