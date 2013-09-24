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


#define ENUMERATOR_RELEASE_VIRT_NUM   2

typedef ULONG(WINAPI *EnumeratorReleaseFunc_t)(IMMDeviceEnumerator* pThis);
static EnumeratorReleaseFunc_t EnumeratorReleaseNext=NULL;
static int st_EnumeratorReleaseDetoured=0;

ULONG WINAPI EnumeratorReleaseCallBack(IMMDeviceEnumerator* pThis)
{
	ULONG uret;

	uret = EnumeratorReleaseNext(pThis);
	return uret;
}

typedef HRESULT(WINAPI *EnumeratorEnumAudioEndpointsFunc_t)(IMMDeviceEnumerator* pThis,EDataFlow dataFlow,DWORD dwStateMask,IMMDeviceCollection **ppDevices);
static EnumeratorEnumAudioEndpointsFunc_t EnumeratorEnumAudioEndpointsNext=NULL;
static int st_EnumeratorEnumAudioEndpointsDetoured=0;



typedef HRESULT(WINAPI *EnumeratorGetDeviceFunc_t)(IMMDeviceEnumerator* pThis,LPCWSTR pwstrId,IMMDevice **ppDevice);
static EnumeratorGetDeviceFunc_t EnumeratorGetDeviceNext=NULL;
static int st_EnumeratorGetDeviceDetoured=0;



static WAVEFORMATEX *st_pFormatEx=NULL;
static int st_PcmCapInited=0;

static int SetFormat(WAVEFORMATEX* pFormatEx)
{
	int ret=0;
	int formatsize=0;
	WAVEFORMATEX *pCopied=NULL;

	if (pFormatEx)
	{
		formatsize = sizeof(*pFormatEx);
		formatsize += pFormatEx->cbSize;
		
		pCopied = (WAVEFORMATEX*)malloc(formatsize);
		if (pCopied==NULL)
		{
			ret = LAST_ERROR_CODE();
			goto out;
		}
		
		memcpy(pCopied,pFormatEx,formatsize);
	}
	EnterCriticalSection(&st_StateCS);
	if (st_pFormatEx)
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

	if (st_pFormatEx)
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


typedef unsigned long ptr_type_t;
static int DetourVirtualFuncTable(CRITICAL_SECTION* pCS,int* pChanged,void**ppNextFunc,void*pCallBackFunc,void* pObject,int virtfuncnum)
{
	int ret =0;
	EnterCriticalSection(pCS);
	if (pChanged && *pChanged == 0)
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
			DetourVirtualFuncTable(&st_DetourCS,&st_EnumeratorReleaseDetoured,(void**)&EnumeratorGetDeviceNext,EnumeratorReleaseCallBack,*ppv,ENUMERATOR_RELEASE_VIRT_NUM);
        }

    }
    return hr;
}





void PcmCapInjectFini(void)
{
	if (st_PcmCapInited)
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
	if (ret < 0)
	{
		return ret;
	}
	
	st_PcmCapInited = 1;
	DEBUG_INFO("Init PcmCapInject succ\n");
	return 0;
}

