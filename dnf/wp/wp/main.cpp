
#include <windows.h>
#include "..\\common\\uniansi.h"


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
    //fprintf(fp,"\t-f filecontent     | write content from file\n");
    //fprintf(fp,"\t-l loaddll         | to load dll\n");
    //fprintf(fp,"\t-f funcname        | to call function name\n");
    exit(ec);
}


static unsigned int st_ProcessId =0;
static unsigned long st_ReadAddr = 0;
static unsigned long st_WriteAddr = 0;
static unsigned int st_Size=0;
static unsigned char* pWriteBuffer=NULL;

#ifdef _UNICODE
char* ChangeParam(int argc,wchar* argvw[])
{
    int ret;
    char** pRetArgv=NULL;
    int retargc=argc,i;
    int *pRetArgvLen=NULL;

    pRetArgvLen = calloc(sizeof(*pRetArgvLen),argc);
    if(pRetArgvLen == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    pRetArgv = calloc(sizeof(*pRetArgv),argc);
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
	for (i=1;i<argc;i++)
	{
		if (strcmp(argv[i],"-h")==0 ||
			strcmp(argv[i],"--help")==0)
		{
			Usage(0,NULL);
		}
		else if (strcmp(argv[i],"-p")==0 ||
			strcmp(argv[i],"--process")==0)
		{
			if (argc <= (i+1))
			{
				Usage(3,"argv[%d] %s need an arg",i,argv[i]);
			}
			st_ProcessId = atoi(argv[i+1]);
			i += 1;
		}
		else if (strcmp(argv[i],"-r")==0 ||
			strcmp(argv[i],"--read")==0)
		{
			if (argc <= (i+1))
			{
				Usage(3,"argv[%d] %s need an arg",i,argv[i]);
			}
			st_ReadAddr = strtoul();
			
			
		}
		else if (strcmp(argv[i],"")==0 ||
			strcmp(argv[i],"")==0)
		{
		}
		else if (strcmp(argv[i],"")==0 ||
			strcmp(argv[i],"")==0)
		{
		}
		else if (strcmp(argv[i],"")==0 ||
			strcmp(argv[i],"")==0)
		{
		}
		else
		{
			Usage(3,"unknown params %s",argv[i]);
		}
	}
}

int main(int argc,TCHAR* argv[])
{
    int i;
	int ret;
#ifdef _UNICODE
    char** pRetArgv=NULL;

	pRetArgv = ChangeParam(argc,argv);
	if (pRetArgv == NULL)
	{
		ret = -1 * LAST_ERROR_CODE();
		goto out;
	}
	ParseParam(argc,pRetArgv);
#else
	ParseParam(argc,argv);
#endif

	

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
