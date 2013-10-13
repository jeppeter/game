// pcmcapinject.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "pcmcapinject.h"
#include <assert.h>
#include <Mmreg.h>
#include <stdlib.h>
#include <output_debug.h>
#include <detours.h>
#include <audioclient.h>
#include <MMDeviceApi.h>
#include <vector>
#include <sched.h>
#include <evt.h>
#include <memshare.h>
#include <StackWalker.h>

#pragma comment(lib,"common.lib")

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))

#define COM_METHOD(TYPE, METHOD) TYPE STDMETHODCALLTYPE METHOD

static CRITICAL_SECTION st_DetourCS;
static CRITICAL_SECTION st_StateCS;

static PCM_AUDIO_FORMAT_t st_AudioFormat ;
static int st_PcmCapInited=0;
static int st_Operation=PCMCAPPER_OPERATION_NONE;


/*****************************************************
*  to handle for the volume
*****************************************************/
static int SetVolume(float volume)
{
    int ret =0;
    EnterCriticalSection(&st_StateCS);
    st_AudioFormat.m_Volume = volume;
    LeaveCriticalSection(&st_StateCS);
    return ret;
}

/*****************************************************
*  format set and get handler
*****************************************************/

static int SetFormat(WAVEFORMATEX* pFormatEx)
{
    unsigned int size = sizeof(*pFormatEx);
    int error = 0;
    size += pFormatEx->cbSize;


    EnterCriticalSection(&st_StateCS);
    if(size <= sizeof(st_AudioFormat.m_Format))
    {
        memcpy(st_AudioFormat.m_Format,pFormatEx,size);
    }
    else
    {
        error =1;
        memcpy(st_AudioFormat.m_Format,pFormatEx,sizeof(st_AudioFormat.m_Format));
    }
    LeaveCriticalSection(&st_StateCS);

    if(error)
    {
        ERROR_INFO("need size (%d) > sizeof(%d)\n",size,sizeof(st_AudioFormat.m_Format));
        DEBUG_BUFFER(pFormatEx,size);
    }


    DEBUG_BUFFER(pFormatEx,size);
    DEBUG_INFO("format %d channels %d samplespersec %d wBitsPerSample %d\n",
               pFormatEx->wFormatTag,pFormatEx->nChannels,pFormatEx->nSamplesPerSec,pFormatEx->wBitsPerSample);

    return 0;
}

static WAVEFORMATEX* GetCopiedFormat()
{
    unsigned int size=FORMAT_EXTEND_SIZE;
    unsigned int cpysize=0;
    WAVEFORMATEX* pFormatEx=NULL;
    WAVEFORMATEX* pPtrFormatEx=NULL;

    pFormatEx = malloc(size);
    if(pFormatEx == NULL)
    {
        return NULL;
    }
    memset(pFormatEx,0,size);

    EnterCriticalSection(&st_StateCS);
    cpysize = sizeof(*pPtrFormatEx);
    pPtrFormatEx = (WAVEFORMATEX*) st_AudioFormat.m_Format;
    cpysize += pPtrFormatEx->cbSize;
    if(cpysize < size)
    {
        memcpy(pFormatEx,pPtrFormatEx,cpysize);
    }
    else
    {
        memcpy(pFormatEx,pPtrFormatEx,size);
    }
    LeaveCriticalSection(&st_StateCS);

    return pFormatEx;
}





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
    HANDLE m_hFillEvt;
    int m_Error;
    int m_Idx;
    ptr_type_t m_BaseAddr;
    ptr_type_t m_Offset;
    unsigned int m_Size;
} EVENT_LIST_t;


typedef struct
{
    unsigned int m_WholeListNum;
    unsigned int m_PackSize;
    int m_GetWholeListNum;
    THREAD_CONTROL_t m_ThreadControl;
    HANDLE *m_pFreeEvt;
    EVENT_LIST_t *m_pWholeList;
    std::vector<EVENT_LIST_t*>* m_pFreeList;
    std::vector<EVENT_LIST_t*>* m_pFillList;
    HANDLE m_hStartEvt;
    HANDLE m_hStopEvt;
    HANDLE m_hMemMap;
    unsigned char *m_pMemBase;
    int m_MemMapSize;
} PCM_EVTS_t;

static CRITICAL_SECTION st_ListCS;
static PCM_EVTS_t* st_pPCMEvt;

static EVENT_LIST_t* GetFreeList()
{
    EVENT_LIST_t* pEventList=NULL;

    EnterCriticalSection(&st_ListCS);
    if(st_pPCMEvt && st_pPCMEvt->m_pFreeList && st_pPCMEvt->m_pFreeList->size() > 0)
    {
        pEventList = st_pPCMEvt->m_pFreeList->at(0);
        st_pPCMEvt->m_pFreeList->erase(st_pPCMEvt->m_pFreeList->begin());
        st_pPCMEvt->m_GetWholeListNum ++;
    }
    LeaveCriticalSection(&st_ListCS);
    return pEventList;
}

static int PutFillList(EVENT_LIST_t* pEventList)
{
    int ret=0;
    EnterCriticalSection(&st_ListCS);
    if(st_pPCMEvt && st_pPCMEvt->m_pWholeList)
    {
        st_pPCMEvt->m_pFillList->push_back(pEventList);
        assert(st_pPCMEvt->m_GetWholeListNum > 0);
        st_pPCMEvt->m_GetWholeListNum -- ;
        ret = 1;
    }
    LeaveCriticalSection(&st_ListCS);
    return ret;

}

static int PutFreeList(EVENT_LIST_t* pEventList)
{
    int ret=0;
    EnterCriticalSection(&st_ListCS);
    if(st_pPCMEvt)
    {
        st_pPCMEvt->m_pFreeList->push_back(pEventList);
        assert(st_pPCMEvt->m_GetWholeListNum > 0);
        st_pPCMEvt->m_GetWholeListNum -- ;
        ret = 1;
    }
    LeaveCriticalSection(&st_ListCS);
    return ret;
}

static HANDLE GetStartEvent()
{
    HANDLE hStartEvt=NULL;

    EnterCriticalSection(&(st_ListCS));
    if(st_pPCMEvt)
    {
        if(st_pPCMEvt->m_hStartEvt)
        {
            hStartEvt = st_pPCMEvt->m_hStartEvt;
            st_pPCMEvt->m_GetWholeListNum ++;
        }
    }
    LeaveCriticalSection(&(st_ListCS));
    return hStartEvt;
}

static int PutStartEvent(HANDLE hStartEvent)
{
    int ret =0;
    EnterCriticalSection(&(st_ListCS));
    if(st_pPCMEvt)
    {
        if(st_pPCMEvt->m_hStartEvt == hStartEvent)
        {
            assert(st_pPCMEvt->m_GetWholeListNum > 0);
            st_pPCMEvt->m_GetWholeListNum --;
            ret = 1;
        }
    }
    LeaveCriticalSection(&(st_ListCS));
    return ret;
}

static HANDLE GetStopEvent()
{
    HANDLE hStopEvt=NULL;

    EnterCriticalSection(&(st_ListCS));
    if(st_pPCMEvt)
    {
        if(st_pPCMEvt->m_hStopEvt)
        {
            hStopEvt = st_pPCMEvt->m_hStopEvt;
            st_pPCMEvt->m_GetWholeListNum ++;
        }
    }
    LeaveCriticalSection(&(st_ListCS));
    return hStopEvt;
}

static int PutStopEvent(HANDLE hStopEvent)
{
    int ret =0;
    EnterCriticalSection(&(st_ListCS));
    if(st_pPCMEvt)
    {
        if(st_pPCMEvt->m_hStopEvt == hStopEvent)
        {
            assert(st_pPCMEvt->m_GetWholeListNum > 0);
            st_pPCMEvt->m_GetWholeListNum --;
            ret = 1;
        }
    }
    LeaveCriticalSection(&(st_ListCS));
    return ret;
}


static int ChangeToFreeList(PCM_EVTS_t* pPCMEvts,int idx)
{
    int ret = 0;
    EVENT_LIST_t* pEventList=NULL;
    unsigned int i;
    int findidx=-1;


    EnterCriticalSection(&st_ListCS);
    if(pPCMEvts)
    {
        for(i=0; i<pPCMEvts->m_pFillList->size(); i++)
        {
            if(pPCMEvts->m_pFillList->at(i)->m_Idx == idx)
            {
                findidx = i;
                pEventList = pPCMEvts->m_pFillList->at(i);
                break;
            }
        }

        if(findidx >=0)
        {
            pPCMEvts->m_pFillList->erase(pPCMEvts->m_pFillList->begin() + findidx);
            pPCMEvts->m_pFreeList->push_back(pEventList);
            ret = 1;
        }
    }
    LeaveCriticalSection(&st_ListCS);
    return ret;
}


typedef DWORD(WINAPI *ThreadFunc_t)(LPVOID param);

void __InitThreadControl(THREAD_CONTROL_t* pThreadControl)
{
    pThreadControl->m_hThread = NULL;
    pThreadControl->m_ThreadId = 0;
    pThreadControl->m_hExitEvt = NULL;
    pThreadControl->m_ThreadRunning = 0;
    pThreadControl->m_ThreadExited = 1;
}

void __StopThread(THREAD_CONTROL_t* pThreadControl,int force)
{
    int ret;
    BOOL bret;
    int printout =0;
    if(pThreadControl)
    {
        if(pThreadControl->m_hThread)
        {
            pThreadControl->m_ThreadRunning = 0;
            assert(pThreadControl->m_hExitEvt);
            while(pThreadControl->m_ThreadExited == 0)
            {
                bret = SetEvent(pThreadControl->m_hExitEvt);
                if(!bret)
                {
                    ret = LAST_ERROR_CODE();
                    ERROR_INFO("could not setevent 0x%x error(%d)\n",pThreadControl->m_hExitEvt,
                               ret);
                }
                if((printout % 100)==0)
                {
                    DEBUG_INFO("printout %d\n",printout);
                }

                if(force && printout > 5)
                {
                    /*we force to do*/
                    break;
                }
                printout ++;
                SchedOut();
            }
        }

        if(pThreadControl->m_hThread)
        {
            DEBUG_INFO("thread 0x%x\n",pThreadControl->m_hThread);
            CloseHandle(pThreadControl->m_hThread);
        }
        pThreadControl->m_hThread = NULL;
        if(pThreadControl->m_hExitEvt)
        {
            DEBUG_INFO("hExitEvt 0x%x\n",pThreadControl->m_hExitEvt);
            CloseHandle(pThreadControl->m_hExitEvt);
        }
        pThreadControl->m_hExitEvt = NULL;

        pThreadControl->m_ThreadRunning = 0;
        pThreadControl->m_ThreadExited = 1;
        pThreadControl->m_ThreadId = 0;
    }

    return ;
}


int __StartThread(THREAD_CONTROL_t* pThreadControl,ThreadFunc_t pStartFunc,void* pParam)
{
    int ret;

    if(pThreadControl == NULL || pStartFunc == NULL || pThreadControl->m_ThreadRunning != 0 ||
            pThreadControl->m_hThread != NULL || pThreadControl->m_ThreadId != 0
            || pThreadControl->m_hExitEvt != NULL || pThreadControl->m_ThreadExited != 1)
    {
        ret = ERROR_INVALID_PARAMETER;
        SetLastError(ret);
        return -ret;
    }

    pThreadControl->m_hExitEvt = GetEvent(NULL,1);
    if(pThreadControl->m_hExitEvt == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not get exit event error(%d)\n",ret);
        goto fail;
    }

    /*now we should give the start running*/
    pThreadControl->m_ThreadRunning = 1;
    pThreadControl->m_ThreadExited = 0;
    pThreadControl->m_hThread = CreateThread(NULL,0,pStartFunc,pParam,0,&(pThreadControl->m_ThreadId));
    if(pThreadControl->m_hThread == NULL)
    {
        pThreadControl->m_ThreadExited = 1;
        pThreadControl->m_ThreadRunning = 0;
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not create thread error(%d)\n",ret);
        goto fail;
    }

    return 0;
fail:
    __StopThread(pThreadControl,0);
    SetLastError(ret);
    return -ret;
}


void __FreePCMEvt(PCM_EVTS_t** ppPCMEvt,int force)
{
    PCM_EVTS_t* pPCMEvt = *ppPCMEvt;
    unsigned int i;
    if(pPCMEvt)
    {
        __StopThread(&(pPCMEvt->m_ThreadControl),force);
        if(pPCMEvt->m_pFreeList)
        {
            DEBUG_INFO("freelist %d\n",pPCMEvt->m_pFreeList->size());

            while(pPCMEvt->m_pFreeList->size() > 0)
            {
                DEBUG_INFO("free list %d\n",pPCMEvt->m_pFreeList->size());
                pPCMEvt->m_pFreeList->erase(pPCMEvt->m_pFreeList->begin());
                DEBUG_INFO("free list %d\n",pPCMEvt->m_pFreeList->size());
            }

            delete pPCMEvt->m_pFreeList;
        }
        pPCMEvt->m_pFreeList = NULL;

        if(pPCMEvt->m_pFillList)
        {
            DEBUG_INFO("filllist %d\n",pPCMEvt->m_pFillList->size());
            while(pPCMEvt->m_pFillList->size() > 0)
            {
                pPCMEvt->m_pFillList->erase(pPCMEvt->m_pFillList->begin());
            }

            delete pPCMEvt->m_pFillList;
        }

        pPCMEvt->m_pFillList = NULL;

        /*now for free the whole list*/
        for(i=0; i<pPCMEvt->m_WholeListNum; i++)
        {
            if(pPCMEvt->m_pWholeList)
            {
                if(pPCMEvt->m_pWholeList[i].m_hFillEvt)
                {
                    DEBUG_INFO("[%d] 0x%x\n",i,pPCMEvt->m_pWholeList[i].m_hFillEvt);
                    CloseHandle(pPCMEvt->m_pWholeList[i].m_hFillEvt);
                }
                pPCMEvt->m_pWholeList[i].m_hFillEvt = NULL;
            }
        }

        if(pPCMEvt->m_pWholeList)
        {
            DEBUG_INFO("wholelist 0x%p\n",pPCMEvt->m_pWholeList);
            free(pPCMEvt->m_pWholeList);
        }
        pPCMEvt->m_pWholeList = NULL;

        if(pPCMEvt->m_pFreeEvt)
        {
            for(i=0; i<pPCMEvt->m_WholeListNum; i++)
            {
                if(pPCMEvt->m_pFreeEvt[i])
                {
                    DEBUG_INFO("[%d] 0x%p\n",i,pPCMEvt->m_pFreeEvt[i]);
                    CloseHandle(pPCMEvt->m_pFreeEvt[i]);
                }
                pPCMEvt->m_pFreeEvt[i]= NULL;
            }

            DEBUG_INFO("freeevt 0x%p\n",pPCMEvt->m_pFreeEvt);
            free(pPCMEvt->m_pFreeEvt);
        }
        pPCMEvt->m_pFreeEvt = NULL;

        if(pPCMEvt->m_hStartEvt)
        {
            DEBUG_INFO("hStartEvt 0x%x\n",pPCMEvt->m_hStartEvt);
            CloseHandle(pPCMEvt->m_hStartEvt);
        }
        pPCMEvt->m_hStartEvt = NULL;

        if(pPCMEvt->m_hStopEvt)
        {
            DEBUG_INFO("hStopEvt 0x%x\n",pPCMEvt->m_hStopEvt);
            CloseHandle(pPCMEvt->m_hStopEvt);
        }
        pPCMEvt->m_hStopEvt = NULL;

        DEBUG_INFO("pMemBase 0x%p\n",pPCMEvt->m_pMemBase);
        UnMapFileBuffer(&(pPCMEvt->m_pMemBase));
        DEBUG_INFO("hMemMap 0x%x\n",pPCMEvt->m_hMemMap);
        CloseMapFileHandle(&(pPCMEvt->m_hMemMap));
        pPCMEvt->m_MemMapSize = 0;
        pPCMEvt->m_WholeListNum = 0;
        assert(pPCMEvt->m_GetWholeListNum == 0);

        DEBUG_INFO("pPCMEvt 0x%p\n",pPCMEvt);
        free(pPCMEvt);
    }
    *ppPCMEvt = NULL;
    return ;
}

DWORD WINAPI __WaitThreadImpl(LPVOID arg);


PCM_EVTS_t* __AllocatePCMEvts(unsigned int num,int packsize,char* pMapFileName,char* pFreeEvtNameBase,char* pFillEvtNameBase,char* pStartEvtName,char* pStopEvtName)
{
    PCM_EVTS_t* pPCMEvt=NULL;
    int ret;
    unsigned int mapsize;
    unsigned int i;
    unsigned char evname[128];

    if(num < 1  || packsize < 0x1000 || pMapFileName == NULL
            || pFreeEvtNameBase == NULL || pFillEvtNameBase == NULL ||
            pStartEvtName == NULL || pStopEvtName == NULL)
    {
        ret = ERROR_INVALID_PARAMETER;
        goto fail;
    }

    pPCMEvt = (PCM_EVTS_t*)calloc(sizeof(*pPCMEvt),1);
    if(pPCMEvt == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("allocate size (%d) error(%d)\n",sizeof(*pPCMEvt),ret);
        goto fail;
    }

    __InitThreadControl(&(pPCMEvt->m_ThreadControl));

    pPCMEvt->m_pFreeList = new std::vector<EVENT_LIST_t*>();
    pPCMEvt->m_pFillList = new std::vector<EVENT_LIST_t*>();

    mapsize = num * packsize;
    pPCMEvt->m_MemMapSize = mapsize;
    pPCMEvt->m_WholeListNum = num;
    pPCMEvt->m_PackSize = packsize;
    /*now we should start map file*/
    pPCMEvt->m_hMemMap = CreateMapFile(pMapFileName,mapsize,0);
    if(pPCMEvt->m_hMemMap == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not map %s size(%d:0x%x) error(%d)\n",pMapFileName,mapsize,mapsize,ret);
        goto fail;
    }

    pPCMEvt->m_pMemBase = MapFileBuffer(pPCMEvt->m_hMemMap,mapsize);
    if(pPCMEvt->m_pMemBase == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not map buffer (%s) size(%d:0x%x) error(%d)\n",pMapFileName,mapsize,mapsize,ret);
        goto fail;
    }

    /*now for the event list*/
    pPCMEvt->m_pWholeList =(EVENT_LIST_t*) calloc(sizeof(pPCMEvt->m_pWholeList[0]),num);
    if(pPCMEvt->m_pWholeList== NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO(" could not calloc %d wholelist error(%d)\n",num,ret);
        goto fail;
    }


    for(i=0; i<num; i++)
    {
        _snprintf_s((char*)evname,sizeof(evname),_TRUNCATE,"%s_%d",pFillEvtNameBase,i);
        pPCMEvt->m_pWholeList[i].m_hFillEvt = GetEvent((const char*)evname,0);
        if(pPCMEvt->m_pWholeList[i].m_hFillEvt == NULL)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("[%d] open %s fill event error(%d)\n",i,evname,ret);
            goto fail;
        }
        pPCMEvt->m_pWholeList[i].m_BaseAddr =(ptr_type_t) pPCMEvt->m_pMemBase;
        pPCMEvt->m_pWholeList[i].m_Error = 0;
        pPCMEvt->m_pWholeList[i].m_Idx = i;
        pPCMEvt->m_pWholeList[i].m_Offset = (packsize)*i;
        pPCMEvt->m_pWholeList[i].m_Size = packsize;
        pPCMEvt->m_pFillList->push_back(&(pPCMEvt->m_pWholeList[i]));
        DEBUG_INFO("filllist %d\n",pPCMEvt->m_pFillList->size());
    }

    pPCMEvt->m_pFreeEvt =(HANDLE*) calloc(sizeof(pPCMEvt->m_pFreeEvt[0]),num);
    if(pPCMEvt->m_pFreeEvt == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not calloc %d free event error(%d)\n",num,ret);
        goto fail;
    }

    for(i=0; i<num; i++)
    {
        _snprintf_s((char*)evname,sizeof(evname),_TRUNCATE,"%s_%d",pFreeEvtNameBase,i);
        pPCMEvt->m_pFreeEvt[i] = GetEvent((const char*)evname,0);
        if(pPCMEvt->m_pFreeEvt[i] == NULL)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("[%d] Get %s free event error(%d)\n",i,evname,ret);
            goto fail;
        }
    }

    pPCMEvt->m_hStartEvt = GetEvent(pStartEvtName,0);
    if(pPCMEvt->m_hStartEvt == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not getevent (%s) start event error(%d)\n",pStartEvtName,ret);
        goto fail;
    }

    pPCMEvt->m_hStopEvt = GetEvent(pStopEvtName,0);
    if(pPCMEvt->m_hStopEvt == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not getevent %s stop event error(%d)\n",pStopEvtName,ret);
        goto fail;
    }

    ret = __StartThread(&(pPCMEvt->m_ThreadControl),__WaitThreadImpl,pPCMEvt);
    if(ret < 0)
    {
        ret = -ret;
        goto fail;
    }


    return pPCMEvt;
fail:
    __FreePCMEvt(&pPCMEvt,0);
    SetLastError(ret);
    return NULL;
}




static int InitializeWholeList(int num,int packsize,char* pMapFileName,char* pFreeEvtNameBase,char* pFillEvtNameBase,char*pStartEvtName,char* pStopEvtName)
{
    int ret = -ERROR_ALREADY_EXISTS;
    PCM_EVTS_t* pPCMEvt=NULL;

    pPCMEvt = __AllocatePCMEvts(num,packsize,pMapFileName, pFreeEvtNameBase,pFillEvtNameBase,pStartEvtName,pStopEvtName);
    if(pPCMEvt == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    ret = 0;
    EnterCriticalSection(&st_ListCS);
    if(st_pPCMEvt == NULL)
    {
        st_pPCMEvt = pPCMEvt;
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
    __FreePCMEvt(&pPCMEvt,0);
    SetLastError(ret);
    return -ret;
}





void DeInitializeWholeList(void)
{
    PCM_EVTS_t* pPCMEvt=NULL;
    int ret;

    do
    {
        ret =0;
        EnterCriticalSection(&st_ListCS);
        if(st_pPCMEvt  && st_pPCMEvt->m_GetWholeListNum > 0)
        {
            ret = 1;
        }
        else if(st_pPCMEvt)
        {
            pPCMEvt = st_pPCMEvt;
            st_pPCMEvt = NULL;
        }
        LeaveCriticalSection(&st_ListCS);
        if(ret > 0)
        {
            SchedOut();
        }
    }
    while(ret > 0);
    __FreePCMEvt(&pPCMEvt,1);
    SetOperation(PCMCAP_AUDIO_NONE);
    return;
}

int IsBufferMute(unsigned char* pBuffer,int size)
{
    int i;
    unsigned char ch;
    unsigned char* pCurPtr=pBuffer;

    /*all bytes are the same ,it means it is mute*/
    ch = 0;

    for(i=0; i<size; i++,pCurPtr++)
    {
        if(*pCurPtr != ch)
        {
            return 0;
        }
    }

    return 1;
}


int WriteSendBuffer(IAudioRenderClient *pClient,unsigned char* pBuffer,int numpacks)
{
    EVENT_LIST_t* pEventList=NULL;
    int ret;
    BOOL bret;
    unsigned int numbytes;
    PCMCAP_AUDIO_BUFFER_t* pAudioBuffer=NULL;
    PCM_AUDIO_FORMAT_t  audioformat;
    WAVEFORMATEX* pAudioFormat=NULL;

    pEventList = GetFreeList();
    if(pEventList == NULL)
    {
        DEBUG_INFO("RenderClient 0x%p get no eventlist\n",pClient);
        return 0;
    }

    ret = GetFormat(pClient,&audioformat);
    if(ret <= 0)
    {
        PutFreeList(pEventList);
        return 0;
    }

    pAudioFormat = (WAVEFORMATEX*)audioformat.m_Format;

    /*now to copy the file*/
    numbytes = numpacks * pAudioFormat->nChannels *(pAudioFormat->wBitsPerSample / 8);

    ret = IsBufferMute(pBuffer,numbytes);
    if(ret > 0)
    {
        PutFreeList(pEventList);
        return 0;
    }
    pAudioBuffer = (PCMCAP_AUDIO_BUFFER_t*)((ptr_type_t)pEventList->m_BaseAddr+pEventList->m_Offset);
    ret = WriteShareMem((unsigned char*)pAudioBuffer,0,(unsigned char*)&audioformat,sizeof(audioformat));
    if(ret < 0)
    {
        ERROR_INFO("audio buffer format write error %d\n",ret);
        PutFreeList(pEventList);
        return 0;
    }

    if(numbytes > (pEventList->m_Size - sizeof(audioformat)-sizeof(pAudioBuffer->m_AudioData) + sizeof(pAudioBuffer->m_AudioData.m_Data)))
    {
        ERROR_INFO("Exceeded size (%d) > (%d - %d - %d + %d)\n",numbytes,pEventList->m_Size ,sizeof(audioformat),
                   sizeof(pAudioBuffer->m_AudioData),sizeof(pAudioBuffer->m_AudioData.m_Data));
        PutFreeList(pEventList);
        return 0;
    }

    ret = WriteShareMem((unsigned char*)pAudioBuffer->m_AudioData.m_Data,0,pBuffer,numbytes);
    if(ret < 0)
    {
        ERROR_INFO("audio buffer data write error %d\n",ret);
        PutFreeList(pEventList);
        return 0;
    }

    ret = WriteShareMem((unsigned char*)&(pAudioBuffer->m_AudioData.m_DataLen),0,(unsigned char*)&(numbytes),sizeof(numbytes));
    if(ret < 0)
    {
        ERROR_INFO("audio datalen write error %d\n",ret);
        PutFreeList(pEventList);
        return 0;
    }

    ret = WriteShareMem((unsigned char*)&(pAudioBuffer->m_AudioData.m_Pointer),0,(unsigned char*)&(pClient),sizeof(pAudioBuffer->m_AudioData.m_Pointer));
    if(ret < 0)
    {
        ERROR_INFO("audio client pointer write error %d\n",ret);
        PutFreeList(pEventList);
        return 0;
    }

    /*now ok ,so we notify the other of the */
    bret = SetEvent(pEventList->m_hFillEvt);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("SetEvent %d error(%d)\n",pEventList->m_Idx,ret);
    }
    //DEBUG_INFO("put filllist packs %d numbytes %d\n",numpacks,numbytes);
    //DEBUG_BUFFER(pAudioBuffer->m_AudioData.m_Data,numbytes > 16 ? 16 : numbytes);
    //DEBUG_BUFFER(pBuffer,numbytes > 16 ? 16 : numbytes);
    PutFillList(pEventList);
    return 1;
}

int NotifyAudioStart(void)
{
    int ret=0;
    HANDLE hStartEvt=NULL;
    BOOL bret;
    int operation ;

    operation = GetOperation();
    if(operation != PCMCAP_AUDIO_BOTH &&
            operation != PCMCAP_AUDIO_CAPTURE)
    {
        return 0;
    }
    hStartEvt = GetStartEvent();
    if(hStartEvt == NULL)
    {
        return 0;
    }

    bret = SetEvent(hStartEvt);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("set start event error(%d)\n",ret);
        PutStartEvent(hStartEvt);
        return 0;
    }
    PutStartEvent(hStartEvt);
    return 1;
}


int NotifyAudioStop(void)
{
    int ret=0;
    BOOL bret;
    HANDLE hStopEvt=NULL;
    int operation ;

    operation = GetOperation();
    if(operation != PCMCAP_AUDIO_BOTH &&
            operation != PCMCAP_AUDIO_CAPTURE)
    {
        return 0;
    }
    hStopEvt = GetStopEvent();
    if(hStopEvt == NULL)
    {
        return 0;
    }

    bret = SetEvent(hStopEvt);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not set stop event error(%d)\n",ret);
        PutStopEvent(hStopEvt);
        return 0;
    }
    PutStopEvent(hStopEvt);
    return 1;
}




/*****************************************************
* to make the start and stop thread
*****************************************************/



DWORD WINAPI __WaitThreadImpl(LPVOID arg)
{
    int ret;
    PCM_EVTS_t *pPCMEvts= (PCM_EVTS_t*)arg;
    DWORD dret;
    HANDLE *pWaitHandles  = NULL;
    unsigned int i,idx;
    unsigned int num;

    num = pPCMEvts->m_WholeListNum ;
    pWaitHandles =(HANDLE*) calloc(sizeof(*pWaitHandles),num + 1);
    if(pWaitHandles == NULL)
    {
        ret = -LAST_ERROR_CODE();
        goto out;
    }

    for(i=0; i<num; i++)
    {
        pWaitHandles[i] = pPCMEvts->m_pFreeEvt[i];
    }

    pWaitHandles[num] = pPCMEvts->m_ThreadControl.m_hExitEvt;

    while(pPCMEvts->m_ThreadControl.m_ThreadRunning)
    {
        /*now we should  wait for 1 second ,this will give enough time */
        dret = WaitForMultipleObjects(num+1,pWaitHandles,FALSE,INFINITE);
        //DEBUG_INFO("dret = %d\n",dret);
        if(dret == WAIT_TIMEOUT)
        {
            continue;
        }
        else if(dret >= WAIT_OBJECT_0 && dret <= (WAIT_OBJECT_0 + num - 1))
        {
            int tries = 0;
            /*this is the free list event */
            idx = dret - WAIT_OBJECT_0;

            do
            {
                ret = ChangeToFreeList(pPCMEvts,idx);
                if(ret == 0)
                {
                    ERROR_INFO("change %d return 0\n",idx);
                    SchedOut();
                }
                tries ++;
            }
            while(ret == 0 && tries <= 5);

            if(ret == 0)
            {
                ERROR_INFO("could not change [%d] to freelist\n",idx);
            }
        }
        else if(dret == (WAIT_OBJECT_0 + num))
        {
            /*this is the notify packs*/
            continue;
        }
        else if(dret == WAIT_FAILED)
        {
            ret = -LAST_ERROR_CODE();
            ERROR_INFO("wait %d event error (%d)\n",num + 1,ret);
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
    pPCMEvts->m_ThreadControl.m_ThreadExited = 1;
    return (DWORD)ret;
}


static HANDLE st_hThreadSema=NULL;


int __HandleAudioRecordStart(PCMCAP_CONTROL_t *pControl)
{
    int ret =0;

    if(st_pPCMEvt)
    {
        return -ERROR_ALREADY_EXISTS;
    }



    /*now it is time to give the mem*/
    ret = InitializeWholeList(pControl->m_NumPacks,pControl->m_PackSize ,
                              (char*)pControl->m_MemShareName,(char*)pControl->m_FreeListSemNameBase,(char*)pControl->m_FillListSemNameBase,
                              (char*)pControl->m_StartEvtName,(char*)pControl->m_StopEvtName);
    if(ret < 0)
    {
        ret = -ret;
        goto fail;
    }


    /*now we should do things ok for it will let the thread running back and it will make the first */
    SetOperation(pControl->m_Operation);
    return 0;
fail:
    assert(ret > 0);
    DeInitializeWholeList();
    SetLastError(ret);
    return -ret;
}

int __HandleAudioRecordStop(PCMCAP_CONTROL_t *pControl)
{
    DeInitializeWholeList();
    SetOperation(pControl->m_Operation);
    return 0;
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

    DEBUG_INFO("tmms %d operation %d\n",tmms,pControl->m_Operation);
    dret = WaitForSingleObject(st_hThreadSema , tmms ? tmms : INFINITE);
    DEBUG_INFO("tmms %d ret %d\n",tmms,dret);
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

    ReleaseSemaphore(st_hThreadSema,1,NULL);
    DEBUG_INFO("operation %d return %d\n",pControl->m_Operation,ret);
    return ret;
}



/*****************************************************
* detour function call table
*****************************************************/
static int DetourVirtualFuncTable(CRITICAL_SECTION* pCS,int* pChanged,void**ppNextFunc,void*pCallBackFunc,void* pObject,int virtfuncnum,const char* pTypeName)
{
    int ret =0;
    int i;
    EnterCriticalSection(pCS);
    if(pChanged && *pChanged == 0)
    {
        /*now to make sure */
        ptr_type_t** vptrptr = (ptr_type_t **)pObject;
        ptr_type_t* vptr = *vptrptr;
        for(i=0; i<=virtfuncnum; i++)
        {
            DEBUG_INFO("[%s]0x%p virtfuncnum[%d] 0x%p\n",pTypeName,pObject,i,vptr[i]);
        }

        DEBUG_INFO("\n");
        assert(ppNextFunc && *ppNextFunc == NULL);
        DEBUG_INFO("[%s]0x%p virtfuncnum[%d]ppNextFunc 0x%p 0x%p\n",pTypeName,pObject,virtfuncnum,ppNextFunc,*ppNextFunc);
        *ppNextFunc =(void*) vptr[virtfuncnum];
        DEBUG_INFO("*ppNextFunc 0x%p CallBackFunc 0x%p\n",*ppNextFunc,pCallBackFunc);
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(ppNextFunc,pCallBackFunc);
        DetourTransactionCommit();
        *pChanged = 1;
        ret = 1;
        DEBUG_INFO("\n");
    }
    else
    {
        ptr_type_t** vptrptr = (ptr_type_t **)pObject;
        ptr_type_t* vptr = *vptrptr;
        unsigned char* pCode;
        pCode = (unsigned char*)vptr[virtfuncnum];
        if(*pCode != 0xe9)
        {
            ERROR_INFO("%s [%d] virtfunc not detoured\n",pTypeName,virtfuncnum);
        }
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
    DetourVirtualFuncTable(&st_DetourCS,&st_StreamAudioVolumeReleaseDetoured,(void**)&StreamAudioVolumeReleaseNext,StreamAudioVolumeReleaseCallBack,pStream,STREAM_AUDIO_VOLUME_RELEASE_NUM,"AudioStreamVolume");
    DetourVirtualFuncTable(&st_DetourCS,&st_StreamAudioVolumeSetChannelVolumeDetoured,(void**)&StreamAudioVolumeSetChannelVolumeNext,StreamAudioVolumeSetChannelVolumeCallBack,pStream,STREAM_AUDIO_VOLUME_SET_CHANNEL_VOLUME_NUM,"AudioStreamVolume");
    DetourVirtualFuncTable(&st_DetourCS,&st_StreamAudioVolumeSetAllVolumesDetoured,(void**)&StreamAudioVolumeSetAllVolumesNext,StreamAudioVolumeSetAllVolumesCallBack,pStream,STREAM_AUDIO_VOLUME_SET_ALL_VOLUMES_NUM,"AudioStreamVolume");
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
    DetourVirtualFuncTable(&st_DetourCS,&st_ChannelAudioVolumeReleaseDetoured,(void**)&ChannelAudioVolumeReleaseNext,ChannelAudioVolumeReleaseCallBack,pChannel,CHANNEL_AUDIO_VOLUME_RELEASE_NUM,"ChannelAudioVolume");
    DetourVirtualFuncTable(&st_DetourCS,&st_ChannelAudioVolumeSetChannelVolumeDetoured,(void**)&ChannelAudioVolumeSetChannelVolumeNext,ChannelAudioVolumeSetChannelVolumeCallBack,pChannel,CHANNEL_AUDIO_VOLUME_SET_CHANNEL_VOLUME_NUM,"ChannelAudioVolume");
    DetourVirtualFuncTable(&st_DetourCS,&st_ChannelAudioVolumeSetAllVolumesDetoured,(void**)&ChannelAudioVolumeSetAllVolumesNext,ChannelAudioVolumeSetAllVolumesCallBack,pChannel,CHANNEL_AUDIO_VOLUME_SET_ALL_VOLUMES_NUM,"ChannelAudioVolume");
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
    DetourVirtualFuncTable(&st_DetourCS,&st_SimpleAudioVolumeReleaseDetoured,(void**)&SimpleAudioVolumeReleaseNext,SimpleAudioVolumeReleaseCallBack,pThis,SIMPLE_AUDIO_VOLUME_RELEASE_NUM,"SimpleAudioVolume");
    DetourVirtualFuncTable(&st_DetourCS,&st_SimpleAudioVolumeSetMasterVolumeDetoured,(void**)&SimpleAudioVolumeSetMasterVolumeNext,SimpleAudioVolumeSetMasterVolumeCallBack,pThis,SIMPLE_AUDIO_VOLUME_SET_MASTER_VOLUME_NUM,"SimpleAudioVolume");
    return 0;
}

/*****************************************************
* audio render client register and buffer set
*****************************************************/
static CRITICAL_SECTION st_RenderCS;
static int st_RenderNotSet=0;
static std::vector<IAudioRenderClient*> st_RenderArrays;
static std::vector<unsigned char*> st_RenderBufferArrays;
static std::vector<WAVEFORMATEX*> st_RenderFormatArrays;

#define RENDER_BUFFER_ASSERT()  \
do\
{\
	assert(st_RenderArrays.size() == st_RenderBufferArrays.size());\
	assert( st_RenderBufferArrays.size() == st_RenderFormatArrays.size());\
}while(0)

static unsigned char* GetReleaseBufferPointer(IAudioRenderClient* pRender)
{
    unsigned char* pPointer=NULL;
    unsigned int i;
    int findidx=-1;
    EnterCriticalSection(&st_RenderCS);
    RENDER_BUFFER_ASSERT();
    for(i=0; i<st_RenderArrays.size(); i++)
    {
        if(pRender == st_RenderArrays[i])
        {
            findidx = i;
            pPointer = st_RenderBufferArrays[i];
            st_RenderBufferArrays[i] = NULL;
            break;
        }
    }
    LeaveCriticalSection(&st_RenderCS);

    if(findidx >= 0 && pPointer == NULL)
    {
        ERROR_INFO("Render 0x%p releasebuffer null\n",pRender);
    }
    return pPointer;
}

static int SetGetBufferPointer(IAudioRenderClient* pRender,unsigned char* pBuffer)
{
    int ret =0;
    int findidx=-1;
    unsigned int i;
    unsigned char* pOldPointer=NULL;
    EnterCriticalSection(&st_RenderCS);
    for(i=0; i<st_RenderArrays.size() ; i++)
    {
        if(pRender == st_RenderArrays[i])
        {
            findidx = i;
            pOldPointer = st_RenderBufferArrays[i];
            st_RenderBufferArrays[i] = pBuffer;
            break;
        }
    }

    if(findidx < 0)
    {
        st_RenderArrays.push_back(pRender);
        st_RenderBufferArrays.push_back(pBuffer);
        st_RenderFormatArrays.push_back(NULL);
        ret = 1;
    }
    RENDER_BUFFER_ASSERT();
    LeaveCriticalSection(&st_RenderCS);

    if(ret > 0)
    {
        DEBUG_INFO("Insert New Render 0x%p\n",pRender);
    }

    if(pOldPointer)
    {
        ERROR_INFO("render 0x%p old renderbuffer 0x%p\n",pRender,pOldPointer);
    }
    return ret;
}


static int ReleaseRenderClient(IAudioRenderClient* pRender)
{
    int ret=0;
    int findidx = -1;
    unsigned int i;
    unsigned char* pOldPointer=NULL;
    WAVEFORMATEX* pFormatEx=NULL;
    EnterCriticalSection(&st_RenderCS);
    for(i=0; i<st_RenderArrays.size(); i++)
    {
        if(pRender == NULL ||pRender == st_RenderArrays[i])
        {
            findidx = i;
            break;
        }
    }

    if(findidx >=0)
    {
        st_RenderArrays.erase(st_RenderArrays.begin() + findidx);
        pOldPointer = st_RenderBufferArrays[findidx];
        st_RenderBufferArrays.erase(st_RenderBufferArrays.begin() + findidx);
        pFormatEx = st_RenderFormatArrays[findidx];
        st_RenderFormatArrays.erase(st_RenderFormatArrays.begin() + findidx);
        ret = 1;
    }
    RENDER_BUFFER_ASSERT();
    LeaveCriticalSection(&st_RenderCS);

    if(pOldPointer)
    {
        ERROR_INFO("release Render(0x%p) oldpointer 0x%p not null\n",pRender,pOldPointer);
    }

    if(pFormatEx)
    {
        free(pFormatEx);
    }
    else
    {
        ERROR_INFO("Render 0x%p not has format\n",pRender);
    }
    pFormatEx = NULL;
    DEBUG_INFO("release RenderClient 0x%p\n",pRender);
    return ret;
}

static int InitializeRenderFormat(IAudioRenderClient* pRender)
{
    WAVEFORMATEX* pFormatEx=NULL;
    int ret=0;
    unsigned int size=sizeof(*pFormatEx);
    int findidx=-1;
    unsigned int i;

    pFormatEx = GetCopiedFormat();
    EnterCriticalSection(&st_RenderCS);
    RENDER_BUFFER_ASSERT();
    for(i=0; st_RenderArrays.size(); i++)
    {
        if(pRender == st_RenderArrays[i])
        {
            findidx = i;
            break;
        }
    }

    if(findidx < 0)
    {
        st_RenderArrays.push_back(pRender);
        st_RenderBufferArrays.push_back(NULL);
        st_RenderFormatArrays.push_back(pFormatEx);
        ret = 1;
    }
    LeaveCriticalSection(&st_RenderCS);

    if(ret == 0)
    {
        /*not inserted ,so we should remove it */
        free(pFormatEx);
        pFormatEx = NULL;
    }

    return ret;
}

static void RemoveAllRenders()
{
    int ret;

    while(1)
    {
        ret = ReleaseRenderClient(NULL);
        if(ret == 0)
        {
            break;
        }
    }

    return ;
}

static int GetFormat(IAudioRenderClient* pRender,PCM_AUDIO_FORMAT_t *pAudioFormat)
{
    int ret=0;
    WAVEFORMATEX* pFormat=NULL;
    unsigned int i;
    int size=sizeof(*pFormat);
    int findidx=-1;
    EnterCriticalSection(&st_RenderCS);

    for(i=0; i<st_RenderArrays.size(); i++)
    {
        if(pRender == st_RenderArrays[i])
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        pFormat = st_RenderFormatArrays[findidx];
        size += pFormat->cbSize;
        if(size >= sizeof(pAudioFormat->m_Format))
        {
            memcpy(pAudioFormat->m_Format,pFormat,sizeof(pAudioFormat->m_Format));
        }
        else
        {
            memcpy(pAudioFormat->m_Format,pFormat,size);
        }

        ret = 1;
    }
    LeaveCriticalSection(&st_RenderCS);

    if(ret > 0)
    {
        EnterCriticalSection(&st_StateCS);
        pAudioFormat->m_Volume = st_AudioFormat.m_Volume;
        LeaveCriticalSection(&st_StateCS);
    }
    return ret;
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
    int ret;

    uret = AudioRenderClientReleaseNext(pRender);
    if(uret == 0)
    {
        ret = ReleaseRenderClient(pRender);
        if(ret == 0)
        {
            ERROR_INFO("not set client 0x%p into the array\n",pRender);
        }
    }
    return uret;
}

typedef HRESULT(WINAPI *AudioRenderClientGetBufferFunc_t)(IAudioRenderClient* pRender,UINT32 NumFramesRequested,BYTE **ppData);
static AudioRenderClientGetBufferFunc_t AudioRenderClientGetBufferNext=NULL;
static int st_AudioRenderClientGetBufferDetoured=0;


HRESULT WINAPI AudioRenderClientGetBufferCallBack(IAudioRenderClient* pRender,UINT32 NumFramesRequested,BYTE **ppData)
{
    HRESULT hr;
    hr = AudioRenderClientGetBufferNext(pRender,NumFramesRequested,ppData);
    if(SUCCEEDED(hr))
    {
        SetGetBufferPointer(pRender,*ppData);
        //DEBUG_INFO("render get buffer 0x%p numframe requested %d\n",*ppData,NumFramesRequested);
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
    unsigned char* pBuffer=NULL;

    operation = GetOperation();
    if(operation == PCMCAPPER_OPERATION_NONE ||
            operation == PCMCAPPER_OPERATION_CAPTURE)
    {
        newdwflag |= AUDCLNT_BUFFERFLAGS_SILENT;
    }
    pBuffer = GetReleaseBufferPointer(pRender);
    if(pBuffer)
    {
        /*write buffer */
        if(!(dwFlags & AUDCLNT_BUFFERFLAGS_SILENT) && (operation == PCMCAPPER_OPERATION_CAPTURE || operation == PCMCAPPER_OPERATION_BOTH))
        {
            WriteSendBuffer(pRender,pBuffer,NumFramesWritten);
        }
    }

    //DEBUG_INFO("NumFramesWritten %d operation %d\n",NumFramesWritten,operation);
    hr = AudioRenderClientReleaseBufferNext(pRender,NumFramesWritten,newdwflag);
    return hr;
}

static int DetourAudioRenderClientVirtFunctions(IAudioRenderClient *pRender)
{
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioRenderClientReleaseDetoured,(void**)&AudioRenderClientReleaseNext,AudioRenderClientReleaseCallBack,pRender,AUDIO_RENDER_RELEASE_NUM,"AudioRenderClient");
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioRenderClientGetBufferDetoured,(void**)&AudioRenderClientGetBufferNext,AudioRenderClientGetBufferCallBack,pRender,AUDIO_RENDER_GET_BUFFER_NUM,"AudioRenderClient");
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioRenderClientReleaseBufferDetoured,(void**)&AudioRenderClientReleaseBufferNext,AudioRenderClientReleaseBufferCallBack,pRender,AUDIO_RENDER_RELEASE_BUFFER_NUM,"AudioRenderClient");
    return 0;
}

/*****************************************************
* audio client Interface replace
*****************************************************/
#if 0
#define  AUDIO_CLIENT_IN() do{DEBUG_INFO("AUDIO CLIENT IN\n");}while(0)
#define  AUDIO_CLIENT_OUT()


static CIAudioClientHook* RegisterAudioClient(IAudioClient* pAudio);
static ULONG UnRegisterAudioClient(IAudioClient* pAudio);


class CIAudioClientHook  : public IAudioClient
{
private:
    IAudioClient* m_ptr;
public:
    CIAudioClientHook(IAudioClient* ptr): m_ptr(ptr) {};
public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_ REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG uret;
        AUDIO_CLIENT_IN();
        uret = m_ptr->AddRef();
        AUDIO_CLIENT_OUT();
        return uret;
    }

    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG uret;
        AUDIO_CLIENT_IN();
        uret = m_ptr->Release();
        if(uret == 1)
        {
            uret = UnRegisterAudioClient(m_ptr);
            if(uret == 0)
            {
                this->m_ptr = NULL;
            }
        }
        AUDIO_CLIENT_OUT();
        if(uret == 0)
        {
            delete this;
        }
        return uret;
    }


    COM_METHOD(HRESULT,Initialize)(THIS_  AUDCLNT_SHAREMODE ShareMode,DWORD StreamFlags,REFERENCE_TIME hnsBufferDuration,
                                   REFERENCE_TIME hnsPeriodicity,const WAVEFORMATEX *pFormat,LPCGUID AudioSessionGuid)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->Initialize(ShareMode,StreamFlags,hnsBufferDuration,hnsPeriodicity,pFormat,AudioSessionGuid);
        if(SUCCEEDED(hr))
        {
            SetFormat((WAVEFORMATEX*)pFormat);
        }
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetBufferSize)(THIS_ UINT32 *pNumBufferFrames)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetBufferSize(pNumBufferFrames);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetStreamLatency)(THIS_	REFERENCE_TIME *phnsLatency)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetStreamLatency(phnsLatency);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetCurrentPadding)(THIS_  UINT32 *pNumPaddingFrames)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetCurrentPadding(pNumPaddingFrames);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,IsFormatSupported)(THIS_  AUDCLNT_SHAREMODE ShareMode,const WAVEFORMATEX *pFormat,WAVEFORMATEX **ppClosestMatch)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->IsFormatSupported(ShareMode,pFormat,ppClosestMatch);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetMixFormat)(THIS_	WAVEFORMATEX **ppDeviceFormat)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetMixFormat(ppDeviceFormat);
        if(SUCCEEDED(hr))
        {
            WAVEFORMATEX* pFormat= (*ppDeviceFormat);
            SetFormat((WAVEFORMATEX*)pFormat);
        }
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDevicePeriod)(THIS_  REFERENCE_TIME *phnsDefaultDevicePeriod,REFERENCE_TIME *phnsMinimumDevicePeriod)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetDevicePeriod(phnsDefaultDevicePeriod,phnsMinimumDevicePeriod);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Start)(THIS)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->Start();
        if(SUCCEEDED(hr))
        {
            NotifyAudioStart();
        }
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Stop)(THIS)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->Stop();
        if(SUCCEEDED(hr))
        {
            NotifyAudioStop();
        }
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Reset)(THIS)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->Reset();
        if(SUCCEEDED(hr))
        {
            NotifyAudioStop();
            NotifyAudioStart();
        }
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetEventHandle)(THIS_ HANDLE eventHandle)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->SetEventHandle(eventHandle);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetService)(THIS_  REFIID riid,void **ppv)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetService(riid,ppv);
        if(SUCCEEDED(hr))
        {
            if(riid == __uuidof(IAudioRenderClient))
            {
                IAudioRenderClient* pRender = (IAudioRenderClient*)*ppv;
                DEBUG_INFO("\n");
                DetourAudioRenderClientVirtFunctions(pRender);
            }
            else if(riid == __uuidof(IAudioStreamVolume))
            {
                IAudioStreamVolume* pStream= (IAudioStreamVolume*)*ppv;
                DEBUG_INFO("\n");
                DetourStreamAudioVolumeVirtFunctions(pStream);
            }
            else if(riid == __uuidof(IChannelAudioVolume))
            {
                IChannelAudioVolume* pChannel = (IChannelAudioVolume*)*ppv;
                DEBUG_INFO("\n");
                DetourChannelAudioVolumeVirtFunctions(pChannel);
            }
            else if(riid == __uuidof(ISimpleAudioVolume))
            {
                ISimpleAudioVolume* pSimple = (ISimpleAudioVolume*)*ppv;
                DEBUG_INFO("\n");
                DetourSimpleAudioVolumeVirtFunctions(pSimple);
            }
        }
        AUDIO_CLIENT_OUT();
        return hr;
    }
};

static std::vector<IAudioClient*> st_AudioClientVecs;
static std::vector<CIAudioClientHook*> st_AudioClientHookVecs;
static CRITICAL_SECTION st_AudioClientCS;

static CIAudioClientHook* RegisterAudioClient(IAudioClient * pAudio)
{
    CIAudioClientHook* pAudioHook=NULL;
    int findidx = -1;
    unsigned int i;
    static int st_DetouredAudioClient=0;

    EnterCriticalSection(&st_AudioClientCS);
    if(st_DetouredAudioClient == 0)
    {
        SetDetourAudioClient(pAudio);
        st_DetouredAudioClient = 1;
    }

    assert(st_AudioClientHookVecs.size() ==  st_AudioClientVecs.size());
    for(i=0; i<st_AudioClientVecs.size() ; i++)
    {
        if(st_AudioClientVecs[i] == pAudio)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        pAudioHook = st_AudioClientHookVecs[findidx];
    }
    else
    {
        pAudioHook = new CIAudioClientHook(pAudio);
        st_AudioClientHookVecs.push_back(pAudioHook);
        st_AudioClientVecs.push_back(pAudio);
        pAudio->AddRef();
    }

    LeaveCriticalSection(&st_AudioClientCS);
    return pAudioHook;
}

static ULONG UnRegisterAudioClient(IAudioClient * pAudio)
{
    int findidx = -1;
    unsigned int i;
    ULONG uret;

    EnterCriticalSection(&st_AudioClientCS);

    assert(st_AudioClientHookVecs.size() ==  st_AudioClientVecs.size());
    for(i=0; i<st_AudioClientVecs.size() ; i++)
    {
        if(st_AudioClientVecs[i] == pAudio)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        st_AudioClientHookVecs.erase(st_AudioClientHookVecs.begin() + findidx);
        st_AudioClientVecs.erase(st_AudioClientVecs.begin() + findidx);
    }

    LeaveCriticalSection(&st_AudioClientCS);

    uret = 1;
    if(findidx >= 0)
    {
        uret = pAudio->Release();
    }
    return uret;
}
#endif

/*****************************************************
* audio client detour functions
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
    if(uret == 0)
    {
        DEBUG_INFO("AudioClient 0x%p uret %d\n",pClient,uret);
    }
    return uret;
}


typedef HRESULT(WINAPI *AudioClientInitializeFunc_t)(IAudioClient* pClient,AUDCLNT_SHAREMODE ShareMode,DWORD StreamFlags,REFERENCE_TIME hnsBufferDuration,REFERENCE_TIME hnsPeriodicity,const WAVEFORMATEX *pFormat,LPCGUID AudioSessionGuid);
static AudioClientInitializeFunc_t AudioClientInitializeNext=NULL;
static int st_AudioClientInitializeDetoured=0;

HRESULT WINAPI AudioClientInitializeCallBack(IAudioClient* pClient,AUDCLNT_SHAREMODE ShareMode,DWORD StreamFlags,REFERENCE_TIME hnsBufferDuration,REFERENCE_TIME hnsPeriodicity,const WAVEFORMATEX *pFormat,LPCGUID AudioSessionGuid)
{
    HRESULT hr;
    DEBUG_INFO("\n");
    hr = AudioClientInitializeNext(pClient,ShareMode,StreamFlags,hnsBufferDuration,hnsPeriodicity,pFormat,AudioSessionGuid);
    if(SUCCEEDED(hr) && pFormat)
    {
        DEBUG_INFO("AudioClient 0x%p sharemode %d streamflags %d\n",pClient,ShareMode,StreamFlags);
        SetFormat((WAVEFORMATEX*)pFormat);
    }
    return hr;
}

typedef HRESULT(WINAPI  *AudioClientStartFunc_t)(IAudioClient* pClient);
static AudioClientStartFunc_t AudioClientStartNext=NULL;
static int st_AudioClientStartDetoured=0;

HRESULT WINAPI AudioClientStartCallBack(IAudioClient* pClient)
{
    HRESULT hr;
    DEBUG_INFO("start in\n");
    hr = AudioClientStartNext(pClient);
    if(SUCCEEDED(hr))
    {
        DEBUG_INFO("AudioClient 0x%p start succ\n",pClient);
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
    DEBUG_INFO("stop in\n");
    hr = AudioClientStopNext(pClient);
    if(SUCCEEDED(hr))
    {
        DEBUG_INFO("AudioClient 0x%p stop succ\n",pClient);
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
        DEBUG_INFO("AudioClient 0x%p Reset\n",pClient);
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
    DEBUG_INFO("\n");
    hr = AudioClientGetServiceNext(pClient,riid,ppv);
    if(SUCCEEDED(hr))
    {
        if(riid == __uuidof(IAudioRenderClient))
        {
            IAudioRenderClient* pRender = (IAudioRenderClient*)*ppv;
            DEBUG_INFO("AudioClient 0x%p Render 0x%p\n",pClient,pRender);
            DetourAudioRenderClientVirtFunctions(pRender);
            InitializeRenderFormat(pRender);
        }
        else if(riid == __uuidof(IAudioStreamVolume))
        {
            IAudioStreamVolume* pStream= (IAudioStreamVolume*)*ppv;
            DEBUG_INFO("AudioClient 0x%p AudioStreamVolume 0x%p\n",pClient,pStream);
            DetourStreamAudioVolumeVirtFunctions(pStream);
        }
        else if(riid == __uuidof(IChannelAudioVolume))
        {
            IChannelAudioVolume* pChannel = (IChannelAudioVolume*)*ppv;
            DEBUG_INFO("AudioClient 0x%p ChannelAudioVolume 0x%p\n",pClient,pChannel);
            DetourChannelAudioVolumeVirtFunctions(pChannel);
        }
        else if(riid == __uuidof(ISimpleAudioVolume))
        {
            ISimpleAudioVolume* pSimple = (ISimpleAudioVolume*)*ppv;
            DEBUG_INFO("AudioClient 0x%p SimpleAudioVolume 0x%p\n",pClient,pSimple);
            DetourSimpleAudioVolumeVirtFunctions(pSimple);
        }
    }
    return hr;
}

static int DetourDeviceAudioClientVirtFunctions(IAudioClient* pClient)
{
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientReleaseDetoured,(void**)&AudioClientReleaseNext,AudioClientReleaseCallBack,pClient,AUDIO_CLIENT_RELEASE_NUM,"AudioClient");
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientInitializeDetoured,(void**)&AudioClientInitializeNext,AudioClientInitializeCallBack,pClient,AUDIO_CLIENT_INITIALIZE_NUM,"AudioClient");
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientStartDetoured,(void**)&AudioClientStartNext,AudioClientStartCallBack,pClient,AUDIO_CLIENT_START_NUM,"AudioClient");
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientStopDetoured,(void**)&AudioClientStopNext,AudioClientStopCallBack,pClient,AUDIO_CLIENT_STOP_NUM,"AudioClient");
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientResetDetoured,(void**)&AudioClientResetNext,AudioClientResetCallBack,pClient,AUDIO_CLIENT_RESET_NUM,"AudioClient");
    DetourVirtualFuncTable(&st_DetourCS,&st_AudioClientGetServiceDetoured,(void**)&AudioClientGetServiceNext,AudioClientGetServiceCallBack,pClient,AUDIO_CLIENT_GET_SERVICE_NUM,"AudioClient");
    return 0;
}



/*****************************************************
* device interface replace
*****************************************************/

#define MMDEVICE_IN()
#define MMDEVICE_OUT()

static ULONG UnRegisterMMDevice(IMMDevice* pDevice);

class CIMMDeviceHook : public IMMDevice
{
private:
    IMMDevice *m_ptr;
public:
    CIMMDeviceHook(IMMDevice* ptr) : m_ptr(ptr) {};
public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_  REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        MMDEVICE_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        MMDEVICE_OUT();
        return hr;
    }

    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG uret;
        MMDEVICE_IN();
        uret = m_ptr->AddRef();
        MMDEVICE_OUT();
        return uret;
    }

    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG uret;
        MMDEVICE_IN();
        uret = m_ptr->Release();
        if(uret == 1)
        {
            uret = UnRegisterMMDevice(m_ptr);
            if(uret == 0)
            {
                this->m_ptr=NULL;
            }
        }
        MMDEVICE_OUT();
        if(uret == 0)
        {
            delete this;
        }
        return uret;
    }

    COM_METHOD(HRESULT,Activate)(THIS_ REFIID iid,DWORD dwClsCtx,PROPVARIANT *pActivationParams,void **ppInterface)
    {
        HRESULT hr;
        MMDEVICE_IN();
        hr = m_ptr->Activate(iid,dwClsCtx,pActivationParams,ppInterface);
        if(SUCCEEDED(hr))
        {
            if(iid == __uuidof(IAudioClient))
            {
                IAudioClient* pClient=(IAudioClient*)*ppInterface;
                DEBUG_INFO("Hook(0x%p) 0x%p active audio client 0x%p\n",this,m_ptr,pClient);
                DetourDeviceAudioClientVirtFunctions(pClient);
            }
        }
        MMDEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,OpenPropertyStore)(THIS_ DWORD stgmAccess,IPropertyStore **ppProperties)
    {
        HRESULT hr;
        MMDEVICE_IN();
        hr = m_ptr->OpenPropertyStore(stgmAccess,ppProperties);
        MMDEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetId)(THIS_ LPWSTR *ppstrId)
    {
        HRESULT hr;
        MMDEVICE_IN();
        hr = m_ptr->GetId(ppstrId);
        MMDEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetState)(THIS_ DWORD *pdwState)
    {
        HRESULT hr;
        MMDEVICE_IN();
        hr = m_ptr->GetState(pdwState);
        MMDEVICE_OUT();
        return hr;
    }


};


static std::vector<IMMDevice*> st_MMDeviceVecs;
static std::vector<CIMMDeviceHook*> st_MMDeviceHookVecs;
static CRITICAL_SECTION st_MMDevCS;



static CIMMDeviceHook* RegisterMMDevice(IMMDevice* pDevice)
{
    CIMMDeviceHook* pDevHook=NULL;
    int findidx = -1;
    unsigned int i;
    EnterCriticalSection(&st_MMDevCS);
    assert(st_MMDeviceVecs.size() == st_MMDeviceHookVecs.size());
    for(i=0; i<st_MMDeviceVecs.size(); i++)
    {
        if(st_MMDeviceVecs[i] == pDevice)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        pDevHook = st_MMDeviceHookVecs[findidx];
    }
    else
    {
        pDevHook = new CIMMDeviceHook(pDevice);
        st_MMDeviceVecs.push_back(pDevice);
        st_MMDeviceHookVecs.push_back(pDevHook);
        assert(pDevHook);
        pDevice->AddRef();
    }

    LeaveCriticalSection(&st_MMDevCS);

    return pDevHook;

}

static ULONG UnRegisterMMDevice(IMMDevice* pDevice)
{
    int findidx=-1;
    ULONG uret;
    unsigned int i;
    CIMMDeviceHook* pDevHook=NULL;
    EnterCriticalSection(&st_MMDevCS);
    for(i=0; i<st_MMDeviceVecs.size(); i++)
    {
        if(st_MMDeviceVecs[i] == pDevice)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        pDevHook = st_MMDeviceHookVecs[findidx];
        st_MMDeviceHookVecs.erase(st_MMDeviceHookVecs.begin()+findidx);
        st_MMDeviceVecs.erase(st_MMDeviceVecs.begin() + findidx);
    }

    LeaveCriticalSection(&st_MMDevCS);
    uret = 1;
    if(findidx >= 0)
    {
        uret = pDevice->Release();
    }

    return uret;
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
    //DEBUG_INFO("uret %d\n",uret);
    return uret;
}

typedef HRESULT(WINAPI *DeviceActivateFunc_t)(IMMDevice *pThis,REFIID iid,DWORD dwClsCtx,PROPVARIANT *pActivationParams,void **ppInterface);
static DeviceActivateFunc_t DeviceActivateNext=NULL;
static int st_DeviceActivateDetoured=0;

HRESULT WINAPI DeviceActivateCallBack(IMMDevice *pThis,REFIID iid,DWORD dwClsCtx,PROPVARIANT *pActivationParams,void **ppInterface)
{
    HRESULT hr;
    //DEBUG_INFO("\n");
    hr = DeviceActivateNext(pThis,iid,dwClsCtx,pActivationParams,ppInterface);
    if(SUCCEEDED(hr))
    {
        //DEBUG_INFO("iid %s\n",iid);
        LPOLESTR test=NULL;
        StringFromCLSID(iid, &test);
        DEBUG_INFO("%ws\n", test);
        CoTaskMemFree(test);
        if(iid == __uuidof(IAudioClient))
        {
            IAudioClient* pClient=(IAudioClient*)*ppInterface;
            DEBUG_INFO("active audio client\n");
            DetourDeviceAudioClientVirtFunctions(pClient);
        }
    }
    else
    {
        LPOLESTR test=NULL;
        StringFromCLSID(iid, &test);
        DEBUG_INFO("%ws\n", test);
        CoTaskMemFree(test);
        ERROR_INFO("error(0x%x)\n",hr);
    }
    return hr;
}



static int DetourDeviceVirtFunctions(IMMDevice* pDevice)
{
    DetourVirtualFuncTable(&st_DetourCS,&st_DeviceReleaseDetoured,(void**)&DeviceReleaseNext,DeviceReleaseCallBack,pDevice,DEVICE_RELEASE_NUM,"MMDevice");
    DetourVirtualFuncTable(&st_DetourCS,&st_DeviceActivateDetoured,(void**)&DeviceActivateNext,DeviceActivateCallBack,pDevice,DEVICE_ACTIVATE_NUM,"MMDevice");
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
    //DEBUG_INFO("uret %d\n",uret);
    if(uret == 0)
    {
        DEBUG_INFO("Collection 0x%p released\n",pThis);
    }
    return uret;
}

typedef HRESULT(WINAPI *DeviceCollectionItemFunc_t)(IMMDeviceCollection* pThis,UINT nDevice,IMMDevice **ppDevice);
static DeviceCollectionItemFunc_t DeviceCollectionItemNext=NULL;
static int st_DeviceCollectionItemDetoured=0;

HRESULT WINAPI DeviceCollectionItemCallBack(IMMDeviceCollection* pThis,UINT nDevice,IMMDevice **ppDevice)
{
    HRESULT hr;
    //DEBUG_INFO("\n");
    hr = DeviceCollectionItemNext(pThis,nDevice,ppDevice);
    if(SUCCEEDED(hr))
    {
        //DEBUG_INFO("\n");
        CIMMDeviceHook *pHook=NULL;
        IMMDevice* pDevice=*ppDevice;
        pHook = RegisterMMDevice(*ppDevice);
        *ppDevice = pHook;
        DEBUG_INFO("Collection 0x%p Item 0x%p (Hook 0x%p)\n",pThis,pDevice,pHook);
        //DetourDeviceVirtFunctions(*ppDevice);
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
    DetourVirtualFuncTable(&st_DetourCS,&st_DeviceCollectionReleaseDetoured,(void**)&DeviceCollectionReleaseNext,DeviceCollectionReleaseCallBack,pCollection,DEVICE_COLLECTION_RELEASE_NUM,"MMDeviceCollection");
    DetourVirtualFuncTable(&st_DetourCS,&st_DeviceCollectionItemDetoured,(void**)&DeviceCollectionItemNext,DeviceCollectionItemCallBack,pCollection,DEVICE_COLLECTION_ITEM_NUM,"MMDeviceCollection");
    return 0;
}

ULONG WINAPI EnumeratorReleaseCallBack(IMMDeviceEnumerator* pThis)
{
    ULONG uret;
    //DEBUG_INFO("\n");
    uret = EnumeratorReleaseNext(pThis);
    if(uret == 0)
    {
        DEBUG_INFO("Enumerator 0x%p uret %d\n",pThis,uret);
    }
    return uret;
}

typedef HRESULT(WINAPI *EnumeratorEnumAudioEndpointsFunc_t)(IMMDeviceEnumerator* pThis,EDataFlow dataFlow,DWORD dwStateMask,IMMDeviceCollection **ppDevices);
static EnumeratorEnumAudioEndpointsFunc_t EnumeratorEnumAudioEndpointsNext=NULL;
static int st_EnumeratorEnumAudioEndpointsDetoured=0;

HRESULT WINAPI EnumeratorEnumAudioEndpointsCallBack(IMMDeviceEnumerator* pThis,EDataFlow dataFlow,DWORD dwStateMask,IMMDeviceCollection **ppDevices)
{
    HRESULT hr;
    //DEBUG_INFO("\n");
    hr = EnumeratorEnumAudioEndpointsNext(pThis,dataFlow,dwStateMask,ppDevices);
    if(SUCCEEDED(hr))
    {
        /*now success ,so we should to detour for the function of device collection*/
        DEBUG_INFO("Enumerator 0x%p flow %d mask %d EnumAudioEndpoints collection 0x%p\n",pThis,dataFlow,dwStateMask,*ppDevices);
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
    //DEBUG_INFO("\n");
    hr = EnumeratorGetDefaultAudioEndpointNext(pThis,dataFlow,role,ppEndpoint);
    if(SUCCEEDED(hr))
    {
        //DEBUG_INFO("\n");
        CIMMDeviceHook *pHook=NULL;
        IMMDevice* pDevice=*ppEndpoint;
        pHook = RegisterMMDevice(pDevice);
        *ppEndpoint= pHook;
        DEBUG_INFO("Enumerator 0x%p GetDefaultAudioEndpoint 0x%p (Hook 0x%p)\n",pThis,pDevice,pHook);
        //DetourDeviceVirtFunctions(*ppEndpoint);
    }
    return hr;
}

typedef HRESULT(WINAPI *EnumeratorGetDeviceFunc_t)(IMMDeviceEnumerator* pThis,LPCWSTR pwstrId,IMMDevice **ppDevice);
static EnumeratorGetDeviceFunc_t EnumeratorGetDeviceNext=NULL;
static int st_EnumeratorGetDeviceDetoured=0;

HRESULT WINAPI EnumeratorGetDeviceCallBack(IMMDeviceEnumerator* pThis,LPCWSTR pwstrId,IMMDevice **ppDevice)
{
    HRESULT hr;
    //DEBUG_INFO("\n");
    hr = EnumeratorGetDeviceNext(pThis,pwstrId,ppDevice);
    if(SUCCEEDED(hr))
    {
        //DEBUG_INFO("\n");
        CIMMDeviceHook *pHook=NULL;
        IMMDevice* pDevice=*ppDevice;
        pHook = RegisterMMDevice(pDevice);
        *ppDevice = pHook;
        DEBUG_INFO("Enumerator 0x%p GetDevice 0x%p (Hook 0x%p)\n",pThis,pDevice,pHook);
        //DetourDeviceVirtFunctions(*ppDevice);
    }
    return hr;
}


static int DetourEnumeratorVirtFunctions(IMMDeviceEnumerator* pEnumerator)
{
    DetourVirtualFuncTable(&st_DetourCS,&st_EnumeratorReleaseDetoured,(void**)&EnumeratorReleaseNext,EnumeratorReleaseCallBack,pEnumerator,ENUMERATOR_RELEASE_NUM,"MMDeviceEnumerator");
    DetourVirtualFuncTable(&st_DetourCS,&st_EnumeratorEnumAudioEndpointsDetoured,(void**)&EnumeratorEnumAudioEndpointsNext,EnumeratorEnumAudioEndpointsCallBack,pEnumerator,ENUMERATOR_ENUM_AUDIO_ENDPOINTS_NUM,"MMDeviceEnumerator");
    DetourVirtualFuncTable(&st_DetourCS,&st_EnumeratorGetDefaultAudioEndpointDetoured,(void**)&EnumeratorGetDefaultAudioEndpointNext,EnumeratorGetDefaultAudioEndpointCallBack,pEnumerator,ENUMERATOR_GET_DEFAULT_AUDIO_ENDPOINT_NUM,"MMDeviceEnumerator");
    DetourVirtualFuncTable(&st_DetourCS,&st_EnumeratorGetDeviceDetoured,(void**)&EnumeratorGetDeviceNext,EnumeratorGetDeviceCallBack,pEnumerator,ENUMERATOR_GET_DEVICE_NUM,"MMDeviceEnumerator");
    return 0;
}


LONG WINAPI DetourApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
    StackWalker sw;
    EXCEPTION_RECORD *xr = pException->ExceptionRecord;
    CONTEXT *xc = pException->ContextRecord;
    DEBUG_INFO("Eip 0x%08x\n",xc->Eip);
    sw.ShowCallstack(GetCurrentThread(), pException->ContextRecord,NULL,NULL);
    return EXCEPTION_EXECUTE_HANDLER;
}

static std::vector<unsigned char*> st_InsertDllFullNames;
static std::vector<unsigned char*> st_InsertDllPartNames;
static CRITICAL_SECTION st_DllNameCS;

#define INSERT_DLL_NAME_ASSERT() \
do\
{\
	assert(st_InsertDllFullNames.size() == st_InsertDllPartNames.size());\
}while(0)

static int InsertDllNames(const char* pFullName,const char* pPartName)
{
    int ret=0;
    int findidx=-1;
    unsigned int i;
    char *pAllocFullName=NULL,*pAllocPartName=NULL;

    pAllocFullName = strdup(pFullName);
    pAllocPartName = strdup(pPartName);
    if(pAllocFullName == NULL || pAllocPartName == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    EnterCriticalSection(&st_DllNameCS);

    INSERT_DLL_NAME_ASSERT();
    for(i=0; i<st_InsertDllPartNames.size(); i++)
    {
        if(strcmp(pAllocPartName,st_InsertDllPartNames[i])==0)
        {
            findidx = i;
            break;
        }
    }

    if(findidx < 0)
    {
        st_InsertDllFullNames.push_back(pAllocFullName);
        st_InsertDllPartNames.push_back(pAllocPartName);
        ret = 1;
    }

    LeaveCriticalSection(&st_DllNameCS);

    if(ret == 0)
    {
        assert(pAllocFullName && pAllocPartName);
        free(pAllocFullName);
        pAllocFullName = NULL;
        free(pAllocPartName);
        pAllocPartName = NULL;
    }

    return ret;

fail:
    if(pAllocFullName)
    {
        free(pAllocFullName);
    }
    pAllocFullName = NULL;

    if(pAllocPartName)
    {
        free(pAllocPartName);
    }
    pAllocPartName = NULL;
    SetLastError(ret);
    return -ret;
}

static int ClearDllNames(const char* pPartName)
{
    char* pFreePartName=NULL;
    char* pFreeFullName=NULL;
    int findidx=-1;
    unsigned int i;
    int ret =0;

    EnterCriticalSection(&st_DllNameCS);
    INSERT_DLL_NAME_ASSERT();
    for(i=0; i<st_InsertDllPartNames.size(); i++)
    {
        if(pPartName == NULL || strcmp(pPartName,st_InsertDllPartNames[i])==0)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >=0)
    {
        pFreePartName = st_InsertDllPartNames[findidx];
        pFreeFullName = st_InsertDllFullNames[findidx];
        st_InsertDllFullNames.erase(st_InsertDllFullNames.begin() + findidx);
        st_InsertDllPartNames.erase(st_InsertDllPartNames.begin() + findidx);
        ret = 1;
    }

    LeaveCriticalSection(&st_DllNameCS);
    if(ret > 0)
    {
        assert(pFreePartName && pFreeFullName);
        free(pFreePartName);
        free(pFreeFullName);
        pFreePartName = NULL;
        pFreeFullName = NULL;
    }
    return ret;
}

static int GetDllNames(char**ppFullName,char**ppPartName)
{
}

static BOOL InsertDlls(HANDLE hProcess)
{
    int ret;
    BOOL bret;



    return TRUE;
fail:
    SetLastError(ret);
    return FALSE;
}

typedef BOOL(WINAPI *CreateProcessFunc_t)(LPCTSTR lpApplicationName,LPTSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,LPSECURITY_ATTRIBUTES lpThreadAttributes,BOOL bInheritHandles,DWORD dwCreationFlags,LPVOID lpEnvironment,LPCTSTR lpCurrentDirectory,LPSTARTUPINFO lpStartupInfo,LPPROCESS_INFORMATION lpProcessInformation);
static CreateProcessFunc_t CreateProcessNext=NULL;

BOOL WINAPI CreateProcessCallBack(LPCTSTR lpApplicationName,
                                  LPTSTR lpCommandLine,
                                  LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                  LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                  BOOL bInheritHandles,
                                  DWORD dwCreationFlags,
                                  LPVOID lpEnvironment,
                                  LPCTSTR lpCurrentDirectory,
                                  LPSTARTUPINFO lpStartupInfo,
                                  LPPROCESS_INFORMATION lpProcessInformation)
{
    DWORD dwMyCreationFlags = (dwCreationFlags | CREATE_SUSPENDED);
    PROCESS_INFORMATION pi;
    DWORD processid;
    int ret;
    LPCSTR rlpDlls[2];
    DWORD nDlls = 0;

    if(!CreateProcessNext(lpApplicationName,
                          lpCommandLine,
                          lpProcessAttributes,
                          lpThreadAttributes,
                          bInheritHandles,
                          dwMyCreationFlags,
                          lpEnvironment,
                          lpCurrentDirectory,
                          lpStartupInfo,
                          &pi))
    {
        ret = LAST_ERROR_CODE();
        DEBUG_INFO("lasterror %d\n",GetLastError());
        SetLastError(ret);
        return FALSE;
    }

    DEBUG_INFO("\n");

    if(!InsertDlls(pi.hProcess))
    {
        DEBUG_INFO("\n");
        ret = LAST_ERROR_CODE();
        SetLastError(ret);
        return FALSE;
    }

    if(lpProcessInformation)
    {
        CopyMemory(lpProcessInformation, &pi, sizeof(pi));
    }

    if(!(dwCreationFlags & CREATE_SUSPENDED))
    {
        ResumeThread(pi.hThread);
    }
    DEBUG_INFO("processid %d\n",processid);
    return TRUE;

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
    SetUnhandledExceptionFilter(DetourApplicationCrashHandler);
    hr = CoCreateInstanceNext(rclsid,
                              pUnkOuter,dwClsContext,riid,ppv);
    if(SUCCEEDED(hr))
    {
        if(rclsid == __uuidof(MMDeviceEnumerator))
        {
            IMMDeviceEnumerator* pEnumerator = (IMMDeviceEnumerator*)(*ppv);
            //DEBUG_INFO("\n");
            /*now we should change function*/
            DetourEnumeratorVirtFunctions(pEnumerator);
        }

    }
    return hr;
}




static PCMCAP_CONTROL_t st_DummyControl;

void PcmCapInjectFini(void)
{
    if(st_PcmCapInited)
    {
        int ret;
        int tries = 0;

        do
        {
            memset(&st_DummyControl,0,sizeof(st_DummyControl));
            st_DummyControl.m_Operation = PCMCAP_AUDIO_NONE;
            ret = HandleAudioOperation(&st_DummyControl);
            tries ++ ;
        }
        while(ret < 0 && tries <= 5);
        if(ret < 0)
        {
            ERROR_INFO("could not set audio none error(%d)\n",ret);
        }
        RemoveAllRenders();
        CloseHandle(st_hThreadSema);
        st_hThreadSema = NULL;
    }
    st_PcmCapInited = 0;
    return;
}

static int DetourPCMCapFunctions(void)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((PVOID*)&CoCreateInstanceNext,CoCreateInstanceCallBack);
    DetourTransactionCommit();

    return 0;
}

int PcmCapInjectInit(HMODULE hModule)
{
    int ret;
    InitializeCriticalSection(&st_StateCS);
    InitializeCriticalSection(&st_DetourCS);
    InitializeCriticalSection(&st_ListCS);
    InitializeCriticalSection(&st_RenderCS);
    //InitializeCriticalSection(&st_AudioClientCS);
    InitializeCriticalSection(&st_MMDevCS);
    InitializeCriticalSection(&st_DllNameCS);
    memset(&st_AudioFormat,0,sizeof(st_AudioFormat));
    st_hThreadSema = CreateSemaphore(NULL,1,10,NULL);
    if(st_hThreadSema == NULL)
    {
        /*we do not success*/
        return 0;
    }

    ret = DetourPCMCapFunctions();
    if(ret < 0)
    {
        return ret;
    }
    DEBUG_INFO("\n");
    SetUnhandledExceptionFilter(DetourApplicationCrashHandler);

    st_PcmCapInited = 1;
    DEBUG_INFO("Init PcmCapInject succ\n");
    return 0;
}

