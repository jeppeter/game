// evcall.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <common/output_debug.h>
#include <common/uniansi.h>
#include <common/strtohex.h>
#include <assert.h>

#define LAST_ERROR_CODE()  ((int)(GetLastError() ? GetLastError() : 1))

#define SDEBUG(...)  do{fprintf(stdout,"%s:%d\t",__FILE__,__LINE__);fprintf(stdout,__VA_ARGS__);}while(0)


void Usage(int ec,const char* fmt,...)
{
    FILE* fp=stderr;
    va_list ap;
    if(ec == 0)
    {
        fp = stdout;
    }
    if(fmt)
    {
        va_start(ap,fmt);
        vfprintf(fp,fmt,ap);
        fprintf(fp,"\n");
    }

    fprintf(fp,"evcall [OPTIONS] notifyevent waitevent\n");
    fprintf(fp,"\t-h|--help                    | to display this help information\n");
    fprintf(fp,"\t-c|--create                  | to use create for event name\n");

    exit(ec);
}

static int st_CreateEvent =0;
static char* st_pNotifyEvent=NULL;
static char* st_pWaitEvent=NULL;

int ParseParam(int argc,char* argv[])
{
    int i;
    for(i=1; i<argc; i++)
    {
        if(strcmp(argv[i],"-h")==0 ||
                strcmp(argv[i],"--help")==0)
        {
            Usage(0,NULL);
        }
        else if(strcmp(argv[i],"-c") == 0 ||
                strcmp(argv[i],"--create") == 0)
        {
            st_CreateEvent = 1;
        }
        else
        {
            break;
        }
    }

    if(i > (argc - 2))
    {
        Usage(3,"must specify notifyevent and wait event\n");
    }

    st_pNotifyEvent = argv[i];
    st_pWaitEvent = argv[i+1];


    return 0;
}


HANDLE GetEvent(const char* pName,int create)
{
    HANDLE hEvent=NULL;
    int ret;
#ifdef _UNICODE
    wchar_t* pNameW=NULL;
    int namesize=0;
    ret = AnsiToUnicode((char*)pName,&pNameW,&namesize);
    if(ret < 0)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }
    if(create)
    {
        hEvent = CreateEvent(NULL,FALSE,FALSE,pNameW);
    }
    else
    {
        hEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,pNameW);
    }
#else
    if(create)
    {
        hEvent = CreateEvent(NULL,FALSE,FALSE,pName);
    }
    else
    {
        hEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,pName);
    }
#endif

    if(hEvent==NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

#ifdef _UNICODE
    AnsiToUnicode(NULL,&pNameW,&namesize);
#endif

    return hEvent;
fail:
#ifdef _UNICODE
    AnsiToUnicode(NULL,&pNameW,&namesize);
#endif
    if(hEvent)
    {
        CloseHandle(hEvent);
    }
    SetLastError(ret);
    return NULL;
}


int main(int argc, char* argv[])
{
    HANDLE hWaitEvt=NULL,hNotifyEvt=NULL;
    int ret;
	BOOL bret;
	DWORD wret;
    ParseParam(argc,argv);

    hWaitEvt = GetEvent(st_pWaitEvent,st_CreateEvent);
    if(hWaitEvt == NULL)
    {
        ret = - LAST_ERROR_CODE();
		SDEBUG("could not %s %s error(%d)\n",st_CreateEvent ? "Create" : "Open" ,
			st_pWaitEvent,ret);
        goto out;
    }
    hNotifyEvt = GetEvent(st_pNotifyEvent,st_CreateEvent);
    if(hNotifyEvt == NULL)
    {
        ret = - LAST_ERROR_CODE();
		SDEBUG("could not %s %s error(%d)\n",st_CreateEvent ? "Create" : "Open" ,
			st_pNotifyEvent,ret);
        goto out;
    }

	/*now first to  notify wait event*/
	bret = SetEvent(hNotifyEvt);
	if (!bret)
	{
		ret = -LAST_ERROR_CODE();
		SDEBUG("could not notify %s error(%d)\n",
			st_pNotifyEvent,ret);
		goto out;
	}

	wret = WaitForSingleObject(hWaitEvt,INFINITE);
	if (wret != WAIT_OBJECT_0)
	{
		ret = -LAST_ERROR_CODE();
		SDEBUG("could not wait for %s return %d error(%d)\n",st_pWaitEvent,wret,ret);
		goto out;
	}

	SDEBUG("Wait %s ok\n",st_pWaitEvent);
	ret = 0;

out:
    if(hWaitEvt)
    {
        CloseHandle(hWaitEvt);
    }
    hWaitEvt = NULL;
    if(hNotifyEvt)
    {
        CloseHandle(hNotifyEvt);
    }
    hNotifyEvt = NULL;
    return ret;
}

