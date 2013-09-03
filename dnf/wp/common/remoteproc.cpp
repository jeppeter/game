
#include "remoteprocess.h"
#include <assert.h>
#include <Windows.h>
#include "..\\common\\output_debug.h"

#define LAST_ERROR_CODE()  (GetLastError() ? GetLastError() : 1)

typedef unsigned long ptr_t;

extern "C" int ProcRead(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen);

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



extern "C" int ProcWrite(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen,int force);
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



