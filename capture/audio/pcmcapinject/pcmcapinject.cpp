// pcmcapinject.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "pcmcapinject.h"
#include <assert.h>
#include <Mmreg.h>
#include <stdlib.h>
#include "../common/output_debug.h"
#include "../common/detours.h"
#include <audioclient.h>
#include <MMDeviceApi.h>

#pragma comment(lib,"common.lib")

#define LAST_ERROR_CODE() (GetLastError() ? GetLastError() : 1)




static CRITICAL_SECTION st_DetourCS;
static CRITICAL_SECTION st_StateCS;

static float st_Volume = 0.0;

/*****************************************************
*  to handle for the volume
*****************************************************/
static int SetVolume(float volume)
{
    int ret =0;
    EnterCriticalSection(&st_StateCS);
    st_Volume = volume;
    LeaveCriticalSection(&st_StateCS);
    return ret;
}

/*****************************************************
*  format set and get handler
*****************************************************/
static WAVEFORMATEX *st_pFormatEx=NULL;
static IAudioClient *st_pHandleAudioClient=NULL;
static IAudioRenderClient *st_pAudioRenderClient=NULL;
static int st_PcmCapInited=0;

static int SetFormat(WAVEFORMATEX* pFormatEx)
{
    int ret=0;
    int formatsize=0;
    WAVEFORMATEX *pCopied=NULL,*pFreed=NULL;

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
    pFreed = st_pFormatEx;
    st_pFormatEx = NULL;
    st_pFormatEx = pCopied;
    LeaveCriticalSection(&st_StateCS);
    if(pFreed)
    {
        free(pFreed);
    }
    pFreed = NULL;
out:
    return ret > 0 ? -ret : 0;
}


static int GetFormat(int* pFormat,int* pChannels,int*pSampleRate,int* pBitsPerSample,float* pVolume)
{
    int ret = 0;
    EnterCriticalSection(&st_StateCS);

    if(st_pFormatEx)
    {
        *pFormat = st_pFormatEx->wFormatTag;
        *pChannels = st_pFormatEx->nChannels;
        *pSampleRate = st_pFormatEx->nSamplesPerSec;
        *pBitsPerSample = st_pFormatEx->wBitsPerSample;
        *pVolume = st_Volume;
        ret = 1;
    }
    LeaveCriticalSection(&st_StateCS);
    return ret;
}


static int st_Operation=PCMCAPPER_OPERATION_RENDER;

static int SetOperation(int operation)
{
    int oldoperation;
    EnterCriticalSection(&st_StateCS);
    oldoperation = st_Operation;
    st_Operation = operation;
    LeaveCriticalSection(&st_StateCS);
    return oldoperation;
}

static int GetOperation()
{
    int oldoperation;
    EnterCriticalSection(&st_StateCS);
    oldoperation = st_Operation;
    LeaveCriticalSection(&st_StateCS);
    return oldoperation;
}

typedef struct
{
    HANDLE m_hNotifyEvent;
    int m_Error;
    int m_Idx;
    ptr_type_t m_BaseAddr;
    ptr_type_t m_Offset;
    int m_Size;
} EVENT_LIST_t;

static std::vector<EVENT_LIST_t*> st_FreeList;
static std::vector<EVENT_LIST_t*> st_ReleaseList;
static EVENT_LIST_t *st_pWholeList=NULL;
static int st_WholeListNum=0;
static int st_GetWholeListNum=0;

static HANDLE st_hStartNotifyEvent=NULL;
static HANDLE st_hStopNotifyEvent=NULL;
static unsigned char* st_pMemBase=NULL;
static HANDLE st_hMemMap=NULL;
static int st_MemMapSize=0;


static CRITICAL_SECTION st_ListCS;


static EVENT_LIST_t* GetFreeList()
{
    EVENT_LIST_t* pEventList=NULL;

    EnterCriticalSection(&st_ListCS);
    if(st_FreeList.size() > 0)
    {
        pEventList = st_FreeList[0];
        st_FreeList.erase(st_FreeList.begin());
        st_GetWholeListNum ++;
    }
    LeaveCriticalSection(&st_ListCS);
    return pEventList;
}

static int PutReleaseList(EVENT_LIST_t* pEventList)
{
    int ret=0;
    EnterCriticalSection(&st_ListCS);
    if(st_pWholeList)
    {
        st_ReleaseList.push_back(pEventList);
        assert(st_GetWholeListNum > 0);
        st_GetWholeListNum -- ;
        ret = 1;
    }
    LeaveCriticalSection(&st_ListCS);
    return ret;

}

static int PutFreeList(EVENT_LIST_t* pEventList)
{
    int ret=0;
    EnterCriticalSection(&st_ListCS);
    if(st_pWholeList)
    {
        st_FreeList.push_back(pEventList);
        assert(st_GetWholeListNum > 0);
        st_GetWholeListNum -- ;
        ret = 1;
    }
    LeaveCriticalSection(&st_ListCS);
    return ret;
}

static int ChangeToFreeList(int idx)
{
    int ret = 0;
    EVENT_LIST_t* pEventList=NULL;
    unsigned int i;
    int findidx=-1;


    EnterCriticalSection(&st_ListCS);
    if(st_pWholeList)
    {
        for(i=0; i<st_ReleaseList.size(); i++)
        {
            if(st_ReleaseList[i]->m_Idx == idx)
            {
                findidx = i;
                pEventList = st_ReleaseList[i];
                break;
            }
        }

        if(findidx >=0)
        {
            st_ReleaseList.erase(st_ReleaseList.begin() + findidx);
            st_FreeList.push_back(pEventList);
			ret = 1;
        }
    }
    LeaveCriticalSection(&st_ListCS);
    return ret;
}


static int InitializeWholeList(int num,unsigned char* pBaseAddr,int packsize,char* pNotifyEvtNameBase)
{
    int ret = -ERROR_ALREADY_EXISTS;
    EVENT_LIST_t* pEventList=NULL;
    int i;
    unsigned char evtname[128];
	int pushbacked=0;

    pEventList= calloc(sizeof(*pEventList),num);
    if(pEventList == NULL)
    {
        ret = -(LAST_ERROR_CODE());
        goto fail;
    }

    for(i=0; i<num; i++)
    {
        pEventList[i].m_hNotifyEvent = NULL;
        pEventList[i].m_Error =0 ;
        pEventList[i].m_Idx = i;
        pEventList[i].m_BaseAddr = (ptr_type_t)pBaseAddr;
        pEventList[i].m_Offset = i * packsize;
        pEventList[i].m_Size = packsize;
    }

    for(i=0; i<num; i++)
    {
        snprintf(evtname,sizeof(evtname),"%s%d",pNotifyEvtNameBase,i);
        pEventList[i].m_hNotifyEvent = GetEvent(evtname,0);
        if(pEventList[i].m_hNotifyEvent)
        {
            ret = -(LAST_ERROR_CODE());
            goto fail;
        }
    }

    ret = 0;
    EnterCriticalSection(&st_ListCS);
    if(st_pWholeList == NULL)
    {
        st_pWholeList = pEventList;
        st_WholeListNum = num;
		assert(st_ReleaseList.size() == 0);
		assert(st_FreeList.size() == 0);
		for (i=0;i<num;i++)
		{
			st_ReleaseList.push_back(&(pEventList[i]));
			pushbacked= 1;
		}
    }
    else
    {
        ret = -ERROR_ALREADY_EXISTS;
    }
    LeaveCriticalSection(&st_ListCS);
    if(ret < 0)
    {
        goto fail;
    }
    return 0;
fail:
	if (pushbacked)
	{
		assert(pEventList);
		EnterCriticalSection(&st_ListCS);
		while(st_ReleaseList.size() > 0)
		{
			st_ReleaseList.erase(st_ReleaseList.begin());
		}
	
		while(st_FreeList.size() > 0)
		{
			st_FreeList.erase(st_FreeList.begin());
		}
		LeaveCriticalSection(&st_ListCS);
	}
	pushbacked = 0;
    if(pEventList)
    {
        for(i=0; i<num; i++)
        {
            if(pEventList[i].m_hNotifyEvent)
            {
                CloseHandle(pEventList[i].m_hNotifyEvent);
            }
            pEventList[i].m_hNotifyEvent = NULL;
            pEventList[i].m_Error = 0;
            pEventList[i].m_BaseAddr = 0;
            pEventList[i].m_Offset = 0;
            pEventList[i].m_Idx =0;
        }

        free(pEventList);
    }
    pEventList = NULL;
    return ret;
}





void DeInitializeWholeList(void)
{
    EVENT_LIST_t* pEventList=NULL;
    int num=0;
    int i;
    int ret;

    do
    {
        ret =0;
        EnterCriticalSection(&st_ListCS);
        if(st_GetWholeListNum == 0)
        {
            pEventList = st_pWholeList;
            num = st_WholeListNum;
            st_pWholeList = NULL;
            num = 0;
            ret = 1;
        }
        LeaveCriticalSection(&st_ListCS);
        if(ret == 0)
        {
            SchedOut();
        }
    }
    while(ret == 0);

    if(pEventList)
    {
        for(i=0; i<num; i++)
        {
            if(pEventList[i].m_hNotifyEvent)
            {
                CloseHandle(pEventList[i].m_hNotifyEvent);
            }
            pEventList[i].m_hNotifyEvent = NULL;
        }

        free(pEventList);
    }
    else
    {
        if(num != 0)
        {
            ERROR_INFO("set pWholeList num %d\n",num);
        }
    }

    pEventList = NULL;

    return;
}


int WriteSendBuffer(unsigned char* pBuffer,int numpacks)
{
    EVENT_LIST_t* pEventList=NULL;
    int ret;
    int format,channels,samplerate,bitspersample;
    float volume;
    int numbytes;
    PCMCAP_AUDIO_BUFFER_t* pAudioBuffer=NULL;
    PCMCAP_AUDIO_BUFFER_t audioformat;

    pEventList = GetFreeList();
    if(pEventList == NULL)
    {
        return 0;
    }

    ret = GetFormat(&(audioformat.m_Format),&(audioformat.m_Channels),&(audioformat.m_SampleRate),&(audioformat.m_BitsPerSample),&(audioformat.m_Volume));
    if(ret <= 0)
    {
        PutFreeList(pEventList);
        return 0;
    }

    /*now to copy the file*/
    numbytes = numpacks * channels *(bitspersample / 8);
    pAudioBuffer = (PCMCAP_AUDIO_BUFFER_t*)((ptr_type_t)pEventList->m_BaseAddr+pEventList->m_Offset);
    ret = WriteShareMem(pAudioBuffer,0,&audioformat,sizeof(audioformat)-sizeof(audioformat.m_AudioData));
    if(ret < 0)
    {
        PutFreeList(pEventList);
        return 0;
    }

    if(numbytes > pEventList->m_Size)
    {
        PutFreeList(pEventList);
        return 0;
    }


    ret = WriteShareMem(&(pAudioBuffer->m_AudioData.m_Data),0,pBuffer,numbytes);
    if(ret < 0)
    {
        PutFreeList(pEventList);
        return 0;
    }

    ret = WriteShareMem(&(pAudioBuffer->m_AudioData.m_DataLen),0,&(numbytes),sizeof(numbytes));
    if(ret < 0)
    {
        PutFreeList(pEventList);
        return 0;
    }

    /*now ok ,so we notify the other of the */
    SetEvent(pEventList->m_hNotifyEvent);
    PutReleaseList(pEventList);
    return 1;
}

int NotifyAudioStart(void)
{
    int ret=0;
    BOOL bret;
    EnterCriticalSection(&st_ListCS);
    if(st_hStartNotifyEvent)
    {
        bret = SetEvent(st_hStartNotifyEvent);
        if(bret)
        {
            ret = 1;
        }
        else
        {
            ret = -LAST_ERROR_CODE();
        }
    }
    LeaveCriticalSection(&st_ListCS);
    return ret;
}


int NotifyAudioStop(void)
{
    int ret=0;
    BOOL bret;
    EnterCriticalSection(&st_ListCS);
    if(st_hStopNotifyEvent)
    {
        bret = SetEvent(st_hStopNotifyEvent);
        if(bret)
        {
            ret = 1;
        }
        else
        {
            ret = -LAST_ERROR_CODE();
        }
    }
    LeaveCriticalSection(&st_ListCS);
    return ret;
}

int SetStartNotify(HANDLE hStartNotifyEvt)
{
    HANDLE origevt=NULL;

    EnterCriticalSection(&st_ListCS);
    origevt = st_hStartNotifyEvent;
    st_hStartNotifyEvent = hStartNotifyEvt;
    LeaveCriticalSection(&st_ListCS);

    if(origevt)
    {
        CloseHandle(origevt);
    }
    origevt = NULL;
    return 0;
}

int SetStopNotify(HANDLE hStopNotifyEvt)
{
    HANDLE origevt=NULL;

    EnterCriticalSection(&st_ListCS);
    origevt = st_hStopNotifyEvent;
    st_hStopNotifyEvent = hStopNotifyEvt;
    LeaveCriticalSection(&st_ListCS);

    if(origevt)
    {
        CloseHandle(origevt);
    }
    origevt = NULL;
    return 0;
}


/*****************************************************
* to make the start and stop thread
*****************************************************/

static HANDLE *st_pFreeEvt=NULL;
static int st_TotalPacks;

typedef struct
{
    HANDLE m_hThread;
    DWORD m_ThreadId;
    int m_ThreadRunning;
    int m_ThreadExited;
    int m_PackNums;
    HANDLE m_NotifyHandle;
    HANDLE m_FreeEvt[1];
} THREAD_INFO_t;


void* __WaitThreadImpl(void* arg)
{
    int ret;
    THREAD_INFO_t *pThreadInfo= (THREAD_INFO_t*)arg;
    DWORD dret;
    HANDLE *pWaitHandles  = NULL;
    int i,idx;

    pWaitHandles = calloc(sizeof(*pWaitHandles),pThreadInfo->m_PackNums + 1);
    if(pWaitHandles == NULL)
    {
        ret = -LAST_ERROR_CODE();
        goto out;
    }

    for(i=0; i<pThreadInfo->m_PackNums; i++)
    {
        pWaitHandles[i] = pThreadInfo->m_FreeEvt[i];
    }

	pWaitHandles[pThreadInfo->m_PackNums] = pThreadInfo->m_NotifyHandle;

    while(pThreadInfo->m_ThreadRunning)
    {
        /*now we should  wait for 1 second ,this will give enough time */
        dret = WaitForMultipleObjects(pThreadInfo->m_PackNums+1,pWaitHandles,FALSE,INFINITE);
        if(dret == WAIT_TIMEOUT)
        {
            continue;
        }
		else if (dret >= WAIT_OBJECT_0 && dret <= (WAIT_OBJECT_0 + pThreadInfo->m_PackNums))
		{
			/*this is the free list event */
			idx = dret - WAIT_OBJECT_0;

			ret = ChangeToFreeList(idx);
			assert(ret > 0);			
		}
		else if (dret == (WAIT_OBJECT_0 + pThreadInfo->m_PackNums + 1))
		{
			/*this is the notify packs*/
			continue;
		}
		else if (dret == WAIT_FAILED)
		{
			ret = -LAST_ERROR_CODE();
			ERROR_INFO("wait %d event error (%d)\n",pThreadInfo->m_PackNums + 1,ret);
			goto out;
		}

		
    }

	ret = 0;
out:
    if(pWaitHandles)
    {
        free(pWaitHandles);
    }
    pWaitHandles = NULL;
    pThreadInfo->m_ThreadExited = 1;
    return ret;
}


static THREAD_INFO_t *st_pThreadInfo=NULL;
static HANDLE st_hThreadSema=NULL;

void FreeThreadInfo(THREAD_INFO_t* pThreadInfo)
{
	/*now first to test whether the thread is running*/
	BOOL bret;
	int ret;
	int i;

	if (pThreadInfo == NULL)
	{
		return;
	}
	
	pThreadInfo->m_ThreadRunning = 0;
	while(pThreadInfo->m_ThreadExited == 0)
	{
		if (pThreadInfo->m_NotifyHandle)
		{
			bret = SetEvent(pThreadInfo->m_NotifyHandle);
			if (!bret)
			{
				ret = LAST_ERROR_CODE();
				ERROR_INFO("could not set notify handle error %d\n",ret);
			}
		}
		SchedOut();
	}

	/*now we are ok*/
	if (pThreadInfo->m_NotifyHandle)
	{
		bret = CloseHandle(pThreadInfo->m_NotifyHandle);
		if (!bret)
		{
			ret=  LAST_ERROR_CODE();
			ERROR_INFO("close notify handle error(%d)\n",ret);
		}
	}
	pThreadInfo->m_NotifyHandle = NULL;

	if (pThreadInfo->m_hThread)
	{
		bret = CloseHandle(pThreadInfo->m_hThread);
		if (!bret)
		{
			ret = LAST_ERROR_CODE();
			ERROR_INFO("close thread handle error(%d)\n",ret);
		}
	}
	pThreadInfo->m_hThread = NULL;
	pThreadInfo->m_ThreadId = 0;

	for (i=0;i<pThreadInfo->m_PackNums;i++)
	{
		if (pThreadInfo->m_FreeEvt[i])
		{
			bret = CloseHandle(pThreadInfo->m_FreeEvt[i]);
			if (!bret)
			{
				ret = LAST_ERROR_CODE();
				ERROR_INFO("close[%d] free event error(%d)\n",i,ret);
			}
		}
		pThreadInfo->m_FreeEvt[i] = NULL;
	}

	pThreadInfo->m_PackNums = 0;
	free(pThreadInfo);	
	return ;
}

THREAD_INFO_t* AllocateThreadInfo(unsigned int numpacks)
{
	THREAD_INFO_t* pThreadInfo=NULL;
	int size=sizeof(*pThreadInfo);
	int i;

	if (numpacks < 1)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	size += (numpacks-1)*sizeof(pThreadInfo->m_FreeEvt[0]);
	pThreadInfo =(THREAD_INFO_t*) malloc(size);
	if (pThreadInfo == NULL)
	{
		return NULL;
	}

	memset(pThreadInfo,0,size);
	pThreadInfo->m_hThread = NULL;
	pThreadInfo->m_ThreadId = 0;
	pThreadInfo->m_ThreadRunning = 1;
	pThreadInfo->m_ThreadExited = 1;
	pThreadInfo->m_PackNums = numpacks;
	pThreadInfo->m_NotifyHandle = NULL;
	for (i=0;i<pThreadInfo->m_PackNums;i++)
	{
		pThreadInfo->m_FreeEvt[i] = NULL;
	}
	return pThreadInfo;	
}



int __HandleAudioRecordStart(PCMCAP_CONTROL_t *pControl)
{
    int ret =0;
	unsigned int i;
	unsigned char eventname[128];

    if(st_pThreadInfo)
    {
        return -ERROR_ALREADY_EXISTS;
    }

	st_pThreadInfo = AllocateThreadInfo(pControl->m_NumPacks);
	if (st_pThreadInfo == NULL)
	{
		ret = LAST_ERROR_CODE();
		goto fail;
	}

	/*now we should set for the handle*/
	st_pThreadInfo->m_NotifyHandle = GetEvent(NULL,1);
	if (st_pThreadInfo->m_NotifyHandle == NULL)
	{
		ret = LAST_ERROR_CODE();
		goto fail;
	}

	/*now for the name*/
	for (i=0;i<pControl->m_NumPacks;i++)
	{
		snprintf(eventname,sizeof(eventname),"%s%d",pControl->m_FreeListSemNameBase,i);
		st_pThreadInfo->m_FreeEvt[i] = GetEvent(eventname,0);
		if (st_pThreadInfo->m_FreeEvt[i] == NULL)
		{
			ret = LAST_ERROR_CODE();
			goto fail;
		}
	}

	/*now we should to give the memory get */



    return ret;
fail:
    assert(ret > 0);
	FreeThreadInfo(st_pThreadInfo);	
    SetLastError(ret);
    return -ret;
}

int __HandleAudioRecordStop(PCMCAP_CONTROL_t *pControl)
{
}


int HandleAudioOperation(PCMCAP_CONTROL_t *pControl)
{
    int ret = 0;
    unsigned int tmms = pControl->m_Timeout* 1000;
    DWORD dret;
    if(st_hThreadSema == NULL)
    {
        return -ERROR_BAD_ENVIRONMENT;
    }

    dret = WaitForSingleObject(st_hThreadSema , tmms ? tmms : INFINITE);
    if(dret != WAIT_OBJECT_0)
    {
        ret = -(LAST_ERROR_CODE());
        ERROR_INFO("could not wait thread sema in %d error(%d)\n",pControl->m_Timeout,ret);
        return ret;
    }

    switch(pControl->m_Operation)
    {
    case PCMCAP_AUDIO_CAPTURE:
    case PCMCAP_AUDIO_BOTH:
        ret = __HandleAudioRecordStart(pControl);
        break;
    case PCMCAP_AUDIO_NONE:
    case PCMCAP_AUDIO_RENDER:
        ret = __HandleAudioRecordStop(pControl);
        break;
    default:
        ret = -ERROR_NOT_SUPPORTED;
        break;
    }

    ReleaseSemaphore(st_hThreadSema);
    return ret;
}



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
* stream audio volume
*****************************************************/
#define  STREAM_AUDIO_VOLUME_RELEASE_NUM             2
#define  STREAM_AUDIO_VOLUME_SET_CHANNEL_VOLUME_NUM  4
#define  STREAM_AUDIO_VOLUME_SET_ALL_VOLUMES_NUM     6

typedef ULONG(WINAPI *StreamAudioVolumeReleaseFunc_t)(IAudioStreamVolume* pThis);
static StreamAudioVolumeReleaseFunc_t StreamAudioVolumeReleaseNext=NULL;
static int st_StreamAudioVolumeReleaseDetoured=0;

ULONG WINAPI  StreamAudioVolumeReleaseCallBack(IAudioStreamVolume *pThis)
{
    ULONG uret;

    uret = StreamAudioVolumeReleaseNext(pThis);
    return uret;
}

typedef HRESULT(WINAPI  *StreamAudioVolumeSetChannelVolumeFunc_t)(IAudioStreamVolume * pThis,UINT32 dwIndex,const float fLevel);
static StreamAudioVolumeSetChannelVolumeFunc_t StreamAudioVolumeSetChannelVolumeNext=NULL;
static int st_StreamAudioVolumeSetChannelVolumeDetoured=0;

HRESULT WINAPI StreamAudioVolumeSetChannelVolumeCallBack(IAudioStreamVolume * pThis,UINT32 dwIndex,const float fLevel)
{
    HRESULT hr;
    hr = StreamAudioVolumeSetChannelVolumeNext(pThis,dwIndex,fLevel);
    if(SUCCEEDED(hr) && dwIndex == 0)
    {
        SetVolume(fLevel);
    }
    return hr;
}


typedef HRESULT(WINAPI *StreamAudioVolumeSetAllVolumesFunc_t)(IAudioStreamVolume * This,UINT32 dwCount,const float *pfVolumes);
static StreamAudioVolumeSetAllVolumesFunc_t StreamAudioVolumeSetAllVolumesNext=NULL;
static int st_StreamAudioVolumeSetAllVolumesDetoured=0;

HRESULT WINAPI StreamAudioVolumeSetAllVolumesCallBack(IAudioStreamVolume * This,UINT32 dwCount,const float *pfVolumes)
{
    HRESULT hr;

    hr = StreamAudioVolumeSetAllVolumesNext(This,dwCount,pfVolumes);
    if(SUCCEEDED(hr) && dwCount > 0)
    {
        SetVolume(pfVolumes[0]);
    }
    return hr;
}

int DetourStreamAudioVolumeVirtFunctions(IAudioStreamVolume *pStream)
{
    DetourVirtualFuncTable(&st_DetourCS,&st_StreamAudioVolumeReleaseDetoured,(void**)&StreamAudioVolumeReleaseNext,StreamAudioVolumeReleaseCallBack,pStream,STREAM_AUDIO_VOLUME_RELEASE_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_StreamAudioVolumeSetChannelVolumeDetoured,(void**)&StreamAudioVolumeSetChannelVolumeNext,StreamAudioVolumeSetChannelVolumeCallBack,pStream,STREAM_AUDIO_VOLUME_SET_CHANNEL_VOLUME_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_StreamAudioVolumeSetAllVolumesDetoured,(void**)&StreamAudioVolumeSetAllVolumesNext,StreamAudioVolumeSetAllVolumesCallBack,pStream,STREAM_AUDIO_VOLUME_SET_ALL_VOLUMES_NUM);
    return 0;
}




/*****************************************************
* channel audio volume
*****************************************************/
#define  CHANNEL_AUDIO_VOLUME_RELEASE_NUM            2
#define  CHANNEL_AUDIO_VOLUME_SET_CHANNEL_VOLUME_NUM 4
#define  CHANNEL_AUDIO_VOLUME_SET_ALL_VOLUMES_NUM    6

typedef ULONG(WINAPI *ChannelAudioVolumeReleaseFunc_t)(IChannelAudioVolume* pThis);
static ChannelAudioVolumeReleaseFunc_t ChannelAudioVolumeReleaseNext=NULL;
static int st_ChannelAudioVolumeReleaseDetoured=0;

ULONG WINAPI  ChannelAudioVolumeReleaseCallBack(IChannelAudioVolume *pThis)
{
    ULONG uret;

    uret = ChannelAudioVolumeReleaseNext(pThis);
    return uret;
}

typedef HRESULT(WINAPI  *ChannelAudioVolumeSetChannelVolumeFunc_t)(IChannelAudioVolume * pThis,UINT32 dwIndex,const float fLevel,LPCGUID EventContext);
static ChannelAudioVolumeSetChannelVolumeFunc_t ChannelAudioVolumeSetChannelVolumeNext=NULL;
static int st_ChannelAudioVolumeSetChannelVolumeDetoured=0;

HRESULT WINAPI ChannelAudioVolumeSetChannelVolumeCallBack(IChannelAudioVolume * pThis,UINT32 dwIndex,const float fLevel,LPCGUID EventContext)
{
    HRESULT hr;
    hr = ChannelAudioVolumeSetChannelVolumeNext(pThis,dwIndex,fLevel,EventContext);
    if(SUCCEEDED(hr) && dwIndex == 0)
    {
        SetVolume(fLevel);
    }
    return hr;
}


typedef HRESULT(WINAPI *ChannelAudioVolumeSetAllVolumesFunc_t)(IChannelAudioVolume * This,UINT32 dwCount,const float *pfVolumes,LPCGUID EventContext);
static ChannelAudioVolumeSetAllVolumesFunc_t ChannelAudioVolumeSetAllVolumesNext=NULL;
static int st_ChannelAudioVolumeSetAllVolumesDetoured=0;

HRESULT WINAPI ChannelAudioVolumeSetAllVolumesCallBack(IChannelAudioVolume * This,UINT32 dwCount,const float *pfVolumes,LPCGUID EventContext)
{
    HRESULT hr;

    hr = ChannelAudioVolumeSetAllVolumesNext(This,dwCount,pfVolumes,EventContext);
    if(SUCCEEDED(hr) && dwCount > 0)
    {
        SetVolume(pfVolumes[0]);
    }
    return hr;
}

int DetourChannelAudioVolumeVirtFunctions(IChannelAudioVolume *pChannel)
{
    DetourVirtualFuncTable(&st_DetourCS,&st_ChannelAudioVolumeReleaseDetoured,(void**)&ChannelAudioVolumeReleaseNext,ChannelAudioVolumeReleaseCallBack,pChannel,CHANNEL_AUDIO_VOLUME_RELEASE_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_ChannelAudioVolumeSetChannelVolumeDetoured,(void**)&ChannelAudioVolumeSetChannelVolumeNext,ChannelAudioVolumeSetChannelVolumeCallBack,pChannel,CHANNEL_AUDIO_VOLUME_SET_CHANNEL_VOLUME_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_ChannelAudioVolumeSetAllVolumesDetoured,(void**)&ChannelAudioVolumeSetAllVolumesNext,ChannelAudioVolumeSetAllVolumesCallBack,pChannel,CHANNEL_AUDIO_VOLUME_SET_ALL_VOLUMES_NUM);
    return 0;
}

/*****************************************************
* simple audio volume
*****************************************************/
#define  SIMPLE_AUDIO_VOLUME_RELEASE_NUM             2
#define  SIMPLE_AUDIO_VOLUME_SET_MASTER_VOLUME_NUM   3

typedef ULONG(WINAPI *SimpleAudioVolumeReleaseFunc_t)(ISimpleAudioVolume *pThis);
static SimpleAudioVolumeReleaseFunc_t SimpleAudioVolumeReleaseNext=NULL;
static int st_SimpleAudioVolumeReleaseDetoured=0;

ULONG WINAPI SimpleAudioVolumeReleaseCallBack(ISimpleAudioVolume *pThis)
{
    ULONG uret;

    uret = SimpleAudioVolumeReleaseNext(pThis);
    return uret;
}

typedef HRESULT(WINAPI *SimpleAudioVolumeSetMasterVolumeFunc_t)(ISimpleAudioVolume * pThis,float fLevel,LPCGUID EventContext);
static SimpleAudioVolumeSetMasterVolumeFunc_t SimpleAudioVolumeSetMasterVolumeNext=NULL;
static int st_SimpleAudioVolumeSetMasterVolumeDetoured=0;

HRESULT WINAPI SimpleAudioVolumeSetMasterVolumeCallBack(ISimpleAudioVolume * pThis,float fLevel,LPCGUID EventContext)
{
    HRESULT hr;

    hr = SimpleAudioVolumeSetMasterVolumeNext(pThis,fLevel,EventContext);
    if(SUCCEEDED(hr))
    {
        SetVolume(fLevel);
    }
    return hr;
}


static int DetourSimpleAudioVolumeVirtFunctions(ISimpleAudioVolume *pThis)
{
    DetourVirtualFuncTable(&st_DetourCS,&st_SimpleAudioVolumeReleaseDetoured,(void**)&SimpleAudioVolumeReleaseNext,SimpleAudioVolumeReleaseCallBack,pThis,SIMPLE_AUDIO_VOLUME_RELEASE_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_SimpleAudioVolumeSetMasterVolumeDetoured,(void**)&SimpleAudioVolumeSetMasterVolumeNext,SimpleAudioVolumeSetMasterVolumeCallBack,pThis,SIMPLE_AUDIO_VOLUME_SET_MASTER_VOLUME_NUM);
    return 0;
}

/*****************************************************
* audio render client
*****************************************************/
#define  AUDIO_RENDER_RELEASE_NUM        2
#define  AUDIO_RENDER_GET_BUFFER_NUM     3
#define  AUDIO_RENDER_RELEASE_BUFFER_NUM 4

typedef ULONG(WINAPI *AudioRenderClientReleaseFunc_t)(IAudioRenderClient* pRender);
static AudioRenderClientReleaseFunc_t AudioRenderClientReleaseNext=NULL;
static int st_AudioRenderClientReleaseDetoured=0;

ULONG WINAPI AudioRenderClientReleaseCallBack(IAudioRenderClient* pRender)
{
    ULONG uret;

    uret = AudioRenderClientReleaseNext(pRender);
    return uret;
}

typedef HRESULT(WINAPI *AudioRenderClientGetBufferFunc_t)(IAudioRenderClient* pRender,UINT32 NumFramesRequested,BYTE **ppData);
static AudioRenderClientGetBufferFunc_t AudioRenderClientGetBufferNext=NULL;
static int st_AudioRenderClientGetBufferDetoured=0;

static unsigned char* st_pRenderBuffer=NULL;

HRESULT WINAPI AudioRenderClientGetBufferCallBack(IAudioRenderClient* pRender,UINT32 NumFramesRequested,BYTE **ppData)
{
    HRESULT hr;
    hr = AudioRenderClientGetBufferNext(pRender,NumFramesRequested,ppData);
    if(SUCCEEDED(hr))
    {
        st_pRenderBuffer = *ppData;
    }
    return hr;
}

typedef HRESULT(WINAPI *AudioRenderClientReleaseBufferFunct_t)(IAudioRenderClient* pRender,UINT32 NumFramesWritten,DWORD dwFlags);
static AudioRenderClientReleaseBufferFunct_t AudioRenderClientReleaseBufferNext=NULL;
static int st_AudioRenderClientReleaseBufferDetoured=0;


HRESULT WINAPI AudioRenderClientReleaseBufferCallBack(IAudioRenderClient* pRender,UINT32 NumFramesWritten,DWORD dwFlags)
{
    HRESULT hr;
    int operation;
    DWORD newdwflag=dwFlags;

    operation = GetOperation();
    if(operation == PCMCAPPER_OPERATION_NONE ||
            operation == PCMCAPPER_OPERATION_CAPTURE)
    {
        newdwflag |= AUDCLNT_BUFFERFLAGS_SILENT;
    }

    hr = AudioRenderClientReleaseBufferNext(pRender,NumFramesWritten,newdwflag);
    if(SUCCEEDED(hr))
    {
        if(st_pRenderBuffer)
        {
            /*write buffer */
            if(!(dwFlags & AUDCLNT_BUFFERFLAGS_SILENT) && (operation == PCMCAPPER_OPERATION_CAPTURE || operation == PCMCAPPER_OPERATION_BOTH))
            {
                WriteSendBuffer(st_pRenderBuffer,NumFramesWritten);
            }
        }
        st_pRenderBuffer = NULL;
    }
    return hr;
}

static int DetourAudioRenderClientVirtFunctions(IAudioRenderClient *pRender)
{
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioRenderClientReleaseDetoured,(void**)&AudioRenderClientReleaseNext,AudioRenderClientReleaseCallBack,pRender,AUDIO_RENDER_RELEASE_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioRenderClientGetBufferDetoured,(void**)&AudioRenderClientGetBufferNext,AudioRenderClientGetBufferCallBack,pRender,AUDIO_RENDER_GET_BUFFER_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioRenderClientReleaseBufferDetoured,(void**)&AudioRenderClientReleaseBufferNext,AudioRenderClientReleaseBufferCallBack,pRender,AUDIO_RENDER_RELEASE_BUFFER_NUM);
    return 0;
}

/*****************************************************
* audio client
*****************************************************/
#define  AUDIO_CLIENT_RELEASE_NUM        2
#define  AUDIO_CLIENT_INITIALIZE_NUM     3
#define  AUDIO_CLIENT_START_NUM          10
#define  AUDIO_CLIENT_STOP_NUM           11
#define  AUDIO_CLIENT_RESET_NUM          12
#define  AUDIO_CLIENT_GET_SERVICE_NUM    14

typedef ULONG(WINAPI *AudioClientReleaseFunc_t)(IAudioClient* pClient);
static AudioClientReleaseFunc_t AudioClientReleaseNext=NULL;
static int st_AudioClientReleaseDetoured=0;

ULONG WINAPI AudioClientReleaseCallBack(IAudioClient* pClient)
{
    ULONG uret;

    uret = AudioClientReleaseNext(pClient);

    return uret;
}


typedef HRESULT(WINAPI *AudioClientInitializeFunc_t)(IAudioClient* pClient,AUDCLNT_SHAREMODE ShareMode,DWORD StreamFlags,REFERENCE_TIME hnsBufferDuration,REFERENCE_TIME hnsPeriodicity,const WAVEFORMATEX *pFormat,LPCGUID AudioSessionGuid);
static AudioClientInitializeFunc_t AudioClientInitializeNext=NULL;
static int st_AudioClientInitializeDetoured=0;

HRESULT WINAPI AudioClientInitializeCallBack(IAudioClient* pClient,AUDCLNT_SHAREMODE ShareMode,DWORD StreamFlags,REFERENCE_TIME hnsBufferDuration,REFERENCE_TIME hnsPeriodicity,const WAVEFORMATEX *pFormat,LPCGUID AudioSessionGuid)
{
    HRESULT hr;

    hr = AudioClientInitializeNext(pClient,ShareMode,StreamFlags,hnsBufferDuration,hnsPeriodicity,pFormat,AudioSessionGuid);
    if(SUCCEEDED(hr) && pFormat)
    {
        SetFormat(pFormat);
    }
    return hr;
}

typedef HRESULT(WINAPI  *AudioClientStartFunc_t)(IAudioClient* pClient);
static AudioClientStartFunc_t AudioClientStartNext=NULL;
static int st_AudioClientStartDetoured=0;

HRESULT WINAPI AudioClientStartCallBack(IAudioClient* pClient)
{
    HRESULT hr;

    hr = AudioClientStartNext(pClient);
    if(SUCCEEDED(hr))
    {
        NotifyAudioStart();
    }
    return hr;
}


typedef HRESULT(WINAPI *AudioClientStopFunc_t)(IAudioClient* pClient);
static AudioClientStopFunc_t AudioClientStopNext=NULL;
static int st_AudioClientStopDetoured=0;

HRESULT WINAPI AudioClientStopCallBack(IAudioClient* pClient)
{
    HRESULT hr;

    hr = AudioClientStopNext(pClient);
    if(SUCCEEDED(hr))
    {
        NotifyAudioStop();
    }
    return hr;
}

typedef HRESULT(WINAPI *AudioClientResetFunc_t)(IAudioClient* pClient);
static AudioClientResetFunc_t AudioClientResetNext=NULL;
static int st_AudioClientResetDetoured=0;

HRESULT WINAPI AudioClientResetCallBack(IAudioClient* pClient)
{
    HRESULT hr;

    hr = AudioClientResetNext(pClient);
    if(SUCCEEDED(hr))
    {
        NotifyAudioStop();
        NotifyAudioStart();
    }
    return hr;
}


typedef HRESULT(WINAPI *AudioClientGetServiceFunc_t)(IAudioClient* pClient,REFIID riid,void **ppv);
static AudioClientGetServiceFunc_t AudioClientGetServiceNext=NULL;
static int st_AudioClientGetServiceDetoured=0;

HRESULT WINAPI AudioClientGetServiceCallBack(IAudioClient* pClient,REFIID riid,void **ppv)
{
    HRESULT hr;

    hr = AudioClientGetServiceNext(pClient,riid,ppv);
    if(SUCCEEDED(hr))
    {
        if(riid == __uuidof(IAudioRenderClient))
        {
            IAudioRenderClient* pRender = (IAudioRenderClient*)*ppv;
            DetourAudioRenderClientVirtFunctions(pRender);
        }
        else if(riid == __uuidof(IAudioStreamVolume))
        {
            IAudioStreamVolume* pStream= (IAudioStreamVolume*)*ppv;
            DetourStreamAudioVolumeVirtFunctions(pStream);
        }
        else if(riid == __uuidof(IChannelAudioVolume))
        {
            IChannelAudioVolume* pChannel = (IChannelAudioVolume*)*ppv;
            DetourChannelAudioVolumeVirtFunctions(pChannel);
        }
        else if(riid == __uuidof(ISimpleAudioVolume))
        {
            ISimpleAudioVolume* pSimple = (ISimpleAudioVolume*)*ppv;
            DetourSimpleAudioVolumeVirtFunctions(pSimple);
        }
    }
    return hr;
}

static int DetourDeviceAudioClientVirtFunctions(IAudioClient* pClient)
{
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientReleaseDetoured,(void**)&AudioClientReleaseNext,AudioClientReleaseCallBack,pClient,AUDIO_CLIENT_RELEASE_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientInitializeDetoured,(void**)&AudioClientInitializeNext,AudioClientInitializeCallBack,pClient,AUDIO_CLIENT_INITIALIZE_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientStartDetoured,(void**)&AudioClientStartNext,AudioClientStartCallBack,pClient,AUDIO_CLIENT_START_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientStopDetoured,(void**)&AudioClientStopNext,AudioClientStopCallBack,pClient,AUDIO_CLIENT_STOP_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientResetDetoured,(void**)&AudioClientResetNext,AudioClientResetCallBack,pClient,AUDIO_CLIENT_RESET_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientGetServiceDetoured,(void**)&AudioClientGetServiceNext,AudioClientGetServiceCallBack,pClient,AUDIO_CLIENT_GET_SERVICE_NUM);
    return 0;
}

/*****************************************************
* device handler
*****************************************************/
#define DEVICE_RELEASE_NUM               2
#define DEVICE_ACTIVATE_NUM              3

typedef ULONG(WINAPI *DeviceReleaseFunc_t)(IMMDevice *pThis);
static DeviceReleaseFunc_t DeviceReleaseNext=NULL;
static int st_DeviceReleaseDetoured=0;

ULONG WINAPI DeviceReleaseCallBack(IMMDevice *pThis)
{
    ULONG uret;

    uret = DeviceReleaseNext(pThis);
    return uret;
}

typedef HRESULT(WINAPI *DeviceActivateFunc_t)(IMMDevice *pThis,REFIID iid,DWORD dwClsCtx,PROPVARIANT *pActivationParams,void **ppInterface);
static DeviceActivateFunc_t DeviceActivateNext=NULL;
static int st_DeviceActivateDetoured=0;

HRESULT WINAPI DeviceActivateCallBack(IMMDevice *pThis,REFIID iid,DWORD dwClsCtx,PROPVARIANT *pActivationParams,void **ppInterface)
{
    HRESULT hr;

    hr = DeviceActivateNext(pThis,iid,dwClsCtx,pActivationParams,ppInterface);
    if(SUCCEEDED(hr))
    {
        if(iid == __uuidof(IAudioClient))
        {
            IAudioClient* pClient=(IAudioClient*)*ppInterface;
            DetourDeviceAudioClientVirtFunctions(pClient);
        }
    }
    return hr;
}



static int DetourDeviceVirtFunctions(IMMDevice* pDevice)
{
    DetourVirtualFuncTable(&st_DetourCS,&st_DeviceReleaseDetoured,(void**)&DeviceReleaseNext,DeviceReleaseCallBack,pDevice,DEVICE_RELEASE_NUM);
    DetourVirtualFuncTable(&st_DetourCS,&st_DeviceActivateDetoured,(void**)&DeviceActivateNext,DeviceActivateCallBack,pDevice,DEVICE_ACTIVATE_NUM);
    return 0;
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
    if(SUCCEEDED(hr))
    {
        DetourDeviceVirtFunctions(*ppDevice);
    }
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
    if(SUCCEEDED(hr))
    {
        DetourDeviceVirtFunctions(*ppEndpoint);
    }
    return hr;
}

typedef HRESULT(WINAPI *EnumeratorGetDeviceFunc_t)(IMMDeviceEnumerator* pThis,LPCWSTR pwstrId,IMMDevice **ppDevice);
static EnumeratorGetDeviceFunc_t EnumeratorGetDeviceNext=NULL;
static int st_EnumeratorGetDeviceDetoured=0;

HRESULT EnumeratorGetDeviceCallBack(IMMDeviceEnumerator* pThis,LPCWSTR pwstrId,IMMDevice **ppDevice)
{
    HRESULT hr;
    hr = EnumeratorGetDeviceNext(pThis,pwstrId,ppDevice);
    if(SUCCEEDED(hr))
    {
        DetourDeviceVirtFunctions(*ppDevice);
    }
    return hr;
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
    InitializeCriticalSection(&st_ListCS);

    ret = DetourPCMCapFunctions();
    if(ret < 0)
    {
        return ret;
    }

    st_PcmCapInited = 1;
    DEBUG_INFO("Init PcmCapInject succ\n");
    return 0;
}

