
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
    fprintf(fp,"\t-p processid       | to set for process id\n");
    fprintf(fp,"\t-r readaddr        | to read address\n");
    fprintf(fp,"\t-w writeaddr       | set write address\n");
    fprintf(fp,"\t-s size            | read or write size\n");
    fprintf(fp,"\t-c content         | write content it is in the value\n");
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
	if (pRetArgvLen == NULL)
	{
		ret = LAST_ERROR_CODE();
		goto fail;
	}

	pRetArgv = calloc(sizeof(*pRetArgv),argc);
	if (pRetArgv == NULL)
	{
		ret = LAST_ERROR_CODE();
		goto fail;
	}

	if (pRetArgvLen)
	{
		free(pRetArgvLen);
	}
	pRetArgvLen = NULL;
	return pRetArgv;
fail:
	if (pRetArgv)
	{
		for(i=0;i<retargc;i++)
		{
			assert(pRetArgvLen);
			if (pRetArgv[i])
			{
				free(pRetArgv[i]);
			}
			pRetArgv[i] = NULL;			
		}

		free(pRetArgv);		
	}

	if (pRetArgvLen)
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
}

int main(int argc,TCHAR argv[])
{
    return 0;
}
