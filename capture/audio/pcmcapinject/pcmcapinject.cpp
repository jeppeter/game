// pcmcapinject.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "pcmcapinject.h"


static CRITICAL_SECTION st_DetourCS;

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

