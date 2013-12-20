// testrawmouseWin32App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#define _WIN32_WINNT 0x0501 
#include "windows.h"

#define MAX_LOADSTRING 100

  template <class T> 
  class AutoArray { 
      public: 
          AutoArray(size_t n) 
          { 
              m_ptr = new T[n]; 
              if (m_ptr) { 
                  m_size = n; 
              } 
              else { 
                  m_size = 0; 
              } 
     
          } 
          ~AutoArray() 
          { 
              delete[] m_ptr; 
          } 
     
          bool OK() 
          { 
              return m_ptr != NULL; 
          } 
     
          operator T*() 
          { 
              return m_ptr; 
          } 
     
          T& operator[](size_t n) 
          { 
              //ASSERT(n < m_size); 
              return m_ptr[n]; 
          } 
     
      protected: 
          // to prohibit copy operations 
          AutoArray(const AutoArray<T>&); 
          AutoArray<T> operator=(const AutoArray<T>&); 
     
          enum { SENTINEL = 0xcc }; 
     
      protected: 
          T* m_ptr; 
          size_t m_size; 
  }; 


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TESTRAWMOUSEWIN32APP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TESTRAWMOUSEWIN32APP);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_TESTRAWMOUSEWIN32APP);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_TESTRAWMOUSEWIN32APP;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

bool fcnDrawRawInputInfo(HDC hdc, HWND hWnd)
{
	char cBuf[255] = ""; 

	// TODO: Add any drawing code here...
	RECT rt;
	GetClientRect(hWnd, &rt);

	// Acquiring the number of input devices which presently are connected 
	UINT nInputDevices; 
	if (GetRawInputDeviceList(NULL, &nInputDevices, sizeof(RAWINPUTDEVICELIST)) != 0) 
	{ 
		return false; 
	} 
	
	wsprintf(cBuf, "Number of raw input devices: %i", nInputDevices);
	DrawText(hdc, cBuf, strlen(cBuf), &rt, DT_LEFT);
	rt.top = rt.top + 20;

	//AutoArray<RAWINPUTDEVICELIST> rawinputDeviceList(nInputDevices); 
	AutoArray<RAWINPUTDEVICELIST> rawinputDeviceList(nInputDevices);
	if (!rawinputDeviceList.OK()) { 
		return false; 
	} 

	UINT ntmp = nInputDevices; 
	if (GetRawInputDeviceList(rawinputDeviceList, &nInputDevices, sizeof(RAWINPUTDEVICELIST)) != ntmp) { 
		return false; 
	} 

	int nKbd = 0;   // The number of keyboards which are connected 
    
	for (UINT i = 0; i < nInputDevices; ++i) { 
		wsprintf(cBuf, "Device %i:", i);
		DrawText(hdc, cBuf, strlen(cBuf), &rt, DT_LEFT);
		rt.top = rt.top + 20;

		// Acquiring the length of device name 
		UINT size; 
		if (GetRawInputDeviceInfo(rawinputDeviceList[i].hDevice, RIDI_DEVICENAME, NULL, &size) != 0) { 
			return false; 
		} 
			
		// Acquiring device name 
		AutoArray<TCHAR> name(size); 
		if (!name.OK()) { 
			return false; 
		}
		
		if ((int)GetRawInputDeviceInfo(rawinputDeviceList[i].hDevice, RIDI_DEVICENAME, name, &size) < 0) { 
			return false; 
		} 
		// Information of device 
		static const TCHAR* deviceTypeNames[] = { 
			_T("Mouse"), 
			_T("Keybd"), 
			_T("HID  "), 
		};

		// Output the Device name
		wsprintf(cBuf, "             NAME:                    %s", name);
		DrawText(hdc, cBuf, strlen(cBuf), &rt, DT_LEFT);
		rt.top = rt.top + 20;

		// Out put the device type
		wsprintf(cBuf, "             TYPE:                    %s", deviceTypeNames[rawinputDeviceList[i]. dwType]);
		DrawText(hdc, cBuf, strlen(cBuf), &rt, DT_LEFT);
		rt.top = rt.top + 20;
		//TRACE3("[%02d] %s hDevice=%p", i, deviceTypeNames[rawinputDeviceList[i]. dwType], rawinputDeviceList[i].hDevice); 
		//TRACE1(" %s\n", name); 

        if (rawinputDeviceList[i].dwType == RIM_TYPEKEYBOARD) { 
            // When it is the keyboard, type sub type the  
            ++nKbd; 
     
            RID_DEVICE_INFO devinfo = { sizeof devinfo, }; 
            UINT sz = sizeof devinfo; 
            if ((int)GetRawInputDeviceInfo(rawinputDeviceList[i]. hDevice, RIDI_DEVICEINFO, &devinfo, &sz) < 0) { 
                return false; 
            } 

			wsprintf(cBuf, "             (KEYBOARD) Type, Subtype: %i, %i", devinfo.keyboard.dwType, devinfo.keyboard.dwSubType);
			DrawText(hdc, cBuf, strlen(cBuf), &rt, DT_LEFT);
			rt.top = rt.top + 20;
        } 

		else if (rawinputDeviceList[i].dwType == RIM_TYPEMOUSE)
		{
            RID_DEVICE_INFO devinfomouse = { sizeof devinfomouse, }; 
            UINT sz = sizeof devinfomouse; 
            if ((int)GetRawInputDeviceInfo(rawinputDeviceList[i]. hDevice, RIDI_DEVICEINFO, &devinfomouse, &sz) < 0) { 
                return false; 
            } 

			wsprintf(cBuf, "             (MOUSE) ID, #Buttons, Sample Rate:  %i, %i, %i", devinfomouse.mouse.dwId, devinfomouse.mouse.dwNumberOfButtons, devinfomouse.mouse.dwSampleRate);
			DrawText(hdc, cBuf, strlen(cBuf), &rt, DT_LEFT);
			rt.top = rt.top + 20;
	
		}



	}
	return true;
	
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;   

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
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
			fcnDrawRawInputInfo(hdc, hWnd); // JAKE: I should probly not be passing this entire object
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

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
