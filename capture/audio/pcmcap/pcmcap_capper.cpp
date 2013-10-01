

#include "pcmcap_capper.h"

#define  PCMCAP_DLL_NAME                 "pcmcapinject.dll"
#define  PCMCAP_SET_OPERATION_FUNC_NAME  "PcmCapInject_SetAudioOperation"

CPcmCapper::CPcmCapper()
{
    m_hProc = NULL;
    m_iOperation = PCMCAPPER_OPERATION_NONE;
    m_pPcmCapperCb = NULL;
    m_lpParam = NULL;
    m_BufNum = 0;
    memset(&(m_FreeEvtBaseName),0,sizeof(m_FreeEvtBaseName));
    memset(&(m_FillEvtBaseName),0,sizeof(m_FillEvtBaseName));
    m_pFillEvt = NULL;
    m_pFreeEvt = NULL;
}

BOOL CPcmCapper::__SetOperationInner(PCMCAP_CONTROL_t * pControl,DWORD *pRetCode)
{
    int ret,res;
    LPVOID pRemoteFunc=NULL,pRemoteAlloc=NULL;
    HANDLE hThread=NULL;
    unsigned int processid=0;
    BOOL bres,bret;
    SIZE_T retsize;
    DWORD threadid=0;
    DWORD dret;
    unsigned int stick,ctick,etick,lefttick;
    int timeout=4;
    DWORD retcode;
    processid = GetProcessId(this->m_hProc);


    /*now to call the */
    ret = __GetRemoteProcAddress(processid,PCMCAP_DLL_NAME,PCMCAP_SET_OPERATION_FUNC_NAME,&pRemoteFunc);
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
        ERROR_INFO("could not allocate 0x%x size %d error(%d)\n",this->m_hProc,sizeof(*pControl),ret);
        goto fail;
    }

    bret = WriteProcessMemory(this->m_hProc,pRemoteAlloc,pControl,sizeof(*pControl),&retsize);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not write 0x%x addr 0x%p size %d error(%d)\n",
                   this->m_hProc,pRemoteAlloc,sizeof(*pControl),ret);
        goto fail;
    }

    if(retsize != sizeof(*pControl))
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not write return %d != %d\n",retsize ,sizeof(*pControl));
        goto fail;
    }

    /*now to call remote address*/
    hThread = CreateRemoteThread(this->m_hProc,NULL,0,pRemoteFunc,pRemoteAlloc,0,&threadid);
    if(hThread == NULL)
    {
        ret = LAST_ERROR_CODE();
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
        else if(dret == WAIT_FAIL)
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
        ERROR_INFO("could not free (0x%x) handle remoteaddr 0x%p size %d error(%d)\n",
                   this->m_hProc,pRemoteAlloc,sizeof(*pControl),ret);
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

                bres = GetCurrentTicks(&ctick);
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
    PCMCAP_CONTROL_t *pControl=NULL;
    DWORD retcode=0;

    if(this->m_hProc == NULL)
    {
        /*it is not set*/
        return TRUE;
    }

    pControl = malloc(sizeof(*pControl));
    if(pControl == NULL)
    {
        return FALSE;
    }

    memset(pControl,0,sizeof(*pControl));
    pControl->m_Operation = PCMCAPPER_OPERATION_NONE;

    bret = this->__SetOperationInner(pControl,&retcode);

    free(pControl);
    pControl = NULL;
    if(retcode != 0)
    {
        SetLastError(ERROR_FATAL_APP_EXIT);
        return FALSE;
    }

    return TRUE;

}


BOOL CPcmCapper::Stop()
{
    int lasterr=0;
    BOOL bret,totalbret=True;
    int i;

    if(this->m_hProc)
    {
        /*now to set the */
        bret = this->__SetOperationNone();
        if(!bret)
        {
            totalbret = False;
            lasterr = lasterr ? lasterr : (LAST_ERROR_CODE());
        }
    }
    this->m_iOperation = PCMCAPPER_OPERATION_NONE;

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

    if(lasterr)
    {
        SetLastError(lasterr);
    }
    else
    {
        SetLastError(0);
    }
    return totalbret;
}

CPcmCapper::~CPcmCapper()
{
    this->Stop();
}
