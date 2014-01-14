// takedbg.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include "procex.h"


int main(int argc, char* argv[])
{

    int ret;

    ret = EnableCurrentDebugPriv();
    if(ret < 0)
    {
        fprintf(stderr,"Enable Error(%d)\n",ret);
        return -1;
    }

    ret = DisableCurrentDebugPriv();
    if(ret < 0)
    {
        fprintf(stderr,"Disable Error(%d)\n",ret);
        return -1;
    }

	fprintf(stdout,"Debug Priv Succ\n");
    return 0;
}

