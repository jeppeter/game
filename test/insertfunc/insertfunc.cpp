// insertfunc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))

HANDLE WINAPI CreateProcessSuspend(const char* lpAppName,const char* lpCommandLine)
{
    HANDLE hProc;
    BOOL bret;
    STARTINFO si;
    PROCESS_INFORMATION pi= {0};
    int ret;

    ZeroMemory(&si,sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;

    bret = CreateProcessA(lpAppName,lpCommandLine,NULL,NULL,FALSE,CREATE_SUSPEND,NULL,NULL,&si,&pi);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }



    return pi.hProcess;
fail:
    if(pi.hProcess)
    {
        CloseHandle(pi.hProcess);
    }
    ZeroMemory(&pi,sizeof(pi));
    SetLastError(ret);
    return NULL;
}

static char* st_pExec = NULL;
static char* st_pCommand= NULL;

char* FormatCommandLine(int argc,char* argv[])
{
    int i;
    char* pCommandLine=NULL;
    int totallen=0,leftlen;
    char* pCurLine=NULL;
    int ret;

    for(i=0; i<argc; i++)
    {
        /*for quoting*/
        totallen += 1;
        totallen += strlen(argv[i]);
        /*for quoting*/
        totallen += 1;
        /*for space*/
        totallen += 1;
    }

    if(totallen == 0)
    {
        SetLastError(0);
        return NULL;
    }

    /*for null terminator*/
    totallen += 1;

    pCommandLine = malloc(totallen);
    if(pCommandLine == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    pCurLine = pCommandLine;
    leftlen = totallen;

    /*now we should copy the buffer*/
    for(i=0; i<argc; i++)
    {
        ret = _snprintf_s(pCurLine,leftlen,leftlen,"\"%s\"",argv[i]);
        pCurLine += ret;
        leftlen -= ret;

        if(i < (argc-1))
        {
            ret = _snprintf_s(pCurLine,leftlen,leftlen," ");
            pCurLine += ret;
            leftlen -= ret;
        }
    }


    return pCommandLine;
fail:
    if(pCommandLine)
    {
        free(pCommandLine);
    }
    pCommandLine = NULL;
    SetLastError(ret);
    return NULL;
}

void ParseParam(int argc,char* argv[])
{
    int ret;
    int i;

    for(i=1; i<argc; i++)
    {
    }
}

int main(int argc,char * argv[])
{
    int ret;
    char* pCommandLine=NULL;
    HANDLE hProc=NULL;
    if(argc < 2)
    {
        fprintf(stderr,"%s cmd ...\n",argv[0]);
        exit(3);
    }

    pCommandLine = FormatCommandLine(argc-1,&(argv[1]));
    if(pCommandLine == NULL)
    {
        ERROR_INFO("can not make command line\n");
        return -3;
    }

    hProc = CreateProcessSuspend(st_pExec,st_pCommand);
    if(hProc == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto out;
    }

    /*now we should insert dll*/


out:
    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;
    if(pCommandLine)
    {
        free(pCommandLine);
    }
    pCommandLine = NULL;
    return -ret;
}

