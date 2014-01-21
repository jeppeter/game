// getcursorbmp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "getcursorbmp.h"
#include <output_debug.h>

#define MAX_LOADSTRING 100

#define LAST_ERROR_CODE() (GetLastError() ? GetLastError() : 1)

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

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
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO:  在此放置代码。
    MSG msg;
    HACCEL hAccelTable;

    // 初始化全局字符串
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_GETCURSORBMP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if(!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GETCURSORBMP));

    // 主消息循环:
    while(GetMessage(&msg, NULL, 0, 0))
    {
        if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
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
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GETCURSORBMP));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GETCURSORBMP);
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

BOOL GetCursorBmp(HWND hwnd)
{
    HCURSOR hcursor=NULL;
    int ret;
    BOOL bret;
    ICONINFOEX iconex ;
    BITMAP maskbmp,colorbmp;
    BITMAPINFO maskinfo,colorinfo;
    HDC  hmaskdc=NULL,hcolordc=NULL;
    VOID *pMaskBuffer=NULL,*pColorBuffer=NULL;
    int masksize=0,colorsize=0,masklen,colorlen;


    ZeroMemory(&iconex,sizeof(iconex));
    hcursor = (HCURSOR)GetClassLongPtrW(hwnd,GCLP_HCURSOR);
    if(hcursor == NULL)
    {
        ERROR_INFO("hcursor not get\n");
        return FALSE;
    }

    iconex.cbSize = sizeof(iconex);
    bret = GetIconInfoEx(hcursor,&iconex);
    if(!bret)
    {
        ret=  LAST_ERROR_CODE();
        ERROR_INFO("can not get iconex Error(%d)\n",ret);
        goto fail;
    }

    bret = GetObject(iconex.hbmMask,sizeof(maskbmp),&maskbmp);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("[0x%08x]can not get bmpmask bitmap Error(%d)\n",iconex.hbmMask,ret);
        goto fail;
    }

    bret = GetObject(iconex.hbmColor,sizeof(colorbmp),&colorbmp);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("[0x%08x]can not get bmpcolor bitmap Error(%d)\n",iconex.hbmColor,ret);
        goto fail;
    }

    hmaskdc = GetDC(NULL);
    if(hmaskdc == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not get hmaskdc Error(%d)\n",ret);
        goto fail;
    }

    hcolordc = GetDC(NULL);
    if(hcolordc == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not get hcolordc Error(%d)\n",ret);
        goto fail;
    }

    masksize = maskbmp.bmWidth * maskbmp.bmHeight * (maskbmp.bmBitsPixel + 7) / 8;
    pMaskBuffer = malloc(masksize);
    if(pMaskBuffer == NULL)
    {
        ret=  LAST_ERROR_CODE();
        goto fail;
    }

    colorsize = colorbmp.bmWidth * colorbmp.bmHeight * (colorbmp.bmBitsPixel + 7)/ 8;
    pColorBuffer = malloc(colorsize);
    if(pColorBuffer == NULL)
    {
        ret= LAST_ERROR_CODE();
        goto fail;
    }

    ZeroMemory(&maskinfo,sizeof(maskinfo));
    maskinfo.bmiHeader.biSize = sizeof(maskinfo.bmiHeader);
	maskinfo.bmiHeader.biWidth = maskbmp.bmWidth;
	maskinfo.bmiHeader.biHeight = maskbmp.bmHeight;
	maskinfo.bmiHeader.biPlanes = 1;
	maskinfo.bmiHeader.biBitCount = maskbmp.bmBitsPixel;
	maskinfo.bmiHeader.biCompression = BI_RGB;
	maskinfo.bmiHeader.biSizeImage = masksize;
	maskinfo.bmiHeader.biClrUsed = 0;
	maskinfo.bmiHeader.biClrImportant = 0;
    ret = ::GetDIBits(hmaskdc,iconex.hbmMask,0,maskbmp.bmHeight,pMaskBuffer,&maskinfo, DIB_RGB_COLORS);
    if(ret == 0)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("[0x%08x]HBITMAP(0x%08x) lines (%d) mask GetDIBits Error(%d)\n",
                   hmaskdc,iconex.hbmMask,maskbmp.bmHeight,ret);
        goto fail;
    }
	masklen = ret;

	ZeroMemory(&colorinfo,sizeof(colorinfo));
    colorinfo.bmiHeader.biSize = sizeof(colorinfo.bmiHeader);
	colorinfo.bmiHeader.biWidth = colorbmp.bmWidth;
	colorinfo.bmiHeader.biHeight = colorbmp.bmHeight;
	colorinfo.bmiHeader.biPlanes = 1;
	colorinfo.bmiHeader.biBitCount = colorbmp.bmBitsPixel;
	colorinfo.bmiHeader.biCompression = BI_RGB;
	colorinfo.bmiHeader.biSizeImage = colorsize;
	colorinfo.bmiHeader.biClrUsed = 0;
	colorinfo.bmiHeader.biClrImportant = 0;
    SetLastError(0);
    ret = ::GetDIBits(hcolordc,iconex.hbmColor,0,colorbmp.bmHeight,pColorBuffer,&colorinfo, DIB_RGB_COLORS);
    if(ret == 0)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("[0x%08x]HBITMAP(0x%08x) lines (%d) color GetDIBits Error(%d)\n",
                   hcolordc,iconex.hbmColor,colorbmp.bmHeight,ret);
        goto fail;
    }
	colorlen = ret;

    /*now to display the information*/
    DEBUG_BUFFER_FMT(&maskinfo,sizeof(maskinfo),"size:%d width:%d height:%d bitcount:%d",maskinfo.bmiHeader.biSize,
                     maskinfo.bmiHeader.biWidth,maskinfo.bmiHeader.biHeight,maskinfo.bmiHeader.biBitCount);
    DEBUG_BUFFER_FMT(pMaskBuffer,masksize,"Mask(0x%08x) w:%d h:%d pixel:%d",iconex.hbmMask,maskbmp.bmWidth,
                     maskbmp.bmHeight,maskbmp.bmBitsPixel);
    DEBUG_BUFFER_FMT(&colorinfo,sizeof(colorinfo),"size:%d width:%d height:%d bitcount:%d",colorinfo.bmiHeader.biSize,
                     colorinfo.bmiHeader.biWidth,colorinfo.bmiHeader.biHeight,colorinfo.bmiHeader.biBitCount);
    DEBUG_BUFFER_FMT(pColorBuffer,colorsize,"Color(0x%08x) w:%d h:%d pixel:%d",iconex.hbmColor,colorbmp.bmWidth,
                     colorbmp.bmHeight,colorbmp.bmBitsPixel);


    if(pMaskBuffer)
    {
        free(pMaskBuffer);
    }
    pMaskBuffer = NULL;
    if(pColorBuffer)
    {
        free(pColorBuffer);
    }
    pColorBuffer = NULL;

    if(hmaskdc)
    {
        ReleaseDC(NULL,hmaskdc);
    }
    hmaskdc = NULL;
    if(hcolordc)
    {
        ReleaseDC(NULL,hcolordc);
    }
    hcolordc = NULL;
#if 0
    if(iconex.hbmMask)
    {
        DeleteObject(iconex.hbmMask);
    }
    iconex.hbmMask = NULL;

    if(iconex.hbmColor)
    {
        DeleteObject(iconex.hbmColor);
    }
    iconex.hbmColor = NULL;
#endif
    SetLastError(0);
    return TRUE;

fail:
    if(pMaskBuffer)
    {
        free(pMaskBuffer);
    }
    pMaskBuffer = NULL;
    if(pColorBuffer)
    {
        free(pColorBuffer);
    }
    pColorBuffer = NULL;
    if(hmaskdc)
    {
        ReleaseDC(NULL,hmaskdc);
    }
    hmaskdc = NULL;
    if(hcolordc)
    {
        ReleaseDC(NULL,hcolordc);
    }
    hcolordc = NULL;
#if 0
    if(iconex.hbmMask)
    {
        DeleteObject(iconex.hbmMask);
    }
    iconex.hbmMask = NULL;

    if(iconex.hbmColor)
    {
        DeleteObject(iconex.hbmColor);
    }
    iconex.hbmColor = NULL;
#endif
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
        case IDM_GETCURSORBMP:
            GetCursorBmp(hWnd);
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
