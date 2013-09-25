// pcmcapinject.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "pcmcapinject.h"
#include <assert.h>
#include <Mmreg.h>
#include <stdlib.h>
#include "../common/output_debug.h"
#include "../common/detours.h"
#include <MMDeviceApi.h>

#pragma comment(lib,"common.lib")

#define LAST_ERROR_CODE() (GetLastError() ? GetLastError() : 1)

static CRITICAL_SECTION st_DetourCS;
static CRITICAL_SECTION st_StateCS;


/*****************************************************
* detour function call table
*****************************************************/
typedef unsigned long ptr_type_t;
static int DetourVirtualFuncTable(CRITICAL_SECTION* pCS,int* pChanged,void**ppNextFunc,void*pCallBackFunc,void* pObject,int virtfuncnum)
{
    int ret =0;
    EnterCriticalSection(pCS);
    if(pChanged && *pChanged == 0)
    {
        /*now to make sure */
        ptr_type_t** vptrptr = (ptr_type_t **)pObject;
        ptr_type_t* vptr = *vptrptr;

        assert(ppNextFunc && *ppNextFunc == NULL);
        *ppNextFunc =(void*) vptr[virtfuncnum];
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(ppNextFunc,pCallBackFunc);
        DetourTransactionCommit();

        *pChanged = 1;
        ret = 1;
    }
    LeaveCriticalSection(pCS);
    return ret;
}




/*****************************************************
* device collection handler
*****************************************************/
#define DEVICE_COLLECTION_RELEASE_NUM            2
#define DEVICE_COLLECTION_ITEM_NUM               4


typedef ULONG(WINAPI *DeviceCollectionReleaseFunc_t)(IMMDeviceCollection* pThis);
static DeviceCollectionReleaseFunc_t DeviceCollectionReleaseNext=NULL;
static int st_DeviceCollectionReleaseDetoured=0;

ULONG WINAPI DeviceCollectionReleaseCallBack(IMMDeviceCollection* pThis)
{
    ULONG uret;

    uret = DeviceCollectionReleaseNext(pThis);
    return uret;
}

typedef HRESULT(WINAPI *DeviceCollectionItemFunc_t)(IMMDeviceCollection* pThis,UINT nDevice,IMMDevice **ppDevice);
static DeviceCollectionItemFunc_t DeviceCollectionItemNext=NULL;
static int st_DeviceCollectionItemDetoured=0;

HRESULT WINAPI DeviceCollectionItemCallBack(IMMDeviceCollection* pThis,UINT nDevice,IMMDevice **ppDevice)
{
    HRESULT hr;
    hr = DeviceCollectionItemNext(pThis,nDevice,ppDevice);
    return hr;
}


/*****************************************************
* device enumerator hook functions
*****************************************************/
#define ENUMERATOR_RELEASE_NUM                    2
#define ENUMERATOR_ENUM_AUDIO_ENDPOINTS_NUM       3
#define ENUMERATOR_GET_DEFAULT_AUDIO_ENDPOINT_NUM 4
#define ENUMERATOR_GET_DEVICE_NUM                 5

typedef ULONG(WINAPI *EnumeratorReleaseFunc_t)(IMMDeviceEnumerator* pThis);
static EnumeratorReleaseFunc_t EnumeratorReleaseNext=NULL;
static int st_EnumeratorReleaseDetoured=0;

static int DetourDeviceCollectionVirtFunctions(IMMDeviceCollection* pCollection)
{
	DetourVirtualFuncTable(&st_DetourCS,&st_DeviceCollectionReleaseDetoured,(void**)&DeviceCollectionReleaseNext,DeviceCollectionReleaseCallBack,pCollection,DEVICE_COLLECTION_RELEASE_NUM);
	DetourVirtualFuncTable(&st_DetourCS,&st_DeviceCollectionItemDetoured,(void**)&DeviceCollectionItemNext,DeviceCollectionItemCallBack,pCollection,DEVICE_COLLECTION_ITEM_NUM);
	return 0;
}

ULONG WINAPI EnumeratorReleaseCallBack(IMMDeviceEnumerator* pThis)
{
    ULONG uret;

    uret = EnumeratorReleaseNext(pThis);
    return uret;
}

typedef HRESULT(WINAPI *EnumeratorEnumAudioEndpointsFunc_t)(IMMDeviceEnumerator* pThis,EDataFlow dataFlow,DWORD dwStateMask,IMMDeviceCollection **ppDevices);
static EnumeratorEnumAudioEndpointsFunc_t EnumeratorEnumAudioEndpointsNext=NULL;
static int st_EnumeratorEnumAudioEndpointsDetoured=0;

HRESULT WINAPI EnumeratorEnumAudioEndpointsCallBack(IMMDeviceEnumerator* pThis,EDataFlow dataFlow,DWORD dwStateMask,IMMDeviceCollection **ppDevices)
{
    HRESULT hr;

    hr = EnumeratorEnumAudioEndpointsNext(pThis,dataFlow,dwStateMask,ppDevices);
    if(SUCCEEDED(hr))
    {
        /*now success ,so we should to detour for the function of device collection*/
		DetourDeviceCollectionVirtFunctions(*ppDevices);
    }
    return hr;
}


typedef HRESULT(WINAPI *EnumeratorGetDefaultAudioEndpointFunc_t)(IMMDeviceEnumerator* pThis,EDataFlow dataFlow,ERole role,IMMDevice **ppEndpoint);
static EnumeratorGetDefaultAudioEndpointFunc_t EnumeratorGetDefaultAudioEndpointNext=NULL;
static int st_EnumeratorGetDefaultAudioEndpointDetoured=0;

HRESULT WINAPI EnumeratorGetDefaultAudioEndpointCallBack(IMMDeviceEnumerator* pThis,EDataFlow dataFlow,ERole role,IMMDevice **ppEndpoint)
{
    HRESULT hr;

    hr = EnumeratorGetDefaultAudioEndpointNext(pThis,dataFlow,role,ppEndpoint);
    return hr;
}

typedef HRESULT(WINAPI *EnumeratorGetDeviceFunc_t)(IMMDeviceEnumerator* pThis,LPCWSTR pwstrId,IMMDevice **ppDevice);
static EnumeratorGetDeviceFunc_t EnumeratorGetDeviceNext=NULL;
static int st_EnumeratorGetDeviceDetoured=0;

HRESULT EnumeratorGetDeviceCallBack(IMMDeviceEnumerator* pThis,LPCWSTR pwstrId,IMMDevice **ppDevice)
{
    HRESULT hr;
    hr = EnumeratorGetDeviceNext(pThis,pwstrId,ppDevice);
    return hr;
}


/*****************************************************
*  format set and get handler
*****************************************************/
static WAVEFORMATEX *st_pFormatEx=NULL;
static int st_PcmCapInited=0;

static int SetFormat(WAVEFORMATEX* pFormatEx)
{
    int ret=0;
    int formatsize=0;
    WAVEFORMATEX *pCopied=NULL;

    if(pFormatEx)
    {
        formatsize = sizeof(*pFormatEx);
        formatsize += pFormatEx->cbSize;

        pCopied = (WAVEFORMATEX*)malloc(formatsize);
        if(pCopied==NULL)
        {
            ret = LAST_ERROR_CODE();
            goto out;
        }

        memcpy(pCopied,pFormatEx,formatsize);
    }
    EnterCriticalSection(&st_StateCS);
    if(st_pFormatEx)
    {
        free(st_pFormatEx);
    }
    st_pFormatEx = NULL;
    st_pFormatEx = pCopied;
    LeaveCriticalSection(&st_StateCS);
out:
    return ret > 0 ? -ret : 0;
}


static int GetFormat(int* pFormat,int* pChannels,int*pSampleRate,int* pBitsPerSample)
{
    int ret = 0;
    EnterCriticalSection(&st_StateCS);

    if(st_pFormatEx)
    {
        *pFormat = st_pFormatEx->wFormatTag;
        *pChannels = st_pFormatEx->nChannels;
        *pSampleRate = st_pFormatEx->nSamplesPerSec;
        *pBitsPerSample = st_pFormatEx->wBitsPerSample;
        ret = 1;
    }
    LeaveCriticalSection(&st_StateCS);
    return ret;
}




/*now we should handle the thread*/
int PcmCapInject_SetAudioOperation(int iOperation)
{
    return 0;
}

int PcmCapInject_SetAudioState(int iState)
{
    return 0;
}




static int DetourEnumeratorVirtFunctions(IMMDeviceEnumerator* pEnumerator)
{
	DetourVirtualFuncTable(&st_DetourCS,&st_EnumeratorReleaseDetoured,(void**)&EnumeratorReleaseNext,EnumeratorReleaseCallBack,pEnumerator,ENUMERATOR_RELEASE_NUM);
	DetourVirtualFuncTable(&st_DetourCS,&st_EnumeratorEnumAudioEndpointsDetoured,(void**)&EnumeratorEnumAudioEndpointsNext,EnumeratorEnumAudioEndpointsCallBack,pEnumerator,ENUMERATOR_ENUM_AUDIO_ENDPOINTS_NUM);
	DetourVirtualFuncTable(&st_DetourCS,&st_EnumeratorGetDefaultAudioEndpointDetoured,(void**)&EnumeratorGetDefaultAudioEndpointNext,EnumeratorGetDefaultAudioEndpointCallBack,pEnumerator,ENUMERATOR_GET_DEFAULT_AUDIO_ENDPOINT_NUM);
	DetourVirtualFuncTable(&st_DetourCS,&st_EnumeratorGetDeviceDetoured,(void**)&EnumeratorGetDeviceNext,EnumeratorGetDeviceCallBack,pEnumerator,ENUMERATOR_GET_DEVICE_NUM);
	return 0;
}

static  HRESULT(WINAPI *CoCreateInstanceNext)(
    REFCLSID rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext,
    REFIID riid,
    LPVOID *ppv
) = CoCreateInstance;

HRESULT WINAPI  CoCreateInstanceCallBack(
    REFCLSID rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext,
    REFIID riid,
    LPVOID *ppv
)
{
    HRESULT hr;
    hr = CoCreateInstanceNext(rclsid,
                              pUnkOuter,dwClsContext,riid,ppv);
    if(SUCCEEDED(hr))
    {
        if(rclsid == __uuidof(MMDeviceEnumerator))
        {
            IMMDeviceEnumerator* pEnumerator = (IMMDeviceEnumerator*)(*ppv);
            /*now we should change function*/
			DetourEnumeratorVirtFunctions(pEnumerator);
        }

    }
    return hr;
}





void PcmCapInjectFini(void)
{
    if(st_PcmCapInited)
    {
        ;
    }
    return;
}

static int DetourPCMCapFunctions(void)
{

    return 0;
}

int PcmCapInjectInit(void)
{
    int ret;
    InitializeCriticalSection(&st_StateCS);
    InitializeCriticalSection(&st_DetourCS);

    ret = DetourPCMCapFunctions();
    if(ret < 0)
    {
        return ret;
    }

    st_PcmCapInited = 1;
    DEBUG_INFO("Init PcmCapInject succ\n");
    return 0;
}

