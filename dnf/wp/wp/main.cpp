
#include <windows.h>
#include <common/uniansi.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <common/strtohex.h>
#include <common/output_debug.h>
#include <common/remoteproc.h>
#define LAST_ERROR_CODE()  (GetLastError() ? GetLastError() : 1)


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

    fprintf(fp,"wp [OPTIONS]\n");
    fprintf(fp,"\t-h|--help                    | to display this help information\n");
    fprintf(fp,"\t-p|--process processid       | to set for process id\n");
    fprintf(fp,"\t-r|--read readaddr           | to read address\n");
    fprintf(fp,"\t-w|--write writeaddr         | set write address\n");
    fprintf(fp,"\t-s|--size size               | read or write size\n");
    fprintf(fp,"\t-c|--content content         | write content it is in the value\n");
    fprintf(fp,"\t-F|--force                   | to specify the force as for write\n");
    fprintf(fp,"\t-k|--kill                    | to kill the process\n");
    //fprintf(fp,"\t-f filecontent     | write content from file\n");
    //fprintf(fp,"\t-l loaddll         | to load dll\n");
    //fprintf(fp,"\t-f funcname        | to call function name\n");
    exit(ec);
}


static unsigned int st_ProcessId =0;
static unsigned long st_ReadAddr = 0;
static unsigned long st_WriteAddr = 0;
static unsigned int st_Size=0;
static unsigned char* st_pWriteBuffer=NULL;
static int st_Force=0;
static int st_KillProc=0;

#ifdef _UNICODE
char** ChangeParam(int argc,wchar_t* argvw[])
{
    int ret;
    char** pRetArgv=NULL;
    int retargc=argc,i;
    int *pRetArgvLen=NULL;

    pRetArgvLen =(int*) calloc(sizeof(*pRetArgvLen),argc);
    if(pRetArgvLen == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    pRetArgv =(char**) calloc(sizeof(*pRetArgv),argc);
    if(pRetArgv == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    for(i=0; i<argc; i++)
    {
        ret = UnicodeToAnsi(argvw[i],&(pRetArgv[i]),&(pRetArgvLen[i]));
        if(ret < 0)
        {
            ret = LAST_ERROR_CODE();
            goto fail;
        }
    }

    if(pRetArgvLen)
    {
        free(pRetArgvLen);
    }
    pRetArgvLen = NULL;
    return pRetArgv;
fail:
    if(pRetArgv)
    {
        for(i=0; i<retargc; i++)
        {
            assert(pRetArgvLen);
            if(pRetArgv[i])
            {
                free(pRetArgv[i]);
            }
            pRetArgv[i] = NULL;
        }

        free(pRetArgv);
    }

    if(pRetArgvLen)
    {
        free(pRetArgvLen);
    }
    pRetArgvLen = NULL;
    SetLastError(ret);
    return NULL;
}
#endif

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
        else if(strcmp(argv[i],"-p")==0 ||
                strcmp(argv[i],"--process")==0)
        {
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }
            st_ProcessId = StrToHex(argv[i+1]);
            i += 1;
        }
        else if(strcmp(argv[i],"-r")==0 ||
                strcmp(argv[i],"--read")==0)
        {
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }
            st_ReadAddr = StrToHex(argv[i+1]);
            i += 1;
        }
        else if(strcmp(argv[i],"-w")==0 ||
                strcmp(argv[i],"--write")==0)
        {
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }
            st_WriteAddr = StrToHex(argv[i+1]);
            i += 1;
        }
        else if(strcmp(argv[i],"-s")==0 ||
                strcmp(argv[i],"--size")==0)
        {
            unsigned int oldsize=st_Size;
            unsigned char* pOldBuffer=st_pWriteBuffer;
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }
            st_Size= StrToHex(argv[i+1]);
            if(st_Size == 0)
            {
                Usage(3,"size must > 0");
            }

            st_pWriteBuffer = (unsigned char*)malloc(st_Size);
            if(st_pWriteBuffer == NULL)
            {
                ERROR_INFO("could not malloc size %d(0x%08x)\n",st_Size,st_Size);
                exit(3);
            }

            if(pOldBuffer)
            {
                memcpy(st_pWriteBuffer,pOldBuffer,st_Size > oldsize ? oldsize: st_Size);
                free(pOldBuffer);
                pOldBuffer = NULL;
                oldsize = 0;
            }

            i += 1;
        }
        else if(strcmp(argv[i],"-c")==0 ||
                strcmp(argv[i],"--content")==0)
        {
            unsigned int c;
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }

            if(st_pWriteBuffer == NULL)
            {
                if(st_Size == 0)
                {
                    st_Size = 4;
                }
                st_pWriteBuffer =(unsigned char*) malloc(st_Size);
                if(st_pWriteBuffer == NULL)
                {
                    Usage(3,"can not allocate size %d",st_Size);
                }
            }

            c = StrToHex(argv[i+1]);
            memcpy(st_pWriteBuffer,&c,st_Size > 4 ? 4 : st_Size);
            i += 1;
        }
        else if(strcmp(argv[i],"-F")==0 ||
                strcmp(argv[i],"--force")==0)
        {
            st_Force = 1;
        }
        else if(strcmp(argv[i],"-k")== 0 ||
                strcmp(argv[i],"--kill")==0)
        {
            st_KillProc = 1;
        }
        else
        {
            Usage(3,"unknown params %s",argv[i]);
        }
    }

    if(st_ProcessId == 0)
    {
        Usage(3,"must specify the processid");
    }
    if(st_KillProc == 0)
    {
        if(st_ReadAddr == 0 && st_WriteAddr == 0)
        {
            Usage(3,"must specify read/write address");
        }

        if(st_WriteAddr && st_pWriteBuffer == NULL)
        {
            Usage(3,"specify write address but not the content");
        }

        if(st_ReadAddr && st_pWriteBuffer == NULL)
        {
            if(st_Size == 0)
            {
                st_Size = 4;
            }
            st_pWriteBuffer = (unsigned char*)malloc(st_Size);
            if(st_pWriteBuffer == NULL)
            {
                Usage(3,"allocate read buffer error");
            }
        }
    }

    return 0;
}



int main(int argc,TCHAR* argv[])
{
    int i;
    int ret;
#ifdef _UNICODE
    char** pRetArgv=NULL;
    pRetArgv = ChangeParam(argc,argv);
    if(pRetArgv == NULL)
    {
        ret = -1 * LAST_ERROR_CODE();
        goto out;
    }
    //ParseParam(argc,pRetArgv);
	ParseParam(argc,(char**)argv);
#else
    ParseParam(argc,argv);
#endif

    if(st_KillProc==0)
    {

        if(st_WriteAddr)
        {
            ret = ProcWrite(st_ProcessId,(void*)st_WriteAddr,st_pWriteBuffer,st_Size,st_Force);
            if(ret < 0)
            {
                goto out;
            }
        }
        else
        {
            ret = ProcRead(st_ProcessId,(void*)st_ReadAddr,st_pWriteBuffer,st_Size);
            if(ret < 0)
            {
                goto out;
            }

            fprintf(stdout,"Read At 0x%08x(%d):",st_ReadAddr,st_Size);
            for(i=0; i<(int)st_Size; i++)
            {
                if((i%16)==0)
                {
                    fprintf(stdout,"\n0x%08x\t",i);
                }
                fprintf(stdout," 0x%02x",st_pWriteBuffer[i]);
            }
            fprintf(stdout,"\n");
        }
    }
    else
    {
        ret = ProcKill(st_ProcessId,1);
        if(ret < 0)
        {
            goto out;
        }
    }

    ret = 0;

out:
#ifdef _UNICODE
    if(pRetArgv)
    {
        for(i=0; i<argc; i++)
        {
            if(pRetArgv[i])
            {
                free(pRetArgv[i]);
            }
            pRetArgv[i] = NULL;
        }
        free(pRetArgv);
    }
    pRetArgv = NULL;
#endif
    return ret;
}
