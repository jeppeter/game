// segfault.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>


int main(int argc, char* argv[])
{
	unsigned int *pIntPtr=NULL;
	__try
	{
		*pIntPtr = 0;
	}

	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		fprintf(stderr,"error\n");
	}

	return 0;
}

