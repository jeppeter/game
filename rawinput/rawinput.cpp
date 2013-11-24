//#include "rawinput.h"
#include <Windows.h>

PVOID GetApiAdd(LPCSTR dllname, LPCSTR procname);
typedef
BOOL
(_stdcall * PRegisterRawInputDevices)(
    PCRAWINPUTDEVICE pRawInputDevices,
    UINT uiNumDevices,
    UINT cbSize
);


PVOID GetApiAdd(LPCSTR dllname, LPCSTR procname)
{
	HMODULE hDll = LoadLibraryA(dllname);
	if(NULL == hDll)
		return NULL;
	PVOID pProc = GetProcAddress(hDll, procname);
	FreeLibrary(hDll);
	return pProc;
}

BOOL RegisitKeyBord(HWND hwnd)
{
	if(NULL == hwnd)
		return false;

	PRegisterRawInputDevices RegisterRawInputDevices = (PRegisterRawInputDevices)GetApiAdd("User32.dll", "RegisterRawInputDevices");
	if(NULL == RegisterRawInputDevices)
		return false;
	
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x06;
	rid.dwFlags = RIDEV_INPUTSINK;
	rid.hwndTarget = hwnd;

	return RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));
}

