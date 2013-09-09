
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
    DEBUG_INFO("Start ++++++++++++++++++++\n");
    for(i=0,procinfo.dwSize = sizeof(procinfo),bret=Process32First(hSnap,&procinfo); bret; i++,procinfo.dwSize=sizeof(procinfo),bret=Process32Next(hSnap,&procinfo))
    {

#ifdef _UNICODE
        ret = _wcsicmp(pExeNameW,procinfo.szExeFile);
        //DEBUG_INFO("szExeFile %S pExeNameW %S(%s) (%d)\n",procinfo.szExeFile,pExeNameW,exename ,ret);
#else
        ret = _stricmp(exename,procinfo.szExeFile);
        DEBUG_INFO("szExeFile %s exename(%s) (%d)\n",procinfo.szExeFile,exename,ret);
#endif
        if(ret == 0)
        {
            /*it is compare ok ,so we should set it for the compare */
            DEBUG_INFO("Find ||||||||||||||||||||||||||||| parent processid %d\n",procinfo.th32ParentProcessID);

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
                pTmpPids = NULL;
            }
            count ++;
        }
    }
    DEBUG_INFO("End ++++++++++++++++++++\n");

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

#if 0
int ProcMemorySize(unsigned int processid,unsigned int*pMemSize)
{
    HEAPLIST32 hl;
    int ret=0;
    HANDLE hHeapSnap = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, processid);
    unsigned int blksize=0;

    hl.dwSize = sizeof(HEAPLIST32);

    if(hHeapSnap == INVALID_HANDLE_VALUE)
    {
        ERROR_INFO("CreateToolhelp32Snapshot failed (%d)\n", GetLastError());
        return -1;
    }

    *pMemSize = 0;
    if(Heap32ListFirst(hHeapSnap, &hl))
    {
        do
        {
            HEAPENTRY32 he;
            ZeroMemory(&he, sizeof(HEAPENTRY32));
            he.dwSize = sizeof(HEAPENTRY32);

            if(Heap32First(&he, processid, hl.th32HeapID))
            {
                //DEBUG_INFO("\nHeap ID: %d\n", hl.th32HeapID);
                do
                {
                    //DEBUG_INFO("Block size: %d\n", he.dwBlockSize);
                    blksize += he.dwBlockSize;

                    he.dwSize = sizeof(HEAPENTRY32);
                }
                while(Heap32Next(&he));
            }
            hl.dwSize = sizeof(HEAPLIST32);
        }
        while(Heap32ListNext(hHeapSnap, &hl));

        *pMemSize = blksize;
        DEBUG_INFO("processid %d memsize 0x%08x\n",processid,blksize);
    }
    else
    {
        ERROR_INFO("Cannot list first heap (%d)\n", GetLastError());
        ret = -3;
    }

    CloseHandle(hHeapSnap);

    return ret;
}

#else

#if 0



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


int ProcMemorySize(unsigned int processid,unsigned int * pMemSize)
{
    int ret;
    HANDLE hSnap=INVALID_HANDLE_VALUE;
    HEAPLIST32 heapinfo;
    int count =0;
    int i,j;
    BOOL bret,b2ret;
    HEAPENTRY32 hentry;
    unsigned int blksize=0;

    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST,processid);
    if(hSnap == INVALID_HANDLE_VALUE)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not get the create snap shot error(%d)\n",ret);
        goto fail;
    }

    SetLastError(0);
    for(i=0,heapinfo.dwSize = sizeof(heapinfo),bret=Heap32ListFirst(hSnap,&heapinfo); bret; i++,heapinfo.dwSize=sizeof(heapinfo),bret=Heap32ListNext(hSnap,&heapinfo))
    {
        DEBUG_INFO("heap processid %d\n",heapinfo.th32ProcessID);
        if(processid == heapinfo.th32ProcessID)
        {
            /*it is compare ok ,so we should set it for the compare */
            SetLastError(0);
            for(j=0,hentry.dwSize = sizeof(hentry),b2ret=Heap32First(&hentry,processid,heapinfo.th32HeapID); !b2ret; j++,hentry.dwSize = sizeof(hentry),b2ret=Heap32Next(&hentry))
            {
                blksize += hentry.dwBlockSize;
                DEBUG_INFO("Get Processid(%d) heapid (%d) size 0x%08x\n",processid,heapinfo.th32HeapID,hentry.dwBlockSize);
            }

            ret = LAST_ERROR_CODE();
            if(ret != ERROR_NO_MORE_FILES)
            {
                ERROR_INFO("could not get processid (%d) heapid (%d) error %d\n",processid,heapinfo.th32HeapID,ret);
                goto fail;
            }
            SetLastError(0);

        }
    }

    ret = LAST_ERROR_CODE();

    if(ret != ERROR_NO_MORE_FILES)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }
    *pMemSize = blksize;

    CloseHandle(hSnap);
    return 1;
fail:
    assert(ret > 0);

    if(hSnap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hSnap);
    }
    hSnap = INVALID_HANDLE_VALUE;
    SetLastError(ret);
    return -ret;
}

#else


int ProcMemorySize(unsigned int processid ,unsigned int* pMemSize)
{
    HANDLE hProc=NULL;
    int ret;
    BOOL bret;
    PROCESS_MEMORY_COUNTERS procmem;

	*pMemSize = 0;
    hProc = OpenProcess(PROCESS_QUERY_INFORMATION ,FALSE,processid);
    if(hProc == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not open process %d error(%d)\n",processid,ret);
        goto fail;
    }

    DEBUG_INFO("processid %d open handle 0x%08x\n",processid,hProc);

	procmem.cb = sizeof(procmem);
    bret = GetProcessMemoryInfo(hProc,&procmem,sizeof(procmem));
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not get processmemory info %d\n",ret);
        goto fail;
    }

	*pMemSize = procmem.WorkingSetSize;
	DEBUG_INFO("process %d MEMSIZE %d\n",processid,procmem.WorkingSetSize);


    CloseHandle(hProc);

    return 0;

fail:
    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;
    return -ret;
}

#endif
#endif

int EnableDebugLevel(int enable)
{
    int oldenable=0;
    int ret;
    HANDLE hToken=NULL;
    BOOL bret;
    TOKEN_PRIVILEGES tp,*poldtp=NULL;
    DWORD oldsize=0,oldcount=1,oldlen;
    unsigned int i;

    bret = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken);
    if(!bret)
    {
        hToken= NULL;
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not opentoken error(%d)\n",ret);
        goto fail;
    }

    tp.PrivilegeCount = 1;
    bret = LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&(tp.Privileges[0].Luid));
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not lookup %s name error(%d)\n",
                   SE_DEBUG_NAME,ret);
        goto fail;
    }
    DEBUG_INFO("se_debug_name %S luid (%ld:%ld) attribute 0x%08x\n",SE_DEBUG_NAME,tp.Privileges[0].Luid.HighPart,tp.Privileges[0].Luid.LowPart,tp.Privileges[0].Attributes);

    if(enable)
    {
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    else
    {
        tp.Privileges[0].Attributes = 0;
    }

    oldsize = sizeof(*poldtp);
    poldtp = (TOKEN_PRIVILEGES*) calloc(oldsize,1);
    if(poldtp == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not allocate %d size\n",oldsize);
        goto fail;
    }
    poldtp->PrivilegeCount = oldcount;
    poldtp->Privileges[0].Luid = tp.Privileges[0].Luid;
    while(1)
    {

        SetLastError(0);
        oldlen = oldsize;
        DEBUG_INFO("oldsize %d sizeof(%d) oldcount (%d)\n",oldsize,sizeof(tp),oldcount);
        //bret =  AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),poldtp,&oldlen);
        bret =  AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL);
        if(bret)
        {
            break;
        }


        oldsize += sizeof(poldtp->Privileges[0]);
        free(poldtp);
        poldtp = (TOKEN_PRIVILEGES*) calloc(oldsize,1);
        if(poldtp == NULL)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("could not allocate %d size\n",oldsize);
            goto fail;
        }
        oldcount ++;
        poldtp->PrivilegeCount = oldcount;
        poldtp->Privileges[0].Luid = tp.Privileges[0].Luid;
    }

    for(i=0; i<poldtp->PrivilegeCount; i++)
    {
        if(memcmp(&(tp.Privileges[0].Luid),&(poldtp->Privileges[i].Luid),sizeof(tp.Privileges[0].Luid))==0)
        {
            if(poldtp->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED)
            {
                oldenable = 1;
            }
        }
    }

    if(hToken)
    {
        CloseHandle(hToken);
    }
    hToken = NULL;


    return oldenable;
fail:
    assert(ret);
    if(hToken)
    {
        CloseHandle(hToken);
    }
    hToken = NULL;
    SetLastError(ret);
    return -ret;
}


