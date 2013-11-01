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
    int wndnum=0,topwndnum=0;
    int i;

    if(argc < 2)
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

    ret = GetProcWindHandles(hProc,&pWnds,&wndsize);
    if(ret < 0)
    {
        ret = LAST_ERROR_CODE();
        fprintf(stderr,"GetProcWindHandles (%d)process error(%d)\n",pid,ret);
        goto out;
    }

    wndnum = ret;

    if(wndnum > 0)
    {
        ret = GetTopWinds(pWnds,wndnum,&pTopWnds,&topwndsize);
        if(ret < 0)
        {
            ret = LAST_ERROR_CODE();
            fprintf(stderr,"GetTopWinds (%d)process error(%d)\n",pid,ret);
            goto out;
        }
        topwndnum = ret;
    }

    fprintf(stdout,"Process(%d) windows %d",pid,wndnum);
    for(i=0; i<wndnum; i++)
    {
        if((i%5)==0)
        {
            fprintf(stdout,"\n0x%08x\t",i);
        }
        fprintf(stdout," 0x%08x",pWnds[i]);
    }
    fprintf(stdout,"\n");


    fprintf(stdout,"Process(%d) top window %d",pid,topwndnum);
    for(i=0; i<topwndnum; i++)
    {
        if((i%5)==0)
        {
            fprintf(stdout,"\n0x%08x\t",i);
        }
        fprintf(stdout," 0x%08x",pTopWnds[i]);
    }
    fprintf(stdout,"\n");


    ret = 0;

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

