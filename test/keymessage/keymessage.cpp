// keymessage.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "keymessage.h"
#include "output_debug.h"
#include <memory>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
UCHAR g_LastKeyboardState[256] = {0};

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                       _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine,
                       _In_ int       nCmdShow)
{
    BOOL bret;
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO:  在此放置代码。
    MSG msg;
    HACCEL hAccelTable;

    // 初始化全局字符串
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_KEYMESSAGE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if(!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KEYMESSAGE));

    // 主消息循环:
    while(1)
    {
        bret =PeekMessage(&msg, NULL, 0, 0,PM_REMOVE);
        if(bret)
        {
            if(msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
            {
                int mapchar;
                mapchar = MapVirtualKey(msg.wParam,MAPVK_VK_TO_CHAR);
                //DEBUG_INFO("message (0x%08x:%d) wParam(0x%08x:%d) lParam(0x%08x:%d) mapchar(0x%08x:%d)\n",
                //           msg.message,msg.message,msg.wParam,msg.wParam,msg.lParam,msg.lParam,mapchar,mapchar);
            }

            if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if(msg.message == WM_QUIT)
            {
                break;
            }
        }
    }

    return (int) msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KEYMESSAGE));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_KEYMESSAGE);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance; // 将实例句柄存储在全局变量中

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if(!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;
    RAWINPUTDEVICE rid;
    UINT dwSize;
    std::auto_ptr<BYTE> lpByte2(new BYTE[20]),lpKeyboardState2(new BYTE[256]);
    LPBYTE lpByte,lpKeyboardState=lpKeyboardState2.get();
    PRAWINPUT raw;
    SHORT shiftstate,ctrlstate,menustate,lwinstate,rwinstate;
    SHORT ashiftstate,actrlstate,amenustate;
    ULONG tickcount;
    BOOL bret;
    UINT i;

    switch(message)
    {
    case WM_CREATE:
        // register interest in raw data
        rid.dwFlags=RIDEV_NOLEGACY|RIDEV_INPUTSINK;	// ignore legacy messages and receive system wide keystrokes
        rid.usUsagePage=1;							// raw keyboard data only
        rid.usUsage=6;
        rid.hwndTarget=hWnd;
        RegisterRawInputDevices(&rid,1,sizeof(rid));
        break;
    case WM_INPUT:
        shiftstate = ::GetKeyState(VK_SHIFT);
        ctrlstate = ::GetKeyState(VK_CONTROL);
        menustate = ::GetKeyState(VK_MENU);
        lwinstate = ::GetKeyState(VK_LWIN);
        rwinstate = ::GetKeyState(VK_RWIN);
        ashiftstate = ::GetAsyncKeyState(VK_SHIFT);
        actrlstate = ::GetAsyncKeyState(VK_CONTROL);
        amenustate = ::GetAsyncKeyState(VK_MENU);

		ZeroMemory(lpKeyboardState,256);
        bret = GetKeyboardState(lpKeyboardState);
        if(bret)
        {
            for(i=0; i<256; i++)
            {
                if(lpKeyboardState[i] != g_LastKeyboardState[i])
                {
                    DEBUG_INFO("Before[%d] state(0x%02x) != laststate(0x%02x)\n",i,lpKeyboardState[i],g_LastKeyboardState[i]);
                }
            }

			DEBUG_INFO("g_LastKeyboardState 0x%p lpKeyboardState 0x%p",g_LastKeyboardState,lpKeyboardState);
			CopyMemory(g_LastKeyboardState,lpKeyboardState,sizeof(g_LastKeyboardState));
			DEBUG_INFO("g_LastKeyboardState 0x%p lpKeyboardState 0x%p",g_LastKeyboardState,lpKeyboardState);
        }


        //DEBUG_INFO("BeforeGet shiftstate 0x%04x ctrlstate 0x%04x menustate 0x%04x lwinstate 0x%04x rwinstate 0x%04x\n",
        //           shiftstate,ctrlstate,menustate,lwinstate,rwinstate);
        //DEBUG_INFO("BeforeGet ashiftstate 0x%04x actrlstate 0x%04x amenustate 0x%04x\n",
        //           ashiftstate,actrlstate,amenustate);
        if(GetRawInputData((HRAWINPUT)lParam,RID_INPUT,NULL,&dwSize,sizeof(RAWINPUTHEADER))==-1)
        {
            break;
        }

        lpByte2.reset(new BYTE[dwSize]);
        lpByte = lpByte2.get();
        if(GetRawInputData((HRAWINPUT)lParam,RID_INPUT,lpByte,&dwSize,sizeof(RAWINPUTHEADER))!=dwSize)
        {
            break;
        }
        raw=(PRAWINPUT)lpByte;
        tickcount = GetTickCount();
        DEBUG_INFO("(%ld) Kbd: make=%04x Flags:%04x Reserved:%04x ExtraInformation:%08x, msg=%04x VK=%04x \n",
                   tickcount,
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


        //DEBUG_INFO("AfterGet shiftstate 0x%04x ctrlstate 0x%04x menustate 0x%04x lwinstate 0x%04x rwinstate 0x%04x\n",
        //           shiftstate,ctrlstate,menustate,lwinstate,rwinstate);
        //DEBUG_INFO("AfterGet ashiftstate 0x%04x actrlstate 0x%04x amenustate 0x%04x\n",
        //           ashiftstate,actrlstate,amenustate);
        bret = GetKeyboardState(lpKeyboardState);
        if(bret)
        {
            for(i=0; i<256; i++)
            {
                if(lpKeyboardState[i] != g_LastKeyboardState[i])
                {
                    DEBUG_INFO("After[%d] state(0x%02x) != laststate(0x%02x)\n",i,lpKeyboardState[i],g_LastKeyboardState[i]);
                }
            }

			CopyMemory(g_LastKeyboardState,lpKeyboardState,sizeof(g_LastKeyboardState));
        }
        break;
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // 分析菜单选择:
        switch(wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO:  在此添加任意绘图代码...
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch(message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
