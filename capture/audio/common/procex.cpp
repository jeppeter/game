
#include <procex.h>
#include <Windows.h>
#include <uniansi.h>
#include <output_debug.h>
#include <TlHelp32.h>
#include <assert.h>

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))


int GetModuleInsertedProcess(const char * pPartDll,unsigned int **ppPids,int * pPidsSize)
{
    char *pCmpDllName=NULL;
#ifdef _UNICODE
    int cmpdllnamesize=0;
#endif
    HANDLE hSnap=NULL;
    int ret;
    BOOL bret;
    int numget=0;
    unsigned int *pRetPids = *ppPids;
    unsigned int *pTmpPids=NULL;
    int pidretsize=*pPidsSize;

    MODULEENTRY32 entry;
    int i,j;

    if(pidretsize == 0 && pRetPids)
    {
        ret = ERROR_INVALID_PARAMETER;
        goto fail;
    }

    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32,0);
    if(hSnap == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not create module32 error(%d)\n",ret);
        goto fail;
    }

    for(i = 0,entry.dwSize = sizeof(entry),bret = Module32First(hSnap,&entry); bret; i++,entry.dwSize=sizeof(entry),bret=Module32Next(hSnap,&entry))
    {
#ifdef _UNICODE
        ret = UnicodeToAnsi(entry.szModule,&pCmpDllName,&cmpdllnamesize);
        if(ret < 0)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("could not get name at %d error(%d)\n",i,ret);
            goto fail;
        }
#else
        pCmpDllName = entry.szModule;
#endif
        if(strcmp(pCmpDllName,pPartDll)==0)
        {
            /*to check if the process id is in the array*/
            if(numget > 0)
            {
                for(j=0; j<numget; j++)
                {
                    if(pRetPids[j] == entry.th32ProcessID)
                    {
                        /*has in the array ,so we do not insert into it*/
                        goto next_cycle;
                    }
                }
            }

            /*insert into the array*/
            if(numget < pidretsize)
            {
                pRetPids[numget] = entry.th32ProcessID;
            }
            else
            {
                if(pidretsize == 0)
                {
                    assert(pRetPids == NULL);
                    pidretsize = 4;
                    pRetPids =(unsigned int*) calloc(sizeof(*pRetPids),pidretsize);
                    if(pRetPids == NULL)
                    {
                        ret = LAST_ERROR_CODE();
                        goto fail;
                    }
                }
                else
                {
                    /*now to copy the memory*/
                    pidretsize <<= 1;
                    pTmpPids = (unsigned int*)calloc(sizeof(*pTmpPids),pidretsize);
                    if(pTmpPids == NULL)
                    {
                        ret = LAST_ERROR_CODE();
                        goto fail;
                    }
                    if(numget)
                    {
                        memcpy(pTmpPids,pRetPids,numget*sizeof(*pTmpPids));
                    }

                    if(pRetPids && pRetPids != *ppPids)
                    {
                        free(pRetPids);
                    }
                    pRetPids = pTmpPids;
                    pTmpPids = NULL;

                }
                pRetPids[numget] = entry.th32ProcessID;
            }
            numget ++;
        }
next_cycle:
        j = j;
    }


    if(*ppPids != pRetPids && *ppPids)
    {
        free(*ppPids);
    }
    *ppPids = pRetPids;
    *pPidsSize = pidretsize;

    return numget;

fail:
#ifdef _UNCIDOE
    UnicodeToAnsi(NULL,&pCmpDllname,&cmpdllnamesize);
#endif

    if(pRetPids && pRetPids != *ppPids)
    {
        free(pRetPids);
    }
    pRetPids = NULL;
    pidretsize = 0;

    if(pTmpPids)
    {
        free(pTmpPids);
    }
    pTmpPids = NULL;

    if(hSnap)
    {
        CloseHandle(hSnap);
    }
    hSnap = NULL;
    SetLastError(ret);
    return -ret;
}

