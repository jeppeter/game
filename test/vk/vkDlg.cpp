
// vkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vk.h"
#include "vkDlg.h"
#include "afxdialogex.h"
#include "output_debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CvkDlg dialog



CvkDlg::CvkDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CvkDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CvkDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CvkDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_GETKEYSTATE, OnGetKeyState)
    ON_BN_CLICKED(IDC_BTN_GETASYNCKEYSTATE, OnGetAsyncKeyState)
END_MESSAGE_MAP()


// CvkDlg message handlers

BOOL CvkDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CvkDlg::OnPaint()
{
    if(IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CvkDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CvkDlg::OnGetKeyState()
{
    USHORT ulshift1, ulshift2;
    USHORT urshift1, urshift2;
    USHORT ulctrl1,ulctrl2;
    USHORT urctrl1,urctrl2;
    USHORT ulmenu1,ulmenu2;
    USHORT urmenu1,urmenu2;
    USHORT ucaps1,ucaps2;
    CString timestr, infostr;
    CEdit* pEdt = NULL;
    int sleeptime = 0;

    pEdt = (CEdit*)this->GetDlgItem(IDC_EDT_MILLS);
    pEdt->GetWindowText(timestr);
#ifdef _UNICODE
    sleeptime = _wtoi((LPCWSTR)timestr);
#else
    sleeptime = atoi((LPCSTR)timestr);
#endif

    ulctrl1 = GetKeyState(VK_LCONTROL);
    urctrl1 = GetKeyState(VK_RCONTROL);
    ulshift1 = GetKeyState(VK_LSHIFT);
    urshift1 = GetKeyState(VK_RSHIFT);
    ulmenu1 =  GetKeyState(VK_LMENU);
    urmenu1 =  GetKeyState(VK_RMENU);
    ucaps1 = GetKeyState(VK_CAPITAL);
    Sleep(sleeptime);
    ulctrl2 = GetKeyState(VK_LCONTROL);
    urctrl2 = GetKeyState(VK_RCONTROL);
    ulshift2 = GetKeyState(VK_LSHIFT);
    urshift2 = GetKeyState(VK_RSHIFT);
    ulmenu2 =  GetKeyState(VK_LMENU);
    urmenu2 =  GetKeyState(VK_RMENU);
    ucaps2 = GetKeyState(VK_CAPITAL);
	DEBUG_INFO("GetKeyState(%d) lctrl1 (0x%04x) lctrl2 (0x%04x) rctrl1(0x%04x) rctrl2(0x%04x) lshift1 (0x%04x) lshift2 (0x%04x) rshift1 (0x%04x) rshift2 (0x%04x) lmenu1 (0x%04x) lmenu2 (0x%04x) rmenu1 (0x%04x) rmenu2 (0x%04x) caps1 (0x%04x) caps2 (0x%04x)",
                   sleeptime, ulctrl1, ulctrl2,
                   urctrl1,urctrl2,
                   ulshift1,ulshift2,
                   urshift1,urshift2,
                   ulmenu1,ulmenu2,
                   urmenu1,urmenu2,
                   ucaps1,ucaps2);
    infostr.Format(TEXT("GetKeyState(%s) lctrl1 (0x%04x) lctrl2 (0x%04x) rctrl1(0x%04x) rctrl2(0x%04x) lshift1 (0x%04x) lshift2 (0x%04x) rshift1 (0x%04x) rshift2 (0x%04x) lmenu1 (0x%04x) lmenu2 (0x%04x) rmenu1 (0x%04x) rmenu2 (0x%04x) caps1 (0x%04x) caps2 (0x%04x)"),
                   (LPCTSTR)timestr, ulctrl1, ulctrl2,
                   urctrl1,urctrl2,
                   ulshift1,ulshift2,
                   urshift1,urshift2,
                   ulmenu1,ulmenu2,
                   urmenu1,urmenu2,
                   ucaps1,ucaps2);
    this->MessageBox(infostr, TEXT("NOTICE"), MB_OK);
    return;
}



void CvkDlg::OnGetAsyncKeyState()
{
    USHORT ulshift1, ulshift2;
    USHORT urshift1, urshift2;
    USHORT ulctrl1,ulctrl2;
    USHORT urctrl1,urctrl2;
    USHORT ulmenu1,ulmenu2;
    USHORT urmenu1,urmenu2;
    USHORT ucaps1,ucaps2;
    CString timestr, infostr;
    CEdit* pEdt = NULL;
    int sleeptime = 0;

    pEdt = (CEdit*)this->GetDlgItem(IDC_EDT_MILLS);
    pEdt->GetWindowText(timestr);
#ifdef _UNICODE
    sleeptime = _wtoi((LPCWSTR)timestr);
#else
    sleeptime = atoi((LPCSTR)timestr);
#endif

    ulctrl1 = GetAsyncKeyState(VK_LCONTROL);
    urctrl1 = GetAsyncKeyState(VK_RCONTROL);
    ulshift1 = GetAsyncKeyState(VK_LSHIFT);
    urshift1 = GetAsyncKeyState(VK_RSHIFT);
    ulmenu1 =  GetAsyncKeyState(VK_LMENU);
    urmenu1 =  GetAsyncKeyState(VK_RMENU);
    ucaps1 = GetAsyncKeyState(VK_CAPITAL);
    Sleep(sleeptime);
    ulctrl2 = GetAsyncKeyState(VK_LCONTROL);
    urctrl2 = GetAsyncKeyState(VK_RCONTROL);
    ulshift2 = GetAsyncKeyState(VK_LSHIFT);
    urshift2 = GetAsyncKeyState(VK_RSHIFT);
    ulmenu2 =  GetAsyncKeyState(VK_LMENU);
    urmenu2 =  GetAsyncKeyState(VK_RMENU);
    ucaps2 = GetAsyncKeyState(VK_CAPITAL);
	DEBUG_INFO("GetAsyncKeyState(%d) lctrl1 (0x%04x) lctrl2 (0x%04x) rctrl1(0x%04x) rctrl2(0x%04x) lshift1 (0x%04x) lshift2 (0x%04x) rshift1 (0x%04x) rshift2 (0x%04x) lmenu1 (0x%04x) lmenu2 (0x%04x) rmenu1 (0x%04x) rmenu2 (0x%04x) caps1 (0x%04x) caps2 (0x%04x)",
                   sleeptime, ulctrl1, ulctrl2,
                   urctrl1,urctrl2,
                   ulshift1,ulshift2,
                   urshift1,urshift2,
                   ulmenu1,ulmenu2,
                   urmenu1,urmenu2,
                   ucaps1,ucaps2);
    infostr.Format(TEXT("GetAsyncKeyState(%s) lctrl1 (0x%04x) lctrl2 (0x%04x) rctrl1(0x%04x) rctrl2(0x%04x) lshift1 (0x%04x) lshift2 (0x%04x) rshift1 (0x%04x) rshift2 (0x%04x) lmenu1 (0x%04x) lmenu2 (0x%04x) rmenu1 (0x%04x) rmenu2 (0x%04x) caps1 (0x%04x) caps2 (0x%04x)"),
                   (LPCTSTR)timestr, ulctrl1, ulctrl2,
                   urctrl1,urctrl2,
                   ulshift1,ulshift2,
                   urshift1,urshift2,
                   ulmenu1,ulmenu2,
                   urmenu1,urmenu2,
                   ucaps1,ucaps2);
    this->MessageBox(infostr, TEXT("NOTICE"), MB_OK);
    return;
}
