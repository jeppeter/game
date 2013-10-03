
#include <evt.h>
#include <uniansi.h>

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))

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


