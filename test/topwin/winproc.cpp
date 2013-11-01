

#include <winproc.h>


#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))


typedef struct _ProcessThreadIds
{
    unsigned int m_ProcId;
    int m_SizeHwnds;
    int m_NumHwnds;
    HANDLE *m_pHwnds;
} PROCESS_THREAD_IDS_t,*LPROCESS_THREAD_IDS_t;


BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM lparam)
{
    int ret;
    HANDLE *pTmpHwnds=NULL;
    int sizetmphwnds;
    PROCESS_THREAD_IDS_t *pThreadIds=(PROCESS_THREAD_IDS_t*)lparam;
    unsigned int threadid=0,procid=0;

    assert(pThreadIds);
    assert(pThreadIds->m_ProcId);

    threadid = GetWindowThreadProcessId(hwnd,&procid);
    if(procid != pThreadIds->m_ProcId)
    {
        /*we just return*/
        return TRUE;
    }

    /*
    	now it is the windows we search ,so we should copy it to the
    	struct
    */

    if(pThreadIds->m_SizeHwnds == 0)
    {
        assert(pThreadIds->m_NumHwnds == 0);
        pThreadIds->m_SizeHwnds = 4;
        pThreadIds->m_pHwnds = calloc(sizeof(pThreadIds->m_pHwnds[0]),pThreadIds->m_SizeHwnds);
        if(pThreadIds->m_pHwnds == NULL)
        {
            ret = LAST_ERROR_CODE();
            goto fail;
        }
    }
    else if(pThreadIds->m_SizeHwnds == pThreadIds->m_NumHwnds)
    {
        assert(pThreadIds->m_pHwnds);
        sizetmphwnds = pThreadIds->m_SizeHwnds << 1;
        pTmpHwnds = calloc(sizeof(pTmpHwnds[0]),sizetmphwnds);
        if(pTmpHwnds == NULL)
        {
            ret = LAST_ERROR_CODE();
            goto fail;
        }
        memcpy(pTmpHwnds,pThreadIds->m_pHwnds,pThreadIds->m_SizeHwnds * sizeof(pTmpHwnds[0]));
        free(pThreadIds->m_pHwnds);
        pThreadIds->m_pHwnds = pTmpHwnds;
        pTmpHwnds= NULL;
        pThreadIds->m_SizeHwnds = sizetmphwnds;
        sizetmphwnds = 0;
    }

    pThreadIds->m_pHwnds[pThreadIds->m_NumHwnds] = hwnd;
    pThreadIds->m_NumHwnds ++;

    return TRUE;
fail:
    assert(ret > 0);
    if(pTmpHwnds)
    {
        free(pTmpHwnds);
    }
    pTmpHwnds = NULL;
    SetLastError(ret);
    return FALSE;
}


int GetProcWindHandles(HANDLE hProc,HANDLE **pphWnds,int *pSize)

{
    int ret;
    int num=0;
    HANDLE *pRetWnds=*pphWnds;
    int retsize = *pSize;
    PROCESS_THREAD_IDS_t *pThreadIds=NULL;
    BOOL bret;

    if(retsize > 0 && pRetWnds == NULL)
    {
        ret = ERROR_INVALID_PARAMETER;
        goto fail;
    }

    if(hProc == NULL)
    {
        if(*pphWnds)
        {
            free(*pphWnds);
        }
        *pphWnds = NULL;
        *pSize = 0;
        return 0;
    }

    pThreadIds = calloc(sizeof(*pThreadIds),1);
    if(pThreadIds == NULL)
    {
        ret=  LAST_ERROR_CODE();
        goto fail;
    }

    pThreadIds->m_ProcId = GetProcessId(hProc);
    if(pThreadIds->m_ProcId == 0)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not get process id of (0x%08x) error(%d)\n",hProc,ret);
        goto fail;
    }

    bret = EnumWindow(EnumWindowsProc,pThreadIds);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    /*now first to enum process and make it ok*/
    if(retsize < pThreadIds->m_NumHwnds)
    {
        pRetWnds = pThreadIds->m_pHwnds;
        pThreadIds->m_pHwnds = NULL;
        retsize = pThreadIds->m_SizeHwnds;
    }
    else
    {
        if(pThreadIds->m_NumHwnds > 0)
        {
            memcpy(pRetWnds,pThreadIds->m_pHwnds,pThreadIds->m_NumHwnds*sizeof(pThreadIds->m_pHwnds[0]));
        }
    }
    num = pThreadIds->m_NumHwnds;

    if(pThreadIds)
    {
        if(pThreadIds->m_pHwnds)
        {
            free(pThreadIds->m_pHwnds);
        }
        pThreadIds->m_pHwnds = NULL;
        free(pThreadIds);
    }
    pThreadIds = NULL;


    if(*pphWnds && pRetWnds != *pphWnds)
    {
        free(*pphWnds);
    }
    *pphWnds = pRetWnds;
    *pSize = retsize;

    return num;
fail:
    assert(ret > 0);

    if(pThreadIds)
    {
        if(pThreadIds->m_pHwnds)
        {
            free(pThreadIds->m_pHwnds);
        }
        pThreadIds->m_pHwnds = NULL;
        free(pThreadIds);
    }
    pThreadIds = NULL;

    if(pRetWnds && pRetWnds != *pphWnds)
    {
        free(pRetWnds);
    }
    pRetWnds = NULL;
    retsize = 0;
    SetLastError(ret);
    return -ret;
}



int GetTopWinds(HANDLE *pWnds,int wndnum,HANDLE **ppTopWnds,int *pTopSize)
{
    BOOL bret;
    HANDLE *pRetTopWnds = *ppTopWnds;
    HANDLE *pTmpTopWnds=NULL;
    int rettopsize = *pTopSize,tmptopsize=0;
    int ret;
    int num = 0;
    int i,j;
    HANDLE hTopWin=NULL;

    if(pWnds == NULL)
    {
        if(*ppTopWnds)
        {
            free(*ppTopWnds);
        }
        *ppTopWnds = NULL;
        *pTopSize = 0;
        return 0;
    }

    for(i = 0 ; i<wndnum; i++)
    {
        SetLastError(0);
        hTopWin = GetTopWindow(pWnds[i]);
        if(hTopWin == NULL)
        {
            if(GetLastError() != 0)
            {
                ret = LAST_ERROR_CODE();
                ERROR_INFO("GetTopWindow[%d] 0x%08x error(%d)\n",i,pWnds[i],ret);
                goto fail;
            }
            hTopWin = pWnds[i];
        }

        if((num+1)>rettopsize)
        {
            tmptopsize = rettopsize << 1;
            if(tmptopsize == 0)
            {
                tmptopsize = 4;
            }

            pTmpTopWnds = calloc(sizeof(pTmpTopWnds[0]),tmptopsize);
            if(pTmpTopWnds == NULL)
            {
                ret = LAST_ERROR_CODE();
                goto fail;
            }

            if(num > 0)
            {
                assert(pRetTopWnds);
                memcpy(pTmpTopWnds,pRetTopWnds,num * sizeof(pTmpTopWnds[0]));
            }

            if(pRetTopWnds && pRetTopWnds != *ppTopWnds)
            {
                free(pRetTopWnds);
            }
            pRetTopWnds = pTmpTopWnds;
            pTmpTopWnds = NULL;
            rettopsize = tmptopsize;
        }

        pRetTopWnds[num] = hTopWin;

        num ++;
    }

    if(*ppTopWnds && *ppTopWnds != pRetTopWnds)
    {
        free(*ppTopWnds);
    }
    if(pTmpTopWnds)
    {
        free(pTmpTopWnds);
    }
    pTmpTopWnds = NULL;

    *ppTopWnds = pRetTopWnds;
    *pTopSize = rettopsize;
    return num;

fail:
    assert(ret > 0);
    if(pRetTopWnds && pRetTopWnds != *ppTopWnds)
    {
        free(pRetTopWnds);
    }
    pRetTopWnds = NULL;
    if(pTmpTopWnds)
    {
        free(pTmpTopWnds);
    }
    pTmpTopWnds = NULL;
    SetLastError(ret);
    return -ret;
}

