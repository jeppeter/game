#include <windows.h>
#include <output_debug.h>
HINSTANCE g_hInstance = (HINSTANCE)GetModuleHandle(NULL);
HWND g_hMainWnd = NULL;
bool g_MovingMainWnd = false;
POINT g_OrigCursorPos;
POINT g_OrigWndPos;
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hRetWnd=NULL;
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		// here you can add extra check and decide whether to start
		// the window move or not
		if (GetCursorPos(&g_OrigCursorPos))
		{
			RECT rt;
			GetWindowRect(hWnd, &rt);
			g_OrigWndPos.x = rt.left;
			g_OrigWndPos.y = rt.top;
			g_MovingMainWnd = true;
			hRetWnd = SetCapture(hWnd);
			DEBUG_INFO("Set Capture (0x%08x) ret (0x%08x)\n",hWnd,hRetWnd);
			SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		}
		return 0;

		// THE RIGHT WAY OF DOING IT:
		//*  <- Remove a slash to comment out the good version!
	case WM_LBUTTONUP:
		DEBUG_INFO("Release Capture Before\n");
		ReleaseCapture();
		DEBUG_INFO("Release Capture After\n");
		return 0;
	case WM_CAPTURECHANGED:
		DEBUG_INFO("main window changeto (%d) => (%d)\n", g_MovingMainWnd, (HWND)lParam == hWnd ? 1 : 0);
		g_MovingMainWnd = (HWND)lParam == hWnd;
		return 0;
		/**/

		// THE WRONG WAY OF DOING IT:
		/*  <- Prefix this with a slash to uncomment the bad version!
		case WM_LBUTTONUP:
		g_MovingMainWnd = false;
		ReleaseCapture();
		return 0;
		// buggy programs usually do not handle WM_CAPTURECHANGED at all
		case WM_CAPTURECHANGED:
		break;
		/**/

	case WM_MOUSEMOVE:
		if (g_MovingMainWnd)
		{
			POINT pt;
			if (GetCursorPos(&pt))
			{
				int wnd_x = g_OrigWndPos.x +
					(pt.x - g_OrigCursorPos.x);
				int wnd_y = g_OrigWndPos.y +
					(pt.y - g_OrigCursorPos.y);
				SetWindowPos(hWnd, NULL, wnd_x,
					wnd_y, 0, 0, SWP_NOACTIVATE |
					SWP_NOOWNERZORDER | SWP_NOZORDER |
					SWP_NOSIZE);
			}
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
bool CreateMainWnd()
{
	static const  TCHAR CLASS_NAME[] = TEXT("MainWndClass");
	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = g_hInstance;
	wc.lpfnWndProc = &MainWndProc;
	wc.lpszClassName = CLASS_NAME;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	if (!RegisterClass(&wc))
		return false;
	g_hMainWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		TEXT("Main Window"),
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
		NULL,
		NULL,
		g_hInstance,
		NULL
		);
	return true;
}
int CALLBACK WinMain(
	 HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
	)
{
	if (!CreateMainWnd())
		return -1;
	ShowWindow(g_hMainWnd, SW_SHOW);
	UpdateWindow(g_hMainWnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

