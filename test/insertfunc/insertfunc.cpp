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

int main(int argc,char * argv[])
{
	
    return 0;
}

