// cproc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))

#define DEBUG_INFO(...) do{fprintf(stdout,"[%s:%d]\t",__FILE__,__LINE__);fprintf(stdout,__VA_ARGS__);}while(0)
#define ERROR_INFO(...) do{fprintf(stderr,"[%s:%d]\t",__FILE__,__LINE__);fprintf(stderr,__VA_ARGS__);}while(0)

char* FormatCommandLine(int argc,char* argv[],int fromidx)
{
    char* pCommand=NULL,*pCurPtr;
    int totallen=0,leftlen;
    int ret;
    int i;
    if(argc == fromidx)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    for(i=fromidx; i<argc; i++)
    {
        /*for space */
        totallen += 1;
        totallen += strlen(argv[i]);
    }
    /*for terminator*/
    totallen += 1;
    pCommand= (char*)calloc(totallen,1);

    if(pCommand == NULL)
    {
        return NULL;
    }

    pCurPtr = pCommand;
    leftlen = totallen;
    for(i=fromidx; i<argc; i++)
    {
        if(i!=fromidx)
        {
            ret = _snprintf_s(pCurPtr,leftlen,_TRUNCATE," ");
            pCurPtr += ret;
            leftlen -= ret;
        }
        ret = _snprintf_s(pCurPtr,leftlen,_TRUNCATE,"%s",argv[i]);
        pCurPtr += ret;
        leftlen -= ret;
    }

    return pCommand;
}

int main(int argc, char* argv[])
{
    BOOL bret;
    int ret;
    char* pCommandLine=NULL;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    memset(&si,0,sizeof(si));
    memset(&pi,0,sizeof(pi));
    si.cb = 0;
    pCommandLine = FormatCommandLine(argc,argv,1);
    if(pCommandLine == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("format arg error(%d)\n",ret);
        return -ret;
    }

    /*now create process*/
    bret = CreateProcessA(NULL,pCommandLine,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not run process (%s) error(%d)\n",pCommandLine,ret);
        free(pCommandLine);
        return -ret;
    }
    DEBUG_INFO("RunProcess %s\n",pCommandLine);

    while(1)
    {
        Sleep(1000);
    }

    free(pCommandLine);
    return 0;
}

