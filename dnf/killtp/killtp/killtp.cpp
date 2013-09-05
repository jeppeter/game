// killtp.cpp : 定义控制台应用程序的入口点。
//

#include <Windows.h>
#include <common/remoteproc.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

int main(int argc, TCHAR* argv[])
{
    unsigned int *pPids=NULL;
    int size = 0;
    int count;
    int i;
    int ret;
    unsigned int memsize;

    if(argc < 2)
    {
        fprintf(stderr,"%s exename\n",argv[0]);
        return -3;
    }

    count = ProcEnum((const char*)argv[1],&pPids,&size);
    if(count < 0)
    {
        ret = count;
        fprintf(stderr,"could not get %s exe error\n",argv[1]);
        goto fail;
    }

    fprintf(stdout,"find (%s):\n",argv[1]);
    if(count > 0)
    {
        for(i=0; i<count; i++)
        {
            ret = ProcMemorySize(pPids[i],&memsize);
            if(ret < 0)
            {
                fprintf(stderr,"could not get (%d) error %d\n",pPids[i],ret);
                goto fail;
            }
			fprintf(stdout,"[%d] %d size %d\n",i,pPids[i],memsize);
        }
    }

    if(pPids)
    {
        free(pPids);
    }
    pPids = NULL;

    return 0;

fail:
    if(pPids)
    {
        free(pPids);
    }
    pPids = NULL;
    size = 0;
    return ret;
}

