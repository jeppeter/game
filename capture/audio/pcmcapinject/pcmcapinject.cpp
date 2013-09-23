// pcmcapinject.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "pcmcapinject.h"
#include <Mmreg.h>
#include <stdlib.h>

#define LAST_ERROR_CODE() (GetLastError() ? GetLastError() : 1)

static CRITICAL_SECTION st_DetourCS;
static CRITICAL_SECTION st_StateCS;

static WAVEFORMATEX *st_pFormatEx;

static int SetFormat(WAVEFORMATEX* pFormatEx)
{
	int ret=0;
	int formatsize=0;
	EnterCriticalSection(&st_StateCS);

	if (st_pFormatEx)
	{
		free(st_pFormatEx);
	}
	st_pFormatEx = NULL;

	/*now to test for the format size*/
	formatsize = sizeof(*pFormatEx);
	formatsize += pFormatEx->cbSize;

	st_pFormatEx = (WAVEFORMATEX*)malloc(formatsize);
	if (st_pFormatEx==NULL)
	{
		ret = LAST_ERROR_CODE();
		goto release;
	}

	memcpy(st_pFormatEx,pFormatEx,formatsize);
	
release:
	LeaveCriticalSection(&st_StateCS);
	return ret > 0 ? -ret : 0;
}





/*now we should handle the thread*/
int PcmCapInject_SetAudioOperation(int iOperation)
{
	return 0;
}

int PcmCapInject_SetAudioState(int iState)
{
	return 0;
}




void PcmCapInjectFini(void)
{
	return;
}

int PcmCapInjectInit(void)
{
	return 0;
}

