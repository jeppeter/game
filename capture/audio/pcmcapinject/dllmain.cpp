// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "pcmcapinject.h"
#include <output_debug.h>



BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                     )
{
    int ret=0;
    switch(ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		DEBUG_INFO("\n");
        ret = PcmCapInjectInit();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        PcmCapInjectFini();
        break;
    }
    if(ret >= 0)
    {
        return TRUE;
    }
    return FALSE;
}

