// segfault.cpp : 定义控制台应用程序的入口点。
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

