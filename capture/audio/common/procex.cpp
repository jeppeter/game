
#include <procex.h>
#include <Windows.h>
#include <uniansi.h>
#include <output_debug.h>
#include <TlHelp32.h>
#include <assert.h>
#include <vector>

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))


int __CheckProcessIdModuleInserted(unsigned int procid,const char* pPartDll)
{
    HANDLE hModSnap=INVALID_HANDLE_VALUE;
    int ret;
    BOOL bret;
    char* pCmpDllName=NULL;
    MODULEENTRY32 mentry;
    int curnum=0;
#ifdef _UNICODE
    int cmpdllnamesize=0;
#endif

try_again:
    hModSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,procid);
    if(hModSnap == INVALID_HANDLE_VALUE)
    {
        ret = LAST_ERROR_CODE();
        if(ret != ERROR_BAD_LENGTH)
        {
            //ERROR_INFO("[%d] snap module error(%d)\n",procid,ret);
            goto fail;
        }
        /*we try again for next use*/
        goto try_again;
    }

    mentry.dwSize = sizeof(mentry);

    bret = Module32First(hModSnap,&mentry);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("[%d] first error(%d)\n",procid,ret);
        goto fail;
    }

#ifdef _UNICODE
    ret = UnicodeToAnsi(mentry.szModule,&pCmpDllName,&cmpdllnamesize);
    if(ret < 0)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("[%d]:[%d] get module name error(%d)\n",procid,curnum,ret);
        goto fail;
    }
#else
    pCmpDllName = mentry.szModule;
#endif
    //DEBUG_INFO("[%d]:[%d] modname %s partdll %s\n",procid,curnum,pCmpDllName,pPartDll);
    if(_stricmp(pCmpDllName,pPartDll)==0)
    {
		DEBUG_INFO("[%d] find %s\n",procid,pPartDll);
        goto findout;
    }

    curnum ++;

    while(Module32Next(hModSnap,&mentry))
    {
#ifdef _UNICODE
        ret = UnicodeToAnsi(mentry.szModule,&pCmpDllName,&cmpdllnamesize);
        if(ret < 0)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("[%d]:[%d] get module name error(%d)\n",procid,curnum,ret);
            goto fail;
        }
#else
        pCmpDllName = mentry.szModule;
#endif
        //DEBUG_INFO("[%d]:[%d] modname %s partdll %s\n",procid,curnum,pCmpDllName,pPartDll);
        if(_stricmp(pCmpDllName,pPartDll)==0)
        {
			DEBUG_INFO("[%d] find %s\n",procid,pPartDll);
            goto findout;
        }
        curnum ++;
    }


#ifdef _UNICODE
    UnicodeToAnsi(NULL,&pCmpDllName,&cmpdllnamesize);
#endif
    if(hModSnap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hModSnap);
    }
    hModSnap = INVALID_HANDLE_VALUE;
    return 0;
findout:
#ifdef _UNICODE
    UnicodeToAnsi(NULL,&pCmpDllName,&cmpdllnamesize);
#endif
    if(hModSnap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hModSnap);
    }
    hModSnap = INVALID_HANDLE_VALUE;
    return 1;
fail:
#ifdef _UNICODE
    UnicodeToAnsi(NULL,&pCmpDllName,&cmpdllnamesize);
#endif
    if(hModSnap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hModSnap);
    }
    hModSnap = INVALID_HANDLE_VALUE;
    SetLastError(ret);
    return -ret;
}



int GetModuleInsertedProcess(const char * pPartDll,unsigned int **ppPids,int * pPidsSize)
{
    char *pCmpDllName=NULL;
    std::vector<unsigned int> procvecs;
#ifdef _UNICODE
    int cmpdllnamesize=0;
#endif
    HANDLE hProcSnap=INVALID_HANDLE_VALUE,hModSnap=INVALID_HANDLE_VALUE;
    int ret;
    BOOL bret;
    int numget=0;
    unsigned int *pRetPids = *ppPids;
    unsigned int *pTmpPids=NULL;
    int pidretsize=*pPidsSize;
    PROCESSENTRY32 pentry;
    unsigned int i;

    if(pidretsize == 0 && pRetPids)
    {
        ret = ERROR_INVALID_PARAMETER;
        goto fail;
    }

    hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if(hProcSnap == INVALID_HANDLE_VALUE)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not create module32 error(%d)\n",ret);
        goto fail;
    }

    pentry.dwSize = sizeof(pentry);

    bret = Process32First(hProcSnap,&pentry);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not get process entry error(%d)\n",ret);
        goto fail;
    }


    DEBUG_INFO("Process ID: %d\n", pentry.th32ProcessID);
    ret = __CheckProcessIdModuleInserted(pentry.th32ProcessID,pPartDll);
    if(ret > 0)
    {
        procvecs.push_back(pentry.th32ProcessID);
        numget ++;
    }

    while(Process32Next(hProcSnap,&pentry))
    {
        DEBUG_INFO("Process ID: %d\n",pentry.th32ProcessID);
        ret = __CheckProcessIdModuleInserted(pentry.th32ProcessID,pPartDll);
        if(ret > 0)
        {
            procvecs.push_back(pentry.th32ProcessID);
            numget ++;
        }
    }

    if(procvecs.size() > 0)
    {
        if(pidretsize <(int) procvecs.size())
        {
			pidretsize = procvecs.size();
            pRetPids = (unsigned int*)calloc(sizeof(*pRetPids),pidretsize);
            if(pRetPids == NULL)
            {
                ret = LAST_ERROR_CODE();
                goto fail;
            }
        }
        for(i=0; i<procvecs.size(); i++)
        {
			pRetPids[i] = procvecs[i];
        }
    }

    if(hProcSnap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hProcSnap);
    }
    hProcSnap = INVALID_HANDLE_VALUE;

    if(pRetPids != *ppPids && *ppPids)
    {
        free(*ppPids);
    }
    *ppPids = pRetPids;
    *pPidsSize = pidretsize;

    procvecs.clear();
    return numget;

fail:
    if(hProcSnap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hProcSnap);
    }
    hProcSnap = INVALID_HANDLE_VALUE;

    if(pRetPids != *ppPids && pRetPids)
    {
        free(pRetPids);
    }
    pRetPids = NULL;

    procvecs.clear();

    SetLastError(ret);
    return -ret;

}

