// memshare.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "../../common/output_debug.h"
#include "../../common/uniansi.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int i;
	for (i=0;i<argc;i++)
	{
		fprintf(stderr,"[%d] %S\n",i,argv[i]);
	}
	return 0;
}

