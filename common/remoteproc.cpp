
#include "remoteproc.h"
#include <assert.h>
#include <Windows.h>
#include "output_debug.h"
#include <TlHelp32.h>
#include "uniansi.h"
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <psapi.h>

#pragma comment(lib,"psapi.lib")

#define LAST_ERROR_CODE()  (GetLastError() ? GetLastError() : 1)

typedef unsigned long ptr_t;


int ProcRead(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen)
{
    HANDLE hProc=NULL;
    int ret;
    int readlen=0;
    DWORD curret;
    unsigned char* pCurPtr=pData;
    unsigned char* pCurRemote = (unsigned char*)pRemoteAddr;
    BOOL bret;


    hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION  | PROCESS_VM_READ ,FALSE,processid);
    if(hProc == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not open process(%d) (%d)\n",processid,ret);
        goto fail;
    }

    /*now to read*/
    readlen = 0;
    while(readlen < datalen)
    {
        bret = ReadProcessMemory(hProc,pCurRemote,pCurPtr,(datalen - readlen),&curret);
        if(!bret)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("could not read at (0x%p from 0x%p) at readlen(%d) error(%d)\n",
                       pCurRemote,pCurPtr,readlen,ret);
            goto fail;
        }
        readlen += curret;
        pCurRemote += curret;
        pCurPtr += curret;
    }


    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;

    return readlen;

fail:
    assert(ret > 0);
    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;
    SetLastError(ret);
    return -ret;
}



int ProcWrite(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen,int force)
{
    HANDLE hProc=NULL;
    int ret,res;
    int writelen=0;
    DWORD curret;
    unsigned char* pCurPtr=pData;
    unsigned char* pCurRemote = (unsigned char*)pRemoteAddr;
    BOOL bret;
    DWORD origbase,curbase;
    int setcurbase=0;


    hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION  | PROCESS_VM_WRITE | PROCESS_VM_READ,FALSE,processid);
    if(hProc == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not open process(%d) (%d)\n",processid,ret);
        goto fail;
    }

    /*now first query the */
    if(force)
    {
        curbase = PAGE_EXECUTE_READWRITE ;
        bret = VirtualProtectEx(hProc,pRemoteAddr,datalen,curbase,&origbase);
        if(!bret)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("could not query at (process %d remote 0x%p - 0x%p) curbase 0x%08x error(%d)\n",
                       processid,pRemoteAddr,((ptr_t)pRemoteAddr+datalen),curbase,ret);
            goto fail;
        }
        setcurbase = 1;
    }

    /*now to read*/
    writelen = 0;
    while(writelen < datalen)
    {
        bret = WriteProcessMemory(hProc,pCurRemote,pCurPtr,(datalen - writelen),&curret);
        if(!bret)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("could not write at (0x%p from 0x%p) at writelen(%d) error(%d)\n",
                       pCurRemote,pCurPtr,writelen,ret);
            goto fail;
        }
        writelen += curret;
        pCurRemote += curret;
        pCurPtr += curret;
    }

    if(setcurbase)
    {

        bret = VirtualProtectEx(hProc,pRemoteAddr,datalen,origbase,&curbase);
        if(!bret)
        {
            res = LAST_ERROR_CODE();
            ERROR_INFO("could not reset (process %d addr 0x%p - 0x%p) with orig 0x%08x error (%d)\n",
                       processid,pRemoteAddr,((ptr_t)pRemoteAddr+datalen),origbase,res);
        }
    }
    setcurbase =0;

    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;
    return writelen;

fail:
    assert(ret > 0);
    if(setcurbase)
    {

        bret = VirtualProtectEx(hProc,pRemoteAddr,datalen,origbase,&curbase);
        if(!bret)
        {
            res = LAST_ERROR_CODE();
            ERROR_INFO("could not reset (process %d addr 0x%p - 0x%p) with orig 0x%08x error (%d)\n",
                       processid,pRemoteAddr,((ptr_t)pRemoteAddr+datalen),origbase,res);
        }
    }
    setcurbase =0;
    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;
    SetLastError(ret);
    return -ret;
}




int ProcKill(unsigned int processid,int force)
{
    BOOL bret;
    int ret;
    HANDLE hProc=NULL;

    hProc = OpenProcess(PROCESS_TERMINATE,FALSE,processid);
    if(hProc==NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not open process (%d)(%d)\n",processid,ret);
        goto fail;
    }

    bret = TerminateProcess(hProc,0);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not kill process(%d:0x%08x) error(%d)\n",processid,hProc,ret);
        goto fail;
    }
    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;

    return 0;
fail:
    assert(ret > 0);
    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;
    return -ret;
}


int ProcEnum(const char * exename,unsigned int ** ppPids,int * pSize)
{
    int ret;
    HANDLE hSnap=INVALID_HANDLE_VALUE;
    PROCESSENTRY32 procinfo;
    int count =0;
    int i;
    BOOL bret;
    unsigned int *pRetPids=*ppPids;
    unsigned int *pTmpPids=NULL;
    int retsize = *pSize;
#ifdef _UNICODE
    wchar_t *pExeNameW=NULL;
    int exenamesize =0,exnamelen=0;

    ret = AnsiToUnicode((char*)exename,&pExeNameW,&exenamesize);
    if(ret < 0)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not change (%s) error (%d)\n",exename,ret);
        goto fail;
    }
#endif

    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(hSnap == INVALID_HANDLE_VALUE)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not get the create snap shot error(%d)\n",ret);
        goto fail;
    }

    procinfo.dwSize = sizeof(procinfo);
    for(i=0,procinfo.dwSize = sizeof(procinfo),bret=Process32First(hSnap,&procinfo); bret; i++,procinfo.dwSize=sizeof(procinfo),bret=Process32Next(hSnap,&procinfo))
    {
#ifdef _UNICODE
        ret = _wcsicmp(pExeNameW,procinfo.szExeFile);
#else
        ret = _stricmp(exename,procinfo.szExeFile);
#endif
        if(ret == 0)
        {
            /*it is compare ok ,so we should set it for the compare */

            if(retsize <(count + 1))
            {
                retsize = (count + 1);
                assert(pTmpPids == NULL);
                pTmpPids = (unsigned int*)malloc(retsize * sizeof(*pTmpPids));
                if(pTmpPids == NULL)
                {
                    ret = LAST_ERROR_CODE();
                    goto fail;
                }

                if(count > 0)
                {
                    memcpy(pTmpPids,pRetPids,count * sizeof(*pTmpPids));
                }
                if(pRetPids && pRetPids != *ppPids)
                {
                    free(pRetPids);
                    pRetPids = NULL;
                }
                pRetPids = pTmpPids;
                pRetPids[count] = procinfo.th32ProcessID;
                count ++;
                pTmpPids = NULL;
            }
        }
    }

    ret = LAST_ERROR_CODE();
    if(ret != ERROR_NO_MORE_FILES)
    {
        ERROR_INFO("could not get snapshot for (%d) error(%d)\n",i,ret);
        goto fail;
    }

    assert(pTmpPids == NULL);
    CloseHandle(hSnap);
    if(*ppPids && pRetPids != *ppPids)
    {
        free(*ppPids);
    }
    *ppPids = pRetPids;
    *pSize = retsize;
    return count;
fail:
    assert(ret > 0);
    if(pTmpPids)
    {
        free(pTmpPids);
    }
    pTmpPids = NULL;

    if(pRetPids && pRetPids != *ppPids)
    {
        free(pRetPids);
    }
    pRetPids=NULL;

    if(hSnap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hSnap);
    }
    hSnap = INVALID_HANDLE_VALUE;
    SetLastError(ret);
    return -ret;
}

int ProcMemorySize(unsigned int processid,unsigned int * pMemSize)
{
    HANDLE hProc=NULL;
    int ret;
    BOOL bret;
    PROCESS_MEMORY_COUNTERS proccounter;

    hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,processid);
    if(hProc == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not open processid (%d) error(%d)\n",processid,ret);
        goto fail;
    }

    proccounter.cb = sizeof(proccounter);
    bret = GetProcessMemoryInfo(hProc,&proccounter,sizeof(proccounter));
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not query info (%d) error(%d)\n",processid,ret);
    }

	*pMemSize = proccounter.WorkingSetSize;

    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;

    return 0;
fail:
    assert(ret > 0);
    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;
    return -ret;
}

