// dllbase.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "uniansi.h"
#include <Windows.h>
#include <Psapi.h>
#include <assert.h>

#define  DEBUG_INFO(fmt,...) do{fprintf(stderr,fmt,__VA_ARGS__);}while(0)
#define  ERROR_INFO(fmt,...) do{fprintf(stderr,fmt,__VA_ARGS__);}while(0)


#define LAST_ERROR_RETURN()  ((int)(GetLastError() ? GetLastError() : 1))



PVOID __GetModuleBaseAddr(unsigned int processid,const char* pDllName)
{
    /*first to open process*/
    HANDLE hProc=NULL;
    BOOL bret;
    int ret;
    PVOID pBaseAddr=NULL;
    HMODULE *pHModules=NULL;
    DWORD modulesize=1024;
    DWORD modulelen=0,moduleret=0;
    DWORD i;
    MODULEINFO modinfo;
    int namesize=1024;
#ifdef _UNICODE
    wchar_t *pNameWide=NULL;
    char *pNameAnsi=NULL;
    int ansisize = 0;
#else
    char *pNameAnsi=NULL;
#endif
    char *pPartName=NULL;



    hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,processid);
    if(hProc == NULL)
    {
        ret = LAST_ERROR_RETURN();
        ERROR_INFO("Could not Open(%d) error(%d)\n",processid,ret);
        goto fail;
    }
    pHModules = (HMODULE*)calloc(sizeof(*pHModules),modulesize);
    if(pHModules == NULL)
    {
        ret = LAST_ERROR_RETURN();
        goto fail;
    }


    bret = EnumProcessModules(hProc,pHModules,modulesize*sizeof(*pHModules),&moduleret);
    if(!bret)
    {
        ret = LAST_ERROR_RETURN();
        ERROR_INFO("[%d] get modulesize %d error(%d)\n",processid,modulesize,ret);
        free(pHModules);
        pHModules = NULL;
        modulesize = ((moduleret +1)/ sizeof(*pHModules));
        pHModules = (HMODULE*)calloc(sizeof(*pHModules),modulesize);
        if(pHModules == NULL)
        {
            ret = LAST_ERROR_RETURN();
            goto fail;
        }
        bret = EnumProcessModules(hProc,pHModules,modulesize*sizeof(*pHModules),&moduleret);
        if(!bret)
        {
            ret =LAST_ERROR_RETURN();
            ERROR_INFO("[%d] get modulesize %d error(%d)\n",processid,modulesize,ret);
            goto fail;
        }
    }

    modulelen = moduleret / sizeof(*pHModules);

#ifdef _UNICODE
    pNameWide = (wchar_t*)calloc(sizeof(*pNameWide),namesize);
    if(pNameWide == NULL)
    {
        ret =LAST_ERROR_RETURN();
        goto fail;
    }
#else  /*_UNICODE*/
    pNameAnsi = (char*)calloc(sizeof(*pNameAnsi),namesize);
    if(pNameAnsi == NULL)
    {
        ret = LAST_ERROR_RETURN();
        goto fail;
    }
#endif  /*_UNICODE*/

    for(i=0; i<modulelen; i++)
    {
#ifdef _UNICODE
        bret = GetModuleFileNameEx(hProc,pHModules[i],pNameWide,namesize);
#else
        bret = GetModuleFileNameEx(hProc,pHModules[i],pNameAnsi,namesize);
#endif
        if(!bret)
        {
            ret =LAST_ERROR_RETURN();
            ERROR_INFO("[%d] process->[%d]module(0x%08x) error(%d)\n",processid,i,pHModules[i],ret);
            continue;
        }

#ifdef _UNICODE
        ret = UnicodeToAnsi(pNameWide,&pNameAnsi,&ansisize);
        if(ret < 0)
        {
            ret = LAST_ERROR_RETURN();
            goto fail;
        }
#endif
        pPartName = strrchr(pNameAnsi,'\\');
        if(pPartName)
        {
            pPartName += 1;
        }
        else
        {
            pPartName = pNameAnsi;
        }

        if(pDllName == NULL || _stricmp(pPartName,pDllName)==0)
        {
            /*now get the module information*/
            bret = GetModuleInformation(hProc,pHModules[i],&modinfo,sizeof(modinfo));
            if(!bret)
            {
                ret = LAST_ERROR_RETURN();
                ERROR_INFO("[%d]process->[%d](0x%08x) dll(%s) get info error(%d)\n",
                           processid,i,pHModules[i],pNameAnsi,ret);
                goto fail;
            }

            pBaseAddr = modinfo.lpBaseOfDll;
            break;
        }

    }

    if(pBaseAddr==NULL)
    {
        ret = ERROR_MOD_NOT_FOUND;
        ERROR_INFO("[%d] not found %s\n",processid,pDllName);
        goto fail;
    }

#ifdef _UNICODE
    if(pNameWide)
    {
        free(pNameWide);
    }
    pNameWide = NULL;
    UnicodeToAnsi(NULL,&pNameAnsi,&ansisize);
#else  /*_UNICODE*/
    if(pNameAnsi)
    {
        free(pNameAnsi);
    }
    pNameAnsi = NULL;
#endif  /*_UNICODE*/
    namesize = 0;

    if(pHModules)
    {
        free(pHModules);
    }
    pHModules = NULL;
    modulelen = 0;
    modulesize = 0;
    moduleret = 0;

    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;


    SetLastError(0);
    return pBaseAddr;
fail:
    assert(ret > 0);

#ifdef _UNICODE
    if(pNameWide)
    {
        free(pNameWide);
    }
    pNameWide = NULL;
    UnicodeToAnsi(NULL,&pNameAnsi,&ansisize);
#else  /*_UNICODE*/
    if(pNameAnsi)
    {
        free(pNameAnsi);
    }
    pNameAnsi = NULL;
#endif  /*_UNICODE*/
    namesize = 0;

    if(pHModules)
    {
        free(pHModules);
    }
    pHModules = NULL;
    modulelen = 0;
    modulesize = 0;
    moduleret = 0;

    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;
    SetLastError(ret);
    return NULL;
}



int main(int argc,char* argv[])
{
    unsigned int pid=0;
    char *pDllName=NULL;
    PVOID pBaseAddr=NULL;
    int ret;
    if(argc < 3)
    {
        fprintf(stderr,"%s pid dllname\n",argv[1]);
        return -3;
    }

    pid = atoi(argv[1]);
    pDllName = argv[2];

    while(1)
    {
        pBaseAddr = __GetModuleBaseAddr(pid,(const char*)pDllName);
        if(pBaseAddr == NULL)
        {
            ret = LAST_ERROR_RETURN();
            fprintf(stderr,"could not get %d %s error(%d)\n",pid,pDllName,ret);
        }
        else
        {
            fprintf(stdout,"[%d] dll(%s) baseaddr 0x%p\n",pid,pDllName,pBaseAddr);
        }

        Sleep(1000);
    }


    return 0;
}
