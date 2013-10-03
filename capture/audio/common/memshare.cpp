
#include <memshare.h>
#include <uniansi.h>

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))


int ReadShareMem(unsigned char* pBasePtr,int offset,unsigned char* pBuffer,int bufsize)
{
    int ret=bufsize;
    unsigned char* pSrcPtr=(pBasePtr+offset);

    __try
    {
        memcpy(pBuffer,pSrcPtr,bufsize);
    }

    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        ret = -ERROR_INVALID_ADDRESS;
    }

    return ret;
}

int WriteShareMem(unsigned char* pBasePtr,int offset,unsigned char* pBuffer,int bufsize)
{
    int ret=bufsize;
    unsigned char* pDstPtr=(pBasePtr+offset);

    __try
    {
        memcpy(pDstPtr,pBuffer,bufsize);
    }

    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        ret = -ERROR_INVALID_ADDRESS;
    }

    return ret;
}

HANDLE CreateMapFile(const char* pMapFileName,int size,int create)
{
    HANDLE hMapFile=NULL;
    int ret;
#ifdef _UNICODE
    wchar_t* pMapFileW=NULL;
    int mapfilesize=0;
    ret = AnsiToUnicode((char*)pMapFileName,&pMapFileW,&mapfilesize);
    if(ret < 0)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }
    if(create)
    {
        hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,size,pMapFileW);
    }
    else
    {
        hMapFile =  OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,pMapFileW);
    }
#else
    if(create)
    {
        hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,size,pMapFileName);
    }
    else
    {
        hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,pMapFileName);
    }
#endif

    if(hMapFile == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

#ifdef _UNICODE
    AnsiToUnicode(NULL,&pMapFileW,&mapfilesize);
#endif

    return hMapFile;
fail:
    if(hMapFile)
    {
        CloseHandle(hMapFile);
    }
    hMapFile = NULL;
#ifdef _UNICODE
    AnsiToUnicode(NULL,&pMapFileW,&mapfilesize);
#endif
    SetLastError(ret);
    return NULL;
}

unsigned char* MapFileBuffer(HANDLE hMapFile,int size)
{
    void* pMemBase=NULL;

    pMemBase = MapViewOfFile(hMapFile,FILE_MAP_ALL_ACCESS,0,0,size);
    return (unsigned char*)pMemBase;
}

void UnMapFileBuffer(unsigned char** ppBuffer)
{
    if(*ppBuffer)
    {
        UnmapViewOfFile(*ppBuffer);
    }
    *ppBuffer = NULL;
    return ;
}

void CloseMapFileHandle(HANDLE *pHandle)
{
    if(*pHandle)
    {
        CloseHandle(*pHandle);
    }
    *pHandle = NULL;
    return;
}


