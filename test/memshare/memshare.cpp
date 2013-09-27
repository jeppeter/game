// memshare.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <common/output_debug.h>
#include <common/uniansi.h>
#include <common/strtohex.h>
#include <assert.h>

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
	fprintf(fp,"\t-C|--create                  | to use create for mem share\n");
    fprintf(fp,"\t-n|--name sharename          | to set for memory shared name\n");
    fprintf(fp,"\t-m|--memsize share size      | to set share memory size\n");
    fprintf(fp,"\t-r|--read read offset        | to read offset\n");
    fprintf(fp,"\t-w|--write write offset      | set write offset\n");
    fprintf(fp,"\t-s|--size size               | read or write size\n");
    fprintf(fp,"\t-c|--content content         | write content it is in the value\n");
    fprintf(fp,"\t-f|--from file               | write content from file\n");
    fprintf(fp,"\t-t|--to  file                | read content dump to file\n");
	fprintf(fp,"\t-T|--time sec                | set timeout for write\n");
	
    exit(ec);
}



static unsigned long st_ReadOffset = 0;
static int st_ReadInit=0;
static unsigned long st_WriteOffset = 0;
static int st_WriteInit=0;
static unsigned long st_MemSize =0 ;
static char* st_pShareName=NULL;
static unsigned char* st_pBuffer=NULL;
static unsigned long st_BufSize=0;
static char* st_pFromFile=NULL;
static char* st_pToFile=NULL;
static int st_Timeout=0;
static int st_CreateMem=0;

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
        else if(strcmp(argv[i],"-r")==0 ||
                strcmp(argv[i],"--read")==0)
        {
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }
            st_ReadOffset = StrToHex(argv[i+1]);
			st_ReadInit = 1;
            i += 1;
        }
        else if(strcmp(argv[i],"-w")==0 ||
                strcmp(argv[i],"--write")==0)
        {
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }
            st_WriteOffset = StrToHex(argv[i+1]);
			st_WriteInit = 1;
            i += 1;
        }
        else if(strcmp(argv[i],"-m")==0 ||
                strcmp(argv[i],"--memsize")==0)
        {
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }
            st_MemSize = StrToHex(argv[i+1]);
			if (st_MemSize == 0)
			{
				Usage(3,"memsize can not be 0");
			}
            i += 1;
        }
        else if(strcmp(argv[i],"-s")==0 ||
                strcmp(argv[i],"--size")==0)
        {
            unsigned int oldsize=st_BufSize;
            unsigned char* pOldBuffer=st_pBuffer;
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }
            st_BufSize= StrToHex(argv[i+1]);
            if(st_BufSize == 0)
            {
                Usage(3,"size must > 0");
            }

            st_pBuffer = (unsigned char*)malloc(st_BufSize);
            if(st_pBuffer == NULL)
            {
                ERROR_INFO("could not malloc size %d(0x%08x)\n",st_BufSize,st_BufSize);
                exit(3);
            }

            if(pOldBuffer)
            {
                memcpy(st_pBuffer,pOldBuffer,st_BufSize > oldsize ? oldsize: st_BufSize);
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

            if(st_pBuffer == NULL)
            {
                if(st_BufSize < 4)
                {
                    st_BufSize = 4;
                }
                st_pBuffer =(unsigned char*) malloc(st_BufSize);
                if(st_pBuffer == NULL)
                {
                    Usage(3,"can not allocate size %d",st_BufSize);
                }
            }
            else if(st_BufSize < 4)
            {
                st_BufSize = 4;
                if(st_pBuffer)
                {
                    free(st_pBuffer);
                }
                st_pBuffer=NULL;
                st_pBuffer = (unsigned char*)malloc(st_BufSize);
                if(st_pBuffer == NULL)
                {
                    ERROR_INFO("could not allocate size %d\n",st_BufSize);
                    exit(3);
                }
            }

            c = StrToHex(argv[i+1]);
            memcpy(st_pBuffer,&c,st_BufSize > 4 ? 4 : st_BufSize);
            i += 1;
        }
        else if(strcmp(argv[i],"-f") == 0 ||
                strcmp(argv[i],"--from") == 0)
        {
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }
			st_pFromFile = argv[i+1];
			i +=  1;
        }
        else if(strcmp(argv[i],"-t") == 0 ||
                strcmp(argv[i],"--to") == 0)
        {
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }
			st_pToFile = argv[i+1];
			i +=  1;
        }
        else if(strcmp(argv[i],"-T") == 0 ||
                strcmp(argv[i],"--timeout") == 0)
        {
            if(argc <= (i+1))
            {
                Usage(3,"argv[%d] %s need an arg",i,argv[i]);
            }
			st_Timeout = StrToHex(argv[i+1]);
			i +=  1;
        }
        else
        {
            Usage(3,"unknown params %s",argv[i]);
        }
    }

	if (st_pShareName == NULL)
	{
		Usage(3,"must specify share name");
	}

	if (st_MemSize == 0)
	{
		Usage(3,"must specify memsize");
	}

	if (st_BufSize == 0)
	{
		st_BufSize = 4;
		assert (st_pBuffer == NULL);
		{
			st_pBuffer = (unsigned char*)malloc(st_BufSize);
			if (st_pBuffer == NULL)
			{
				ERROR_INFO("could not get the %d size buffer\n",st_BufSize);
				exit (3);
			}
		}
	}

	if (st_ReadInit == 0 && st_WriteInit == 0)
	{
		Usage(3,"must specify read/write use (-r/-w)");
	}


    return 0;
}



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


int main(int argc, char* argv[])
{
    int i;
	
	ParseParam(argc,argv);

	
    return 0;
}

