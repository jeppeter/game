

#include <winproc.h>
#include <assert.h>
#include <output_debug.h>
#include <vector>

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))


typedef struct _ProcessThreadIds
{
    unsigned int m_ProcId;
    int m_SizeHwnds;
    int m_NumHwnds;
    HWND *m_pHwnds;
} PROCESS_THREAD_IDS_t,*LPROCESS_THREAD_IDS_t;


BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM lparam)
{
    int ret;
    HWND *pTmpHwnds=NULL;
    int sizetmphwnds;
    PROCESS_THREAD_IDS_t *pThreadIds=(PROCESS_THREAD_IDS_t*)lparam;
    DWORD threadid=0,procid=0;

    assert(pThreadIds);
    assert(pThreadIds->m_ProcId);

    threadid = GetWindowThreadProcessId(hwnd,&procid);
    if(procid != pThreadIds->m_ProcId)
    {
        /*we just return*/
        return TRUE;
    }
    DEBUG_INFO("hwnd 0x%08x get procid %d\n",hwnd,procid);

    /*
    	now it is the windows we search ,so we should copy it to the
    	struct
    */

    if(pThreadIds->m_SizeHwnds == 0)
    {
        assert(pThreadIds->m_NumHwnds == 0);
        pThreadIds->m_SizeHwnds = 4;
        pThreadIds->m_pHwnds = (HWND*)calloc(sizeof(pThreadIds->m_pHwnds[0]),pThreadIds->m_SizeHwnds);
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
        pTmpHwnds = (HWND*)calloc(sizeof(pTmpHwnds[0]),sizetmphwnds);
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


int GetProcWindHandles(HANDLE hProc,HWND **pphWnds,int *pSize)
{
    int ret;
    int num=0;
    HWND *pRetWnds=*pphWnds;
    int retsize = *pSize;
    PROCESS_THREAD_IDS_t *pThreadIds=NULL;
    BOOL bret;


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

    if(retsize > 0 && pRetWnds == NULL)
    {
        ret = ERROR_INVALID_PARAMETER;
        goto fail;
    }

    pThreadIds =(PROCESS_THREAD_IDS_t*) calloc(sizeof(*pThreadIds),1);
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

    bret = EnumWindows(EnumWindowsProc,(LPARAM)pThreadIds);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    /*now first to enum process and make it ok*/
    if(retsize < pThreadIds->m_NumHwnds)
    {
        assert(pThreadIds->m_pHwnds);
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


/*******************************************************
*      windows algorithm is like this first to find the appwindow
*      then find the window has the owner window of the appwindow to store the ownerwindows
*      then find the window has the owner window of the ownerwindows
*******************************************************/
int GetTopWinds(HWND *pWnds,int wndnum,HWND **ppTopWnds,int *pTopSize)
{
    BOOL bret;
    HWND *pRetTopWnds = *ppTopWnds;
    HWND *pOwnWnds=NULL,*pOwnedWnds=NULL,*pTmpWnds=NULL,*pRemoveWnds=NULL;
    int ownwndnum=0,ownedwndnum=0,tmpwndsize=0,rmnum=0;
    int ownwndsize=0,ownedwndsize=0,rmsize=0;
    int rettopsize = *pTopSize,tmptopsize=0;
    int frmidx=-1;
    int ret;
    int num = 0;
    int i,j,k;
    HWND hOwnWin=NULL;
    WINDOWINFO info;
    std::vector<HWND> hOwnVecs;
    HWND hAppWnd = NULL;
    int mustrm=0;

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

    for(i=0; i<wndnum; i++)
    {
        /*to get the GetWindow GW_OWNER ,for the windows*/
        hOwnWin = GetWindow(pWnds[i],GW_OWNER);
        hOwnVecs.push_back(hOwnWin);
        DEBUG_INFO("[%d] hwnd 0x%08x owned by 0x%08x\n",i,pWnds[i],hOwnWin);
    }

    for(i=0; i<wndnum; i++)
    {
        info.cbSize = sizeof(info);
        bret = GetWindowInfo(pWnds[i],&info);
        if(!bret)
        {
            continue;
        }

        /*now to get the app windows*/
        if(info.dwExStyle & WS_EX_APPWINDOW)
        {
            hAppWnd = pWnds[i];
            break;
        }
    }

    if(hAppWnd == NULL)
    {
        ERROR_INFO("could not find the appwindows\n");
        return 0;
    }

    if(pOwnWnds == NULL)
    {
        ownwndsize = wndnum;
        pOwnWnds =(HWND*) calloc(sizeof(pOwnWnds[0]),ownwndsize);
        if(pOwnWnds == NULL)
        {
            ret = LAST_ERROR_CODE();
            goto fail;
        }
    }

    if(pOwnedWnds == NULL)
    {
        ownedwndsize = wndnum;
        pOwnedWnds =(HWND*) calloc(sizeof(pOwnedWnds[0]),ownedwndsize);
        if(pOwnedWnds == NULL)
        {
            ret = LAST_ERROR_CODE();
            goto fail;
        }
    }
    if(pRemoveWnds == NULL)
    {
        rmsize = wndnum;
        pRemoveWnds = (HWND*) calloc(sizeof(pRemoveWnds[0]),rmsize);
        if(pRemoveWnds == NULL)
        {
            ret = LAST_ERROR_CODE();
            goto fail;
        }
    }

    /*to set the root*/
    ownwndnum = 1;
    pOwnWnds[0] = hAppWnd;
    /*now we should test for the owned wnds*/
    do
    {
        ownedwndnum = 0;
        rmnum = 0;
        memset(pRemoveWnds,0,sizeof(pRemoveWnds[0])*rmsize);
		memset(pOwnedWnds,0,);
        for(i=0; i<wndnum; i++)
        {
            for(j=0; j<ownwndnum; j++)
            {
                DEBUG_INFO("hOwnVecs[%d] 0x%08x pOwnWnds[%d] 0x%08x\n",i,hOwnVecs[i],
                           j,pOwnWnds[j]);
                if(hOwnVecs[i] == pOwnWnds[j] && hOwnVecs[i])
                {
                    DEBUG_INFO("Set[%d] 0x%08x\n",ownedwndnum,pWnds[i]);
					assert(ownedwndnum < ownedwndsize);
                    pOwnedWnds[ownedwndnum] = pWnds[i];
                    ownedwndnum ++;

                    /***********************************************
                                 if we find the ownwnds ,so replace it from the owned window, just like this one
                                 a <= b
                                 a <= c
                                 b <= e
                                 d <= f

                                 it will at last for
                                 first step
                                      a d
                                second
                                      b c d
                                third
                                      e c f
                                ***********************************************/
                    frmidx = -1;
                    for(k=0; k<rmnum; k++)
                    {
                        if(pRemoveWnds[k] == pOwnWnds[j])
                        {
                            frmidx = k;
                            break;
                        }
                    }

                    if(frmidx < 0)
                    {
                    	assert(rmnum < rmsize);
                        pRemoveWnds[rmnum] = pOwnedWnds[j];
                        rmnum ++;
                    }
                }
            }
        }

        if(ownedwndnum > 0)
        {
            /*we should use */
            for(i = 0; i< ownwndnum; i++)
            {
                mustrm = 0;
                for(j=0; j<rmnum; j++)
                {
                    if(pOwnWnds[i] == pRemoveWnds[j])
                    {
                        mustrm = 1;
                        break;
                    }
                }

                if(mustrm == 0)
                {
                    pOwnedWnds[ownedwndnum] = pOwnWnds[i];
                    ownedwndnum ++;
                }
            }
            memset(pOwnWnds,0,sizeof(pOwnWnds[0])*ownwndsize);
            memcpy(pOwnWnds,pOwnedWnds,sizeof(pOwnWnds[0])*ownedwndnum);
            ownwndnum = ownedwndnum;
            memset(pOwnedWnds,0,sizeof(pOwnedWnds[0])*ownedwndsize);
        }

    }
    while(ownedwndnum > 0);

    DEBUG_INFO("ownwndnum %d\n",ownwndnum);
    num = ownwndnum;
    if(ownwndnum > 0)
    {
        if(ownwndnum > rettopsize)
        {
            rettopsize = ownwndnum;
            pRetTopWnds =(HWND*) calloc(sizeof(*pRetTopWnds),rettopsize);
            if(pRetTopWnds == NULL)
            {
                ret = LAST_ERROR_CODE();
                goto fail;
            }
        }

        memset(pRetTopWnds,0,sizeof(pRetTopWnds[0])*rettopsize);
        memcpy(pRetTopWnds,pOwnWnds,sizeof(pOwnWnds[0])*ownwndnum);
    }
    else if(pRetTopWnds)
    {
        if(rettopsize > 0)
        {
            memset(pRetTopWnds,0,sizeof(pRetTopWnds[0])*rettopsize);
        }
    }

    if(pOwnWnds)
    {
        free(pOwnWnds);
    }
    pOwnWnds = NULL;
    if(pOwnedWnds)
    {
        free(pOwnedWnds);
    }
    pOwnedWnds = NULL;
    if(pTmpWnds)
    {
        free(pTmpWnds);
    }
    pTmpWnds = NULL;

    if(pRemoveWnds)
    {
        free(pRemoveWnds);
    }
    pRemoveWnds=NULL;


    if(*ppTopWnds && *ppTopWnds != pRetTopWnds)
    {
        free(*ppTopWnds);
    }

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
    if(pOwnWnds)
    {
        free(pOwnWnds);
    }
    pOwnWnds = NULL;
    if(pOwnedWnds)
    {
        free(pOwnedWnds);
    }
    pOwnedWnds = NULL;
    if(pTmpWnds)
    {
        free(pTmpWnds);
    }
    pTmpWnds = NULL;

    if(pRemoveWnds)
    {
        free(pRemoveWnds);
    }
    pRemoveWnds=NULL;
    SetLastError(ret);
    return -ret;
}

