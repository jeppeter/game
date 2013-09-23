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
	WAVEFORMATEX *pCopied=NULL;

	if (pFormatEx)
	{
		formatsize = sizeof(*pFormatEx);
		formatsize += pFormatEx->cbSize;
		
		pCopied = (WAVEFORMATEX*)malloc(formatsize);
		if (pCopied==NULL)
		{
			ret = LAST_ERROR_CODE();
			goto out;
		}
		
		memcpy(pCopied,pFormatEx,formatsize);
	}
	EnterCriticalSection(&st_StateCS);
	if (st_pFormatEx)
	{
		free(st_pFormatEx);
	}
	st_pFormatEx = NULL;
	st_pFormatEx = pCopied;	
	LeaveCriticalSection(&st_StateCS);
out:	
	return ret > 0 ? -ret : 0;
}


static int GetFormat(int* pFormat,int* pChannels,int*pSampleRate,int* pBitsPerSample)
{
	int ret = 0;
	EnterCriticalSection(&st_StateCS);

	if (st_pFormatEx)
	{
		*pFormat = st_pFormatEx->wFormatTag;
		*pChannels = st_pFormatEx->nChannels;
		*pSampleRate = st_pFormatEx->nSamplesPerSec;
		*pBitsPerSample = st_pFormatEx->wBitsPerSample;
		ret = 1;
	}
	LeaveCriticalSection(&st_StateCS);
	return ret;
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

