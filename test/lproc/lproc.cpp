// lproc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <procex.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <TlHelp32.h>

#define ERROR_INFO(...) do{fprintf(stderr,"%s:%s:%d[%ld]\t",__FILE__,__FUNCTION__,__LINE__,time(NULL));fprintf(stderr,__VA_ARGS__);}while(0)
#define DEBUG_INFO(...) do{fprintf(stderr,"%s:%s:%d[%ld]\t",__FILE__,__FUNCTION__,__LINE__,time(NULL));fprintf(stderr,__VA_ARGS__);}while(0)


#if 1
int main(int argc, char* argv[])
{
    int i,j;
    unsigned int *pPids=NULL;
    int pidsize=0,pidnum;
    int ret;
    for(i=1; i<argc; i++)
    {
        ret= GetModuleInsertedProcess(argv[i],&pPids,&pidsize);
        if(ret < 0)
        {
            ERROR_INFO("get %s pids error(%d)\n",argv[i]);
        }
        else
        {
            pidnum = ret;
            DEBUG_INFO("Get %s num(%d)\n",argv[i],pidnum);
            for(j=0; j<pidnum; j++)
            {
                DEBUG_INFO("[%d] %d\n",j,pPids[j]);
            }
        }

    }

    ret = 0;
out:
    if(pPids)
    {
        free(pPids);
    }
    pPids = NULL;
    pidsize = 0;
    return -ret;
}

#else
int main(int argc,char*argv[])
{
    DWORD procID = 0;
    FILE *fp;

    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);


    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if(processesSnapshot == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    fp = _tfopen(_T("z:\\Process_List.txt") , _T("a+"));

    if(fp == NULL)
    {
        DEBUG_INFO("Can't open File");
        return -1;
    }

    Process32First(processesSnapshot, &processInfo);


    _ftprintf(fp, _T("\n Process: %s :: Process ID: %d"), processInfo.szExeFile, processInfo.th32ProcessID);
    procID =  processInfo.th32ProcessID;

    if(procID == 0)
    {
        while(Process32Next(processesSnapshot, &processInfo))
        {
            _ftprintf(fp, _T("\n Process: %s :: Process ID: %d"), processInfo.szExeFile, processInfo.th32ProcessID);
        }
    }

    fclose(fp);
    CloseHandle(processesSnapshot);
	return 0;
}
#endif
