
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
    HANDLE hProcSnap=NULL,hModSnap=NULL;
    int ret;
    BOOL bret,b2ret;
    int numget=0;
    unsigned int *pRetPids = *ppPids;
    unsigned int *pTmpPids=NULL;
    int pidretsize=*pPidsSize;

    MODULEENTRY32 mentry;
    PROCESSENTRY32 pentry;
    int i,j,k;

    DEBUG_INFO("\n");
    if(pidretsize == 0 && pRetPids)
    {
        ret = ERROR_INVALID_PARAMETER;
        goto fail;
    }
    DEBUG_INFO("\n");

    hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(hProcSnap == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not create module32 error(%d)\n",ret);
        goto fail;
    }
    DEBUG_INFO("\n");

	memset(&pentry,0,sizeof(pentry));
    pentry.dwSize =sizeof(pentry);

    for(i=0,pentry.dwSize = sizeof(pentry),bret = Process32First(hProcSnap,&pentry); !bret; i++,pentry.dwSize= sizeof(pentry),bret = Process32Next(hProcSnap,&pentry))
    {
    	DEBUG_INFO("i = %d\n",i);
        assert(hModSnap == NULL);
        hModSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pentry.th32ProcessID);
        if(hModSnap == NULL)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("could not create module32 error(%d)\n",ret);
            continue;
        }

		memset(&mentry,0,sizeof(mentry));
        for(j=0,mentry.dwSize = sizeof(mentry),b2ret=Module32First(hModSnap,&mentry); !b2ret; j++,mentry.dwSize = sizeof(mentry),b2ret=Module32Next(hModSnap,&mentry))
        {
#ifdef _UNICODE
            ret = UnicodeToAnsi(mentry.szModule,&pCmpDllName,&cmpdllnamesize);
            if(ret < 0)
            {
                ret = LAST_ERROR_CODE();
                ERROR_INFO("could not get name at %d error(%d)\n",i,ret);
                goto fail;
            }
#else
            pCmpDllName = mentry.szModule;
#endif
            DEBUG_INFO("pCmpDllName %s pPartDll %s\n",pCmpDllName,pPartDll);
            if(strcmp(pCmpDllName,pPartDll)==0)
            {
                /*to check if the process id is in the array*/
                if(numget > 0)
                {
                    for(k=0; k<numget; k++)
                    {
                        if(pRetPids[k] == pentry.th32ProcessID)
                        {
                            /*has in the array ,so we do not insert into it*/
                            goto next_cycle;
                        }
                    }
                }

                /*insert into the array*/
                if(numget < pidretsize)
                {
                    pRetPids[numget] = pentry.th32ProcessID;
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
                    pRetPids[numget] = pentry.th32ProcessID;
                }
                numget ++;
                goto next_cycle;
            }
        }
next_cycle:
        if(hModSnap)
        {
            CloseHandle(hModSnap);
        }
        hModSnap = NULL;
    }

    DEBUG_INFO("scan %d process(es) error(%d)\n",i,LAST_ERROR_CODE());

    if(*ppPids != pRetPids && *ppPids)
    {
        free(*ppPids);
    }
    *ppPids = pRetPids;
    *pPidsSize = pidretsize;
    assert(pTmpPids == NULL);
#ifdef _UNICODE
    UnicodeToAnsi(NULL,&pCmpDllName,&cmpdllnamesize);
#endif

    if(hModSnap)
    {
        CloseHandle(hModSnap);
    }
    hModSnap = NULL;
    if(hProcSnap)
    {
        CloseHandle(hProcSnap);
    }
    hProcSnap = NULL;

    return numget;

fail:
#ifdef _UNICODE
    UnicodeToAnsi(NULL,&pCmpDllName,&cmpdllnamesize);
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

    if(hModSnap)
    {
        CloseHandle(hModSnap);
    }
    hModSnap = NULL;
    if(hProcSnap)
    {
        CloseHandle(hProcSnap);
    }
    hProcSnap = NULL;
    SetLastError(ret);
    return -ret;
}

