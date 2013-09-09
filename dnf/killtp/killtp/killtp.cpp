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
    int enabled =0;
    int lastenable = 0;
    int res;
    const char* exename = NULL;
    unsigned int mem[2];
    int times = 0;

    if(argc < 2)
    {
        fprintf(stderr,"%s exename\n",argv[0]);
        return -3;
    }

    if(argc == 2)
    {
        exename = (const char*)argv[1];
        count = ProcEnum(exename,&pPids,&size);
        if(count < 0)
        {
            ret = count;
            fprintf(stderr,"could not get %s\n",exename);
            goto fail;
        }

        fprintf(stdout,"exe (%s) (count %d)\t",exename,count);
        for(i=0; i<count; i++)
        {
            fprintf(stdout," %d",pPids[i]);
        }
        fprintf(stdout,"\n");
        ret = 0;
        goto fail;
    }
    else
    {
        exename = (const char*)argv[2];
    }



    while(1)
    {
        count = ProcEnum(exename,&pPids,&size);
        if(count  == 2)
        {
            ret = count;
            fprintf(stderr,"get %s exe ok\n",exename);
            break;
        }
        Sleep(500);
        times ++;
        if((times% 10)==0)
        {
            fprintf(stdout,"could not get %s (count %d)\t",exename,count);
            for(i=0; i<count; i++)
            {
                fprintf(stdout," %d",pPids[i]);
            }
            fprintf(stdout,"\n");
        }
    }

    /*to sleep for a while and get the memory size*/
    Sleep(1000);
    count = ProcEnum(exename,&pPids,&size);
    if(count != 2)
    {
        ret = GetLastError() ? GetLastError() : 1;
        ret = -ret;
        fprintf(stderr,"get (%s) error (%d)\n",exename,ret);
        goto fail;
    }

    lastenable = EnableDebugLevel(1);
    if(lastenable < 0)
    {
        ret = lastenable;
        fprintf(stderr,"can not enable debug level error(%d)\n",ret);
        goto fail;
    }
    enabled = 1;


    for(i=0; i<count; i++)
    {
        ret = ProcMemorySize(pPids[i],&mem[i]);
        if(ret < 0)
        {
            fprintf(stderr,"could not get (%d) memory error %d\n",pPids[i],ret);
            goto fail;
        }
    }

    fprintf(stdout,"mem[0] %d(%d) mem[1] %d(%d)\n",mem[0],pPids[0],mem[1],pPids[1]);

    if(mem[0] > mem[1])
    {
        fprintf(stdout,"Kill %d\n",pPids[1]);
        ret = ProcKill(pPids[1],1);
    }
    else
    {
        fprintf(stdout,"Kill %d\n",pPids[1]);
        ret = ProcKill(pPids[0],1);
    }

    if(ret < 0)
    {
        fprintf(stderr,"could not kill %s error(%d)\n",exename,ret);
        goto fail;
    }
    fprintf(stdout,"Kill succ\n");

    if(enabled)
    {
        res = EnableDebugLevel(lastenable);
        if(res < 0)
        {
            ret = res;
            goto fail;
        }
    }
    enabled = 0;

    if(pPids)
    {
        free(pPids);
    }
    pPids = NULL;

    return 0;

fail:
    if(enabled)
    {
        res = EnableDebugLevel(lastenable);
        if(res < 0)
        {
            fprintf(stderr,"could not disable debug error(%d)\n",res);
        }
    }
    enabled = 0;
    if(pPids)
    {
        free(pPids);
    }
    pPids = NULL;
    size = 0;
    return ret;
}

