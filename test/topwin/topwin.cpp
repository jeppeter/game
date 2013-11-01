// topwin.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "winproc.h"

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))


int main(int argc, char * argv[])
{
    int ret;
    int pid=0;
    HANDLE hProc=NULL;
    HANDLE *pWnds=NULL,*pTopWnds=NULL;
    int wndsize=0,topwndsize=0;

    if(argc < 0)
    {
        fprintf(stderr,"%s pid\n",argv[0]);
        ret = ERROR_INVALID_PARAMETER;
        goto out;
    }

    pid = atoi(argv[1]);
    if(pid == 0)
    {
        fprintf(stderr,"%s pid (number)\n",argv[0]);
        ret = ERROR_INVALID_PARAMETER;
        goto out;
    }

    hProc = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
    if(hProc == NULL)
    {
        ret = LAST_ERROR_CODE();
        fprintf(stderr,"could not open process %d error(%d)\n",pid,ret);
        goto out;
    }

	




out:
    GetTopWinds(NULL,0,&pTopWnds,&topwndsize);
    GetProcWindHandles(NULL,&pWnds,&wndsize);

    if(hProc)
    {
        CloseHandle(hProc);
    }
    hProc = NULL;
    SetLastError(ret);
    return -ret;
}

