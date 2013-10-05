

#include "pcmcap_capper.h"
#include <dllinsert.h>
#include <output_debug.h>
#include <timeticks.h>
#include <assert.h>
#include <memshare.h>
#include <evt.h>
#include <sched.h>

#define  PCMCAP_DLL_NAME                 "pcmcapinject.dll"
#define  PCMCAP_SET_OPERATION_FUNC_NAME  "HandleAudioOperation"

#define  MAP_FILE_OBJNAME_BASE           "PCMCAP_CAPPER_MAPFILE"
#define  FREE_EVENT_OBJNAME_BASE         "PCMCAP_CAPPER_FREEEVT"
#define  FILL_EVENT_OBJNAME_BASE         "PCMCAP_CAPPER_FILLEVT"
#define  START_EVENT_OBJNAME_BASE        "PCMCAP_CAPPER_START"
#define  STOP_EVENT_OBJNAME_BASE         "PCMCAP_CAPPER_STOP"

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))

CPcmCapper::CPcmCapper()
{
    m_hProc = NULL;
    m_ProcessId = 0;
    m_iOperation = PCMCAPPER_OPERATION_NONE;
    m_pPcmCapperCb = NULL;
    m_lpParam = NULL;
    m_BufNum = 0;
    m_BufBlockSize = 0;

    m_ThreadControl.m_hThread = NULL;
    m_ThreadControl.m_ThreadId = 0;
    m_ThreadControl.m_hExitEvt = NULL;
    m_ThreadControl.m_ThreadRunning = 0;
    m_ThreadControl.m_ThreadExited = 1;

    m_hMapFile = NULL;
    m_pMapBuffer = NULL;
    memset(&(m_MapBaseName),0,sizeof(m_MapBaseName));

    memset(&(m_FreeEvtBaseName),0,sizeof(m_FreeEvtBaseName));
    memset(&(m_FillEvtBaseName),0,sizeof(m_FillEvtBaseName));
    memset(&(m_StartEvtBaseName),0,sizeof(m_StartEvtBaseName));
    memset(&(m_StopEvtBaseName),0,sizeof(m_StopEvtBaseName));
    m_hStartEvt = NULL;
    m_hStopEvt = NULL;
    m_pFreeEvt = NULL;
    m_pFillEvt = NULL;
}

BOOL CPcmCapper::__SetOperationInner(PCMCAP_CONTROL_t * pControl,DWORD *pRetCode)
{
    int ret,res;
    LPVOID pRemoteFunc=NULL,pRemoteAlloc=NULL;
    HANDLE hThread=NULL;
    BOOL bres,bret;
    SIZE_T retsize;
    DWORD threadid=0;
    DWORD dret;
    unsigned int stick,ctick,etick,lefttick;
    int timeout=4;
    DWORD retcode;

	DEBUG_INFO("operation %d\n",pControl->m_Operation);

    pControl->m_Timeout = timeout;
    /*now to call the */
    ret = __GetRemoteProcAddress(this->m_ProcessId,PCMCAP_DLL_NAME,PCMCAP_SET_OPERATION_FUNC_NAME,&pRemoteFunc);
    if(ret < 0)
    {
        ret = -ret;
        goto fail;
    }

    /*now to allocate memory and we will put the memory*/
    pRemoteAlloc = VirtualAllocEx(this->m_hProc,NULL,sizeof(*pControl),MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if(pRemoteAlloc == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not allocate 0x%x (%d) size %d error(%d)\n",this->m_hProc,this->m_ProcessId,sizeof(*pControl),ret);
        goto fail;
    }

    bret = WriteProcessMemory(this->m_hProc,pRemoteAlloc,pControl,sizeof(*pControl),&retsize);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not write 0x%x (%d) addr 0x%p size %d error(%d)\n",
                   this->m_hProc,this->m_ProcessId,pRemoteAlloc,sizeof(*pControl),ret);
        goto fail;
    }

    if(retsize != sizeof(*pControl))
    {
        ret = ERROR_INSUFFICIENT_BUFFER;
        ERROR_INFO("could not write return %d != %d\n",retsize ,sizeof(*pControl));
        goto fail;
    }

    /*now to call remote address*/
    hThread = CreateRemoteThread(this->m_hProc,NULL,0,(LPTHREAD_START_ROUTINE)pRemoteFunc,pRemoteAlloc,0,&threadid);
    if(hThread == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not create (0x%x:%d) process thread error(%d)\n",this->m_hProc,this->m_ProcessId,ret);
        goto fail;
    }

    timeout = 4;
    bret = InitializeTicks(&stick,&ctick,&etick,timeout);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    /*now to do the job handle*/
    while(1)
    {
        lefttick = LeftTicks(&stick,&ctick,&etick,timeout);
        if(lefttick == 0)
        {
            ERROR_INFO("thread 0x%x timeout %d\n",hThread,timeout);
            ret = WAIT_TIMEOUT;
            goto fail;
        }

        dret = WaitForSingleObject(hThread,lefttick);
        if(dret == WAIT_OBJECT_0)
        {
            bret = GetExitCodeThread(hThread,&retcode);
            if(bret)
            {
                break;
            }
        }
        else if(dret == WAIT_FAILED)
        {
            ret = LAST_ERROR_CODE();
            goto fail;
        }

        GetCurrentTicks(&ctick);
    }

    *pRetCode = retcode;

    assert(hThread);
    CloseHandle(hThread);
    hThread = NULL;

    assert(pRemoteAlloc);
    bres = VirtualFreeEx(this->m_hProc,pRemoteAlloc,sizeof(*pControl),MEM_DECOMMIT);
    if(!bres)
    {
        res = LAST_ERROR_CODE();
        ERROR_INFO("could not free (0x%x) process %d handle remoteaddr 0x%p size %d error(%d)\n",
                   this->m_hProc,this->m_ProcessId,pRemoteAlloc,sizeof(*pControl),ret);
    }

    pRemoteAlloc = NULL;


    return TRUE;
fail:
    if(hThread)
    {
        /*now to wait for a while and at last to kill it*/
        timeout = 2;
        bres = InitializeTicks(&stick,&ctick,&etick,timeout);
        if(bres)
        {
            while(1)
            {
                lefttick = LeftTicks(&stick,&ctick,&etick,timeout);
                if(lefttick == 0)
                {
                    ERROR_INFO("could not wait for thread handle 0x%x timeout\n",hThread);
                    break;
                }
                dret = WaitForSingleObject(hThread,lefttick);
                if(dret == WAIT_OBJECT_0)
                {
                    bres = GetExitCodeThread(hThread,&retcode);
                    if(bres)
                    {
                        break;
                    }
                }
                else if(dret == WAIT_FAILED)
                {
                    break;
                }

                GetCurrentTicks(&ctick);
            }
        }
        else
        {
            ERROR_INFO("could not get ticks\n");
        }
        CloseHandle(hThread);
    }
    hThread = NULL;
    if(pRemoteAlloc)
    {
        bres = VirtualFreeEx(this->m_hProc,pRemoteAlloc,sizeof(*pControl),MEM_DECOMMIT);
        if(!bres)
        {
            res = LAST_ERROR_CODE();
            ERROR_INFO("could not free (0x%x) handle remoteaddr 0x%p size %d error(%d)\n",
                       this->m_hProc,pRemoteAlloc,sizeof(*pControl),ret);
        }
    }
    pRemoteAlloc = NULL;
    pRemoteFunc = NULL;
    SetLastError(ret);
    return FALSE;
}

BOOL CPcmCapper::__SetOperationNone()
{
    PCMCAP_CONTROL_t* pControl=NULL;
    BOOL bret;
    DWORD retcode=0;

    if(this->m_hProc == NULL)
    {
        /*it is not set*/
        return TRUE;
    }

    pControl = (PCMCAP_CONTROL_t*)malloc(sizeof(*pControl));
    if(pControl == NULL)
    {
        return FALSE;
    }

    memset(pControl,0,sizeof(*pControl));
    pControl->m_Operation = PCMCAPPER_OPERATION_NONE;

    bret = this->__SetOperationInner(pControl,&retcode);


    free(pControl);
    pControl = NULL;

    if(!bret)
    {
        return FALSE;
    }
    if(retcode != 0)
    {
        SetLastError(ERROR_FATAL_APP_EXIT);
        return FALSE;
    }



    return TRUE;

}


BOOL CPcmCapper::Stop()
{
    return this->__StopOperation(PCMCAPPER_OPERATION_NONE);
}

BOOL CPcmCapper::__StopOperation(int iOperation)
{
    int lasterr = 0;
    BOOL bret,totalbret=TRUE;

    if(this->m_hProc)
    {
        /*now to set the */
        assert(iOperation == PCMCAPPER_OPERATION_NONE || iOperation == PCMCAPPER_OPERATION_RENDER);
        switch(iOperation)
        {
        case PCMCAPPER_OPERATION_NONE:
            bret = this->__SetOperationNone();
            break;
        case PCMCAPPER_OPERATION_RENDER:
            bret = this->__SetOperationRender();
            break;

        }
        if(!bret)
        {
            totalbret = FALSE;
            lasterr = lasterr ? lasterr : (LAST_ERROR_CODE());
        }
    }
    this->m_iOperation = iOperation;

    this->__StopThread();
    this->__DestroyEvent();


    this->__DestroyMap();
    SetLastError(lasterr);
    return totalbret;

}

CPcmCapper::~CPcmCapper()
{
    this->Stop();
    this->m_hProc = NULL;
    this->m_BufNum = 0;
    this->m_BufBlockSize = 0;
    this->m_pPcmCapperCb = NULL;
    this->m_lpParam = NULL;
}

void CPcmCapper::__DestroyMap()
{
    UnMapFileBuffer(&(this->m_pMapBuffer));
    CloseMapFileHandle(&(this->m_hMapFile));
    memset(&(this->m_MapBaseName),0,sizeof(this->m_MapBaseName));
    return ;
}

int CPcmCapper::__CreateMap()
{
    int ret;
    unsigned int mapsize;
    _snprintf_s((char*)this->m_MapBaseName,sizeof(this->m_MapBaseName),_TRUNCATE,"%s%d",MAP_FILE_OBJNAME_BASE,this->m_ProcessId);
    mapsize = this->m_BufBlockSize * this->m_BufNum;
    this->m_hMapFile = CreateMapFile((char*)this->m_MapBaseName,mapsize ,1);
    if(this->m_hMapFile == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not create (%s) (%d:0x%x) error (%d)\n",this->m_MapBaseName,mapsize,mapsize,ret);
        this->__DestroyMap();
        return -ret;
    }

    this->m_pMapBuffer = MapFileBuffer(this->m_hMapFile,mapsize);
    if(this->m_pMapBuffer == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not mapfilebuffer %s (%d:0x%x) error(%d)\n",this->m_MapBaseName,mapsize,mapsize,ret);
        this->__DestroyMap();
        return -ret;
    }

    return 0;
}


void CPcmCapper::__DestroyEvent()
{
    unsigned int i;
    for(i=0; i<this->m_BufNum; i++)
    {
        /*now to close handle*/
        if(this->m_pFillEvt)
        {
            if(this->m_pFillEvt[i])
            {
                CloseHandle(this->m_pFillEvt[i]);
            }
            this->m_pFillEvt[i] = NULL;
        }

        if(this->m_pFreeEvt)
        {
            if(this->m_pFreeEvt[i])
            {
                CloseHandle(this->m_pFreeEvt[i]);
            }
            this->m_pFreeEvt[i] = NULL;
        }
    }

    memset(&(this->m_FillEvtBaseName),0,sizeof(this->m_FillEvtBaseName));
    memset(&(this->m_FreeEvtBaseName),0,sizeof(this->m_FreeEvtBaseName));
    memset(&(this->m_StartEvtBaseName),0,sizeof(this->m_StartEvtBaseName));
    memset(&(this->m_StopEvtBaseName),0,sizeof(this->m_StopEvtBaseName));

    if(this->m_hStartEvt)
    {
        CloseHandle(this->m_hStartEvt);
    }
    this->m_hStartEvt = NULL;

    if(this->m_hStopEvt)
    {
        CloseHandle(this->m_hStopEvt);
    }
    this->m_hStopEvt = NULL;

    if(this->m_pFillEvt)
    {
        free(this->m_pFillEvt);
    }
    this->m_pFillEvt = NULL;

    if(this->m_pFreeEvt)
    {
        free(this->m_pFreeEvt);
    }
    this->m_pFreeEvt = NULL;
    return ;
}

int CPcmCapper::__CreateEvent()
{
    int ret;
    unsigned int i;
    unsigned char evname[128];
    _snprintf_s((char*)this->m_FreeEvtBaseName,sizeof(this->m_FreeEvtBaseName),_TRUNCATE,"%s%d",FREE_EVENT_OBJNAME_BASE,this->m_ProcessId);
    _snprintf_s((char*)this->m_FillEvtBaseName,sizeof(this->m_FillEvtBaseName),_TRUNCATE,"%s%d",FILL_EVENT_OBJNAME_BASE,this->m_ProcessId);
    _snprintf_s((char*)this->m_StartEvtBaseName,sizeof(this->m_StartEvtBaseName),_TRUNCATE,"%s_%d",START_EVENT_OBJNAME_BASE,this->m_ProcessId);
    _snprintf_s((char*)this->m_StopEvtBaseName,sizeof(this->m_StopEvtBaseName),_TRUNCATE,"%s_%d",STOP_EVENT_OBJNAME_BASE,this->m_ProcessId);


    assert(this->m_pFreeEvt == NULL);
    assert(this->m_pFillEvt == NULL);
    assert(this->m_hStartEvt == NULL);
    assert(this->m_hStopEvt == NULL);
    assert(this->m_BufNum > 0);
    assert(this->m_BufBlockSize >= 0x1000);

    this->m_pFreeEvt =(HANDLE*) calloc(sizeof(this->m_pFreeEvt[0]),this->m_BufNum);
    if(this->m_pFreeEvt == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not allocate size %d error (%d)\n",sizeof(this->m_pFreeEvt[0])*this->m_BufNum,ret);
        this->__DestroyEvent();
        SetLastError(ret);
        return -ret;
    }

    for(i=0; i<this->m_BufNum; i++)
    {
        _snprintf_s((char*)evname,sizeof(evname),_TRUNCATE,"%s_%d",this->m_FreeEvtBaseName,i);
        this->m_pFreeEvt[i] = GetEvent((const char*)evname,1);
        if(this->m_pFreeEvt[i] == NULL)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("could not create %s event error(%d)\n",evname,ret);
            this->__DestroyEvent();
            SetLastError(ret);
            return -ret;
        }
    }

    this->m_pFillEvt =(HANDLE*) calloc(sizeof(this->m_pFillEvt[0]),this->m_BufNum);
    if(this->m_pFillEvt == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not allocate size %d error (%d)\n",sizeof(this->m_pFillEvt[0])*this->m_BufNum,ret);
        this->__DestroyEvent();
        SetLastError(ret);
        return -ret;
    }

    for(i=0; i<this->m_BufNum; i++)
    {
        _snprintf_s((char*)evname,sizeof(evname),_TRUNCATE,"%s_%d",this->m_FillEvtBaseName,i);
        this->m_pFillEvt[i] = GetEvent((const char*)evname,1);
        if(this->m_pFillEvt[i] == NULL)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("could not create %s event error(%d)\n",evname,ret);
            this->__DestroyEvent();
            SetLastError(ret);
            return -ret;
        }
    }

    this->m_hStartEvt = GetEvent((const char*)this->m_StartEvtBaseName,1);
    if(this->m_hStartEvt == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not create startevent (%s) error(%d)\n",this->m_StartEvtBaseName,ret);
        this->__DestroyEvent();
        SetLastError(ret);
        return -ret;
    }

    this->m_hStopEvt = GetEvent((const char*)this->m_StopEvtBaseName,1);
    if(this->m_hStopEvt == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not create stopevent (%s) error(%d)\n",this->m_StopEvtBaseName,ret);
        this->__DestroyEvent();
        SetLastError(ret);
        return -ret;
    }

    return 0;
}

int CPcmCapper::__FreeAllEvent()
{
    unsigned int i;
    int ret;
    BOOL bret;

    assert(this->m_pFreeEvt);
    assert(this->m_BufNum > 0);

    for(i=0; i<this->m_BufNum; i++)
    {
        assert(this->m_pFreeEvt[i]);
        bret = SetEvent(this->m_pFreeEvt[i]);
        if(!bret)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("[%d] setevent error(%d)\n",i,ret);
            goto fail;
        }
    }

    SetLastError(0);
    return 0;

fail:
    SetLastError(ret);
    return -ret;
}

BOOL CPcmCapper::Start(HANDLE hProc,int iOperation,int iBufNum,int iBlockSize,IPcmCapperCallback * pPcc,LPVOID lpParam)
{

    if(iOperation != PCMCAP_AUDIO_BOTH &&
            iOperation != PCMCAP_AUDIO_NONE &&
            iOperation != PCMCAP_AUDIO_CAPTURE &&
            iOperation != PCMCAP_AUDIO_RENDER)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if(iBufNum < 1 || iBlockSize < 0x1000 || hProc == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    this->__StopOperation(PCMCAPPER_OPERATION_NONE);

    this->m_hProc = hProc;
    this->m_BufNum = iBufNum;
    this->m_BufBlockSize = iBlockSize;
    this->m_ProcessId = GetProcessId(hProc);
    this->m_pPcmCapperCb = pPcc;
    this->m_lpParam = lpParam;

    return this->SetAudioOperation(iOperation);
}

void CPcmCapper::__StopThread()
{
    BOOL bret;
    int ret;
    if(this->m_ThreadControl.m_hThread)
    {
        assert(this->m_ThreadControl.m_hExitEvt);
        this->m_ThreadControl.m_ThreadRunning = 0;
        while(this->m_ThreadControl.m_ThreadExited == 0)
        {
            bret = SetEvent(this->m_ThreadControl.m_hExitEvt);
            if(!bret)
            {
                ret = LAST_ERROR_CODE();
                ERROR_INFO("could not set exitevt error(%d)\n",ret);
            }
            SchedOut();
        }

        /*now we should thread control*/
        CloseHandle(this->m_ThreadControl.m_hThread);
    }

    this->m_ThreadControl.m_hThread = NULL;
    this->m_ThreadControl.m_ThreadExited = 1;
    if(this->m_ThreadControl.m_hExitEvt)
    {
        CloseHandle(this->m_ThreadControl.m_hExitEvt);
    }
    this->m_ThreadControl.m_hExitEvt = NULL;
    this->m_ThreadControl.m_ThreadRunning = 0;
    this->m_ThreadControl.m_ThreadId = 0;
    return ;
}

int CPcmCapper::__StartThread()
{
    int ret;

    assert(this->m_ThreadControl.m_hThread == NULL);
    assert(this->m_ThreadControl.m_hExitEvt == NULL);
    assert(this->m_ThreadControl.m_ThreadExited == 1);
    assert(this->m_ThreadControl.m_ThreadRunning == 0);
    assert(this->m_ThreadControl.m_ThreadId == 0);

    this->m_ThreadControl.m_hExitEvt = GetEvent(NULL,1);
    if(this->m_ThreadControl.m_hExitEvt == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not create exit event error(%d)\n",ret);
        goto fail;
    }

    /*now for the thread running state*/
    this->m_ThreadControl.m_ThreadExited = 0;
    this->m_ThreadControl.m_ThreadRunning = 1;

    this->m_ThreadControl.m_hThread = CreateThread(NULL,0,CPcmCapper::ThreadFunc,this,0,&(this->m_ThreadControl.m_ThreadId));
    if(this->m_ThreadControl.m_hThread == NULL)
    {
        this->m_ThreadControl.m_hThread = NULL;
        this->m_ThreadControl.m_ThreadExited = 1;
        this->m_ThreadControl.m_ThreadRunning = 0;
        this->m_ThreadControl.m_ThreadId = 0;
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not create thread func error(%d)\n",ret);
        goto fail;
    }

    /*ok we should start thread*/

    return 0;
fail:
    this->__StopThread();
    return -ret;
}

DWORD WINAPI CPcmCapper::ThreadFunc(void * arg)
{
    CPcmCapper* pThis = (CPcmCapper*)arg;
    return pThis->__ThreadImpl();
}

DWORD CPcmCapper::__ThreadImpl()
{
    int ret;
    DWORD dret;
    HANDLE *pWaitHandle=NULL;
    unsigned int i;
    unsigned int bufnum;

    /*to include the exit num*/
    assert(this->m_BufNum > 0);
    assert(this->m_ThreadControl.m_hExitEvt);
    bufnum = this->m_BufNum;
    pWaitHandle = (HANDLE*)calloc(sizeof(*pWaitHandle),bufnum+3);
    if(pWaitHandle == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not allocate %d handles error(%d)\n",(bufnum+3)*sizeof(*pWaitHandle),ret);
        goto out;
    }

    for(i=0; i<bufnum; i++)
    {
        assert(this->m_pFillEvt);
        assert(this->m_pFreeEvt);
        assert(this->m_pFillEvt[i]);
        assert(this->m_pFreeEvt[i]);
        pWaitHandle[i] = this->m_pFillEvt[i];
    }

    pWaitHandle[bufnum] = this->m_ThreadControl.m_hExitEvt;
    pWaitHandle[bufnum+1] = this->m_hStartEvt;
    pWaitHandle[bufnum+2] = this->m_hStopEvt;


    while(this->m_ThreadControl.m_ThreadRunning)
    {
        dret = WaitForMultipleObjects(bufnum + 3,pWaitHandle,FALSE,INFINITE);
        if(dret <= (WAIT_OBJECT_0+bufnum-1) && dret >= WAIT_OBJECT_0)
        {
            this->__AudioRenderBuffer(dret - WAIT_OBJECT_0);
        }
        else if(dret == (WAIT_OBJECT_0 + bufnum))
        {
            /*nothing to handle*/
            ;
        }
        else if(dret == (WAIT_OBJECT_0 + bufnum + 1))
        {
            /*this is start calling*/
            this->__AudioStartCall();
        }
        else if(dret == (WAIT_OBJECT_0+bufnum + 2))
        {
            /*this is stop calling*/
            this->__AudioStopCall();
        }
        else if(dret == WAIT_FAILED)
        {
            ret = -LAST_ERROR_CODE();
            ERROR_INFO("wait for num %d error(%d)\n",bufnum,ret);
            goto out;
        }
    }

    ret = 0;
out:
    if(pWaitHandle)
    {
        free(pWaitHandle);
    }
    pWaitHandle = NULL;
    this->m_ThreadControl.m_ThreadExited = 1;
    return ret;
}


void CPcmCapper::__AudioRenderBuffer(int idx)
{
    PCMCAP_AUDIO_BUFFER_t* pItem;
    BOOL bret;
    int ret;
    assert(this->m_pMapBuffer);
    assert(idx >= 0 && idx < (int)this->m_BufNum);

    pItem = (PCMCAP_AUDIO_BUFFER_t*)((ptr_type_t)this->m_pMapBuffer + (idx)*this->m_BufBlockSize);

    if(this->m_pPcmCapperCb)
    {
        this->m_pPcmCapperCb->WaveInCb(pItem,this->m_lpParam);
    }

    /*now to set the event*/
    assert(this->m_pFreeEvt);
    assert(this->m_pFreeEvt[idx]);
    bret = SetEvent(this->m_pFreeEvt[idx]);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("setevent %d error(%d)\n",
                   idx,ret);

    }

    return ;

}

BOOL CPcmCapper::__SetOperationBoth()
{
    PCMCAP_CONTROL_t* pControl=NULL;
    int ret;
    BOOL bret;
    DWORD retcode;
    assert(this->m_hProc);
    assert(this->m_ProcessId);
    pControl = (PCMCAP_CONTROL_t*)calloc(sizeof(*pControl),1);
    if(pControl == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }
    pControl->m_Operation = PCMCAPPER_OPERATION_BOTH;
    strncpy_s((char*)pControl->m_MemShareName,sizeof(pControl->m_MemShareName),(const char*)this->m_MapBaseName,_TRUNCATE);
    pControl->m_MemShareSize = this->m_BufNum * this->m_BufBlockSize;
    pControl->m_PackSize = this->m_BufBlockSize;
    pControl->m_NumPacks = this->m_BufNum;
    strncpy_s((char*)pControl->m_FreeListSemNameBase,sizeof(pControl->m_FreeListSemNameBase),(const char*)this->m_FreeEvtBaseName,_TRUNCATE);
    strncpy_s((char*)pControl->m_FillListSemNameBase,sizeof(pControl->m_FillListSemNameBase),(const char*)this->m_FillEvtBaseName,_TRUNCATE);
    strncpy_s((char*)pControl->m_StartEvtName,sizeof(pControl->m_StartEvtName),(const char*)this->m_StartEvtBaseName,_TRUNCATE);
    strncpy_s((char*)pControl->m_StopEvtName,sizeof(pControl->m_StopEvtName),(const char*)this->m_StopEvtBaseName,_TRUNCATE);

    bret = this->__SetOperationInner(pControl,&retcode);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not set operation BOTH error (%d)\n",ret);
        goto fail;
    }
    if(retcode != 0)
    {
        ret = ERROR_FATAL_APP_EXIT;
        ERROR_INFO("set both error code %d\n",retcode);
        goto fail;
    }

    free(pControl);
    pControl = NULL;

    SetLastError(0);
    return TRUE;
fail:
    if(pControl)
    {
        free(pControl);
    }
    pControl = NULL;
    SetLastError(ret);
    return FALSE;
}

BOOL CPcmCapper::__SetOperationCapture()
{
    PCMCAP_CONTROL_t* pControl=NULL;
    int ret;
    BOOL bret;
    DWORD retcode;
    /**/
    pControl = (PCMCAP_CONTROL_t*)calloc(sizeof(*pControl),1);
    if(pControl == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }
    pControl->m_Operation = PCMCAPPER_OPERATION_CAPTURE;
    strncpy_s((char*)pControl->m_MemShareName,sizeof(pControl->m_MemShareName),(const char*)this->m_MapBaseName,_TRUNCATE);
    pControl->m_MemShareSize = this->m_BufNum * this->m_BufBlockSize;
    pControl->m_PackSize = this->m_BufBlockSize;
    pControl->m_NumPacks = this->m_BufNum;
    strncpy_s((char*)pControl->m_FreeListSemNameBase,sizeof(pControl->m_FreeListSemNameBase),(const char*)this->m_FreeEvtBaseName,_TRUNCATE);
    strncpy_s((char*)pControl->m_FillListSemNameBase,sizeof(pControl->m_FillListSemNameBase),(const char*)this->m_FillEvtBaseName,_TRUNCATE);

    bret = this->__SetOperationInner(pControl,&retcode);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not set operation BOTH error (%d)\n",ret);
        goto fail;
    }
    if(retcode != 0)
    {
        ret = ERROR_FATAL_APP_EXIT;
        ERROR_INFO("set both error code %d\n",retcode);
        goto fail;
    }

    free(pControl);
    pControl = NULL;

    SetLastError(0);
    return TRUE;
fail:
    if(pControl)
    {
        free(pControl);
    }
    pControl = NULL;
    SetLastError(ret);
    return FALSE;
}

BOOL CPcmCapper::__SetOperationRender()
{
    PCMCAP_CONTROL_t* pControl=NULL;
    int ret;
    BOOL bret;
    DWORD retcode;

    if(this->m_hProc == NULL)
    {
        return TRUE;
    }
    /**/
    pControl = (PCMCAP_CONTROL_t*)calloc(sizeof(*pControl),1);
    if(pControl == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }
    pControl->m_Operation = PCMCAPPER_OPERATION_RENDER;

    bret = this->__SetOperationInner(pControl,&retcode);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not set operation BOTH error (%d)\n",ret);
        goto fail;
    }
    if(retcode != 0)
    {
        ret = ERROR_FATAL_APP_EXIT;
        ERROR_INFO("set both error code %d\n",retcode);
        goto fail;
    }

    free(pControl);
    pControl = NULL;

    SetLastError(0);
    return TRUE;
fail:
    if(pControl)
    {
        free(pControl);
    }
    pControl = NULL;
    SetLastError(ret);
    return FALSE;
}

BOOL CPcmCapper::SetAudioOperation(int iOperation)
{
    BOOL bret=TRUE;

    switch(iOperation)
    {
    case PCMCAPPER_OPERATION_NONE:
    case PCMCAPPER_OPERATION_RENDER:
        this->__StopOperation(iOperation);
        break;

    case PCMCAPPER_OPERATION_BOTH:
        this->__StopOperation(PCMCAPPER_OPERATION_NONE);
        if(this->m_hProc == NULL || this->m_ProcessId == 0 || this->m_BufBlockSize < 0x1000 ||
                this->m_BufNum < 1)
        {
            bret = FALSE;
            SetLastError(ERROR_INVALID_PARAMETER);
            break;
        }
        bret = this->__StartOperation(PCMCAPPER_OPERATION_BOTH);
        break;
    case PCMCAPPER_OPERATION_CAPTURE:
        this->__StopOperation(PCMCAPPER_OPERATION_NONE);
        if(this->m_hProc == NULL || this->m_ProcessId == 0 || this->m_BufBlockSize < 0x1000 ||
                this->m_BufNum < 1)
        {
            bret = FALSE;
            SetLastError(ERROR_INVALID_PARAMETER);
            break;
        }
        bret = this->__StartOperation(PCMCAPPER_OPERATION_CAPTURE);
        break;
    default:
        bret = FALSE;
        SetLastError(ERROR_NOT_SUPPORTED);
        break;
    }

    if(!bret)
    {
        return FALSE;
    }

    return TRUE;
}


BOOL CPcmCapper::__StartOperation(int iOperation)
{
    int ret;
    BOOL bret;
    /*now we should allocate the buffer and the event*/
    ret = this->__CreateMap();
    if(ret < 0)
    {
        this->Stop();
        SetLastError(-ret);
        return FALSE;
    }

    ret = this->__CreateEvent();
    if(ret < 0)
    {
        this->Stop();
        SetLastError(-ret);
        return FALSE;
    }


    ret = this->__FreeAllEvent();
    if(ret < 0)
    {
        this->Stop();
        SetLastError(-ret);
        return FALSE;
    }

    ret = this->__StartThread();
    if(ret < 0)
    {
        this->Stop();
        SetLastError(-ret);
        return FALSE;
    }

    switch(iOperation)
    {
    case PCMCAPPER_OPERATION_BOTH:
        bret= this->__SetOperationBoth();
        break;
    case PCMCAPPER_OPERATION_CAPTURE:
        bret = this->__SetOperationCapture();
        break;
    }

    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        this->Stop();
        SetLastError(ret);
        return FALSE;
    }

    return TRUE;
}

void CPcmCapper::__AudioStartCall()
{
    if(this->m_pPcmCapperCb)
    {
        this->m_pPcmCapperCb->WaveOpenCb(this->m_lpParam);
    }
    return ;
}

void CPcmCapper::__AudioStopCall()
{
    if(this->m_pPcmCapperCb)
    {
        this->m_pPcmCapperCb->WaveCloseCb(this->m_lpParam);
    }
    return ;
}

