
#include <windows.h>


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

int main(int argc,TCHAR argv[])
{
    return 0;
}
