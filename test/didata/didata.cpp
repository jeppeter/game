// didata.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "didata.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
BOOL InitDirectInput(HINSTANCE hInstance,HWND hwnd);
int Read_Device(IDIRECTINPUTDEVICE8 *pDev,LPDIDEVICEOBJECTDATA pData,int datanum);
void FiniDirectInput(void);


// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL                                  InitDirectInput(HINSTANCE hInstance,HWND hwnd);
void                                    FiniDirectInput(void);
IDIRECTINPUTDEVICE8   *st_pMouseDev=NULL;
IDIRECTINPUTDEVICE8   *st_pKeyboardDev=NULL;
IDIRECTINPUT8  *st_pInput=NULL;


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                       _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine,
                       _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    BOOL bret;
    DIDEVICEOBJECTDATA data[10];

    // TODO:  在此放置代码。
    MSG msg;
    HACCEL hAccelTable;
    int ret;

    // 初始化全局字符串
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_DIDATA, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if(!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DIDATA));

    // 主消息循环:
    ZeroMemory(&msg,sizeof(msg));
    while(msg.message != WM_QUIT)
    {

        bret = PeekMessage(&msg,NULL,0,0,PM_REMOVE);
        if(bret)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            ret = Read_Device(st_pKeyboardDev,data,10);
            if(ret < 0)
            {
                break;
            }
            else if(ret > 0)
            {
                DEBUG_BUFFER_FMT(data,ret,"Keyboard Data:");
            }

            ret = Read_Device(st_pMouseDev,data,10);
            if(ret < 0)
            {
                break;
            }
            else if(ret > 0)
            {
                DEBUG_BUFFER_FMT(data,ret,"Mouse Data:");
            }

        }
    }

	FiniDirectInput();
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
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIDATA));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DIDATA);
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
    BOOL bret;
    int ret;

    hInst = hInstance; // 将实例句柄存储在全局变量中

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if(!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    bret = InitDirectInput(hInstance,hWnd);
    if(!bret)
    {
        ret = GetLastError();
        SetLastError(ret);
        return FALSE;
    }

    return TRUE;
}

int Read_Device(IDIRECTINPUTDEVICE8 *pDev,LPDIDEVICEOBJECTDATA pData,int datanum)
{
    int retsize=0;
    HRESULT hr;
    DWORD ditem=0;
    while(1)
    {
        ditem = datanum;
        hr = pDev->GetDeviceData(sizeof(*pData),pData,&ditem,0);
        if(hr == DI_OK)
        {
            break;
        }

        hr = pDev->Acquire();
        if(FAILED(hr))
        {
            ERROR_INFO("Acquire Error(0x%08x)\n",hr);
            return -1;
        }
    }

    return ditem*sizeof(*pData);
}

void FiniDirectInput(void)
{
    if(st_pKeyboardDev)
    {
        st_pKeyboardDev->Release();
    }
    st_pKeyboardDev = NULL;

    if(st_pMouseDev)
    {
        st_pMouseDev->Release();
    }
    st_pMouseDev = NULL;

    if(st_pInput)
    {
        st_pInput->Release();
    }
    st_pInput = NULL;

    return ;
}

BOOL InitDirectInput(HINSTANCE hInstance,HWND hwnd)
{
    int ret;
    HRESULT hr;
    DIPROPDWORD     property;

    hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID *)&st_pInput, NULL);
    if(FAILED(hr))
    {
        ERROR_INFO("Can Not Create Error(0x%08x)\n",hr);
        goto fail;
    }

    hr = st_pInput->CreateDevice(GUID_SysKeyboard, &st_pKeyboardDev, NULL);
    if(FAILED(hr))
    {
        ERROR_INFO("Create Keyboard Error(0x%08x)\n",hr);
        goto fail;
    }

    /*now for the format set and */
    hr = st_pKeyboardDev->SetDataFormat(&c_dfDIKeyboard);
    if(FAILED(hr))
    {
        ERROR_INFO("SetDataFormat Keyboard Error(0x%08x)\n",hr);
        goto fail;
    }

    /*set cooperative level*/
    hr = st_pKeyboardDev->SetCooperativeLevel(hwnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if(FAILED(hr))
    {
        ERROR_INFO("SetCooperativeLevel Keyboard Error(0x%08x)\n",hr);
        goto fail;
    }

    /*now for the buffer size set*/
    ZeroMemory(&property,sizeof(property));
    property.diph.dwSize =  sizeof(DIPROPDWORD);
    property.diph.dwHeaderSize =  sizeof(DIPROPHEADER);
    property.diph.dwObj = 0;
    property.diph.dwHow = DIPH_DEVICE;
    property.dwData = 16;

    hr = st_pKeyboardDev->SetProperty(DIPROP_BUFFERSIZE, &(property.diph));
    if(FAILED(hr))
    {
        ERROR_INFO("SetProperty BUFFERSIZE Error(0x%08x)\n",hr);
        goto fail;
    }

    /*we do not need any more return value check*/
    st_pKeyboardDev->Acquire();

    /*now to create mouse device*/
    hr = st_pInput->CreateDevice(GUID_SysMouse, &st_pMouseDev, NULL);
    if(FAILED(hr))
    {
        ERROR_INFO("Create Mouse Error(0x%08x)\n",hr);
        goto fail;
    }

    /*now for the format set and */
    hr = st_pMouseDev->SetDataFormat(&c_dfDIMouse);
    if(FAILED(hr))
    {
        ERROR_INFO("SetDataFormat Mouse Error(0x%08x)\n",hr);
        goto fail;
    }

    hr = st_pMouseDev->SetCooperativeLevel(hwnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if(FAILED(hr))
    {
        ERROR_INFO("SetCooperativeLevel Mouse Error(0x%08x)\n",hr);
        goto fail;
    }

    /*now for the buffer size set*/
    ZeroMemory(&property,sizeof(property));
    property.diph.dwSize =  sizeof(DIPROPDWORD);
    property.diph.dwHeaderSize =  sizeof(DIPROPHEADER);
    property.diph.dwObj = 0;
    property.diph.dwHow = DIPH_DEVICE;
    property.dwData = 16;

    hr = st_pMouseDev->SetProperty(DIPROP_BUFFERSIZE, &(property.diph));
    if(FAILED(hr))
    {
        ERROR_INFO("SetProperty BUFFERSIZE Error(0x%08x)\n",hr);
        goto fail;
    }

    st_pMouseDev->Acquire();



    SetLastError(0);
    return TRUE;
fail:
    SetLastError(ret);
    return FALSE;
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

    switch(message)
    {
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
