// memshare.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../../common/output_debug.h"
#include "../../common/uniansi.h"

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
    fprintf(fp,"\t-n|--name sharename          | to set for memory shared name\n");
	fprintf(fp,"\t-m|--memsize share size      | to set share memory size\n");
    fprintf(fp,"\t-r|--read read offset        | to read offset\n");
    fprintf(fp,"\t-w|--write write offset      | set write offset\n");
    fprintf(fp,"\t-s|--size size               | read or write size\n");
    fprintf(fp,"\t-c|--content content         | write content it is in the value\n");
    fprintf(fp,"\t-f|--from file               | write content from file\n");
	fprintf(fp,"\t-t|--to  file                | read content dump to file\n");
    exit(ec);
}



int _tmain(int argc, _TCHAR* argv[])
{
	int i;
	for (i=0;i<argc;i++)
	{
		fprintf(stderr,"[%d] %S\n",i,argv[i]);
	}
	return 0;
}

