
#include "stdafx.h"
#include "rawinput.h"
#include "output_debug.h"
#include <stdint.h>
#include <assert.h>
#include <Windowsx.h>

#define MAX_LOADSTRING 100

HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int GetAllRawInputInfos(HWND hwnd)
{
    RAWINPUTDEVICE *pRegisterDevices=NULL;
    RAWINPUTDEVICELIST *pRawInputList=NULL;
    int ret;
    BOOL bret;
    UINT i,uret,ulistnum;
    UINT ucbsize,ucbmaxsize=0;
    void* pData=NULL;
    RID_DEVICE_INFO *pInfo=NULL;

    pRegisterDevices =(RAWINPUTDEVICE*) calloc(sizeof(*pRegisterDevices),2);
    if(pRegisterDevices == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    pRegisterDevices[0].usUsagePage = 0x1;
    pRegisterDevices[0].usUsage = 0x6;  /*for keyboard*/
    pRegisterDevices[0].dwFlags = RIDEV_CAPTUREMOUSE;
    pRegisterDevices[0].hwndTarget = NULL;

    pRegisterDevices[1].usUsagePage = 0x1;
    pRegisterDevices[1].usUsage = 0x2;  /*for mouse*/
    pRegisterDevices[1].dwFlags = RIDEV_INPUTSINK;
    pRegisterDevices[1].hwndTarget = NULL;

    bret = RegisterRawInputDevices(pRegisterDevices,1,sizeof(*pRegisterDevices));
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("Could not RegisterDevices Error(%d)\n",ret);
        goto fail;
    }

    ulistnum = 0;
    bret = GetRawInputDeviceList(NULL,&ulistnum,sizeof(*pRawInputList));
    ret = LAST_ERROR_CODE();
    DEBUG_INFO("GetRawInputDeviceList Error(%d) ulistnum(%d)\n",ret,ulistnum);
    pRawInputList = (RAWINPUTDEVICELIST*)calloc(sizeof(*pRawInputList),ulistnum);
    if(pRawInputList == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    bret = GetRawInputDeviceList(pRawInputList,&ulistnum,sizeof(*pRawInputList));
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("GetRawInputDeviceList Error(%d) ulistnum(%d)\n",ret,ulistnum);
        goto fail;
    }

    assert(pData == NULL);
    assert(ucbmaxsize == 0);
    for(i=0; i<ulistnum; i++)
    {
        DEBUG_INFO("[%d] hDevice(0x%08x) dwType %d\n",i,pRawInputList[i].hDevice,pRawInputList[i].dwType);
        while(1)
        {
            ucbsize = ucbmaxsize;
            uret = GetRawInputDeviceInfoA(pRawInputList[i].hDevice,RIDI_DEVICENAME,pData,&ucbsize);
            if(uret != 0 && uret != (UINT)-1)
            {
                break;
            }

            if(ucbsize <= ucbmaxsize)
            {
                ret = LAST_ERROR_CODE();
                ERROR_INFO("[%d] hDevice(0x%08x) GetName Error(%d)\n",i,pRawInputList[i].hDevice,ret);
                goto fail;
            }

            if(pData)
            {
                free(pData);
            }
            pData = NULL;
            ucbmaxsize = ucbsize;
            pData = calloc(1,ucbmaxsize);
            if(pData == NULL)
            {
                ret = LAST_ERROR_CODE();
                goto fail;
            }
        }

        DEBUG_INFO("ucbmaxsize %d hDevice (0x%08x) Name (%s)\n",ucbmaxsize,pRawInputList[i].hDevice,pData);
        while(1)
        {
            ucbsize = ucbmaxsize;
            if(ucbsize >= sizeof(*pInfo))
            {
                ucbsize = sizeof(*pInfo);
            }
            uret = GetRawInputDeviceInfoW(pRawInputList[i].hDevice,RIDI_DEVICEINFO,pData,&ucbsize);
            if(uret != 0 && uret != (UINT)-1)
            {
                break;
            }

            if(ucbsize <= ucbmaxsize)
            {
                ret = LAST_ERROR_CODE();
                ERROR_INFO("[%d] hDevice(0x%08x) (ucbsize %d ucbmaxsize %d) GetInfo Error(%d)\n",i,pRawInputList[i].hDevice
                           ,ucbsize,ucbmaxsize,ret);
                goto next_cycle;
            }

            if(pData)
            {
                free(pData);
            }
            pData = NULL;
            ucbmaxsize = ucbsize << 1;
            pData = calloc(1,ucbmaxsize);
            if(pData == NULL)
            {
                ret = LAST_ERROR_CODE();
                goto fail;
            }
        }

        pInfo = (RID_DEVICE_INFO*)pData;
        if(pInfo->dwType == RIM_TYPEMOUSE)
        {
            DEBUG_INFO("Mouse dwId %d dwNumberOfButtons %d dwSampleRate %d fHasHorizontalWheel %d\n",
                       pInfo->mouse.dwId,pInfo->mouse.dwNumberOfButtons,pInfo->mouse.dwSampleRate,pInfo->mouse.fHasHorizontalWheel);
        }
        else if(pInfo->dwType == RIM_TYPEKEYBOARD)
        {
            DEBUG_INFO("Keyboard dwType %d dwSubType %d dwKeyboardMode %d dwNumberOfFunctionKeys %d dwNumberOfIndicators %d dwNumberOfKeysTotal %d\n",pInfo->keyboard.dwType,pInfo->keyboard.dwSubType,
                       pInfo->keyboard.dwKeyboardMode,
                       pInfo->keyboard.dwNumberOfFunctionKeys,
                       pInfo->keyboard.dwNumberOfIndicators,
                       pInfo->keyboard.dwNumberOfKeysTotal);
        }
        else if(pInfo->dwType == RIM_TYPEHID)
        {
            DEBUG_INFO("HID dwVendorId 0x%08x dwProductId 0x%08x dwVersionNumber 0x%08x usUsagePage %d usUsage %d\n",pInfo->hid.dwVendorId,
                       pInfo->hid.dwProductId,pInfo->hid.dwVersionNumber,
                       pInfo->hid.usUsagePage,pInfo->hid.usUsage);
        }
        else
        {
            DEBUG_BUFFER_FMT(((uint8_t*)pInfo+4),pInfo->cbSize-4,"Unkown Type %d",pInfo->dwType);
        }

next_cycle:
        i = i;
    }


    if(pData)
    {
        free(pData);
    }
    pData = NULL;
    if(pRawInputList)
    {
        free(pRawInputList);
    }
    pRawInputList = NULL;
    if(pRegisterDevices)
    {
        free(pRegisterDevices);
    }
    pRegisterDevices = NULL;
    SetLastError(0);
    return 0;
fail:
    assert(ret > 0);
    if(pData)
    {
        free(pData);
    }
    pData = NULL;
    if(pRawInputList)
    {
        free(pRawInputList);
    }
    pRawInputList = NULL;
    if(pRegisterDevices)
    {
        free(pRegisterDevices);
    }
    pRegisterDevices = NULL;
    SetLastError(ret);
    return -ret;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                       _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine,
                       _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    // TODO:
    MSG msg;
    HACCEL hAccelTable;

    //
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_RAWINPUT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    //
    if(!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RAWINPUT));

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



ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RAWINPUT));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_RAWINPUT);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
	RECT rRect={0};

    hInst = hInstance; //

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if(!hWnd)
    {
        return FALSE;
    }

    GetAllRawInputInfos(hWnd);
	MoveWindow(hWnd,100,100,1000,800,TRUE);
	GetWindowRect(hWnd,&rRect);
	DEBUG_INFO("rRect.left %d rRect.top %d rRect.right %d rRect.bottom %d\n",
		rRect.left,rRect.top,rRect.right,rRect.bottom);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;
    static UINT mousetick=0;
    UINT curtick=0;

    switch(message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
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

    case WM_MOUSEMOVE:
        curtick = GetTickCount();
        if((curtick - mousetick)>50)
        {
            UINT xpos,ypos;
            xpos = GET_X_LPARAM(lParam);
            ypos = GET_Y_LPARAM(lParam);
            DEBUG_INFO("lParam 0x%08x X %d Y %d\n",lParam,xpos,ypos);
            mousetick = curtick;
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO:
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
