
// controldemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "controldemo.h"
#include "controldemoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// 对话框数据
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CcontroldemoDlg 对话框




CcontroldemoDlg::CcontroldemoDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CcontroldemoDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pCapper = NULL;
    m_pDemoCallBack = NULL;
    m_hProc = NULL;
}

void CcontroldemoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CcontroldemoDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_CHK_RENDER, OnCheckBoxClick)
    ON_BN_CLICKED(IDC_CHK_CAPTURE,OnCheckBoxClick)
END_MESSAGE_MAP()


// CcontroldemoDlg 消息处理程序

BOOL CcontroldemoDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if(pSysMenu != NULL)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if(!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CcontroldemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CcontroldemoDlg::OnPaint()
{
    if(IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CcontroldemoDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CcontroldemoDlg::OnCheckBoxClick()
{
    if(this->m_pCapper == NULL || this->m_pDemoCallBack == NULL)
    {
        return ;
    }
    return ;
}


void CcontroldemoDlg::StartCapper()
{
    char *pExecAnsi=NULL,*pDllAnsi=NULL,*pParamAnsi=NULL,*pDumpAnsi=NULL,*pBufNumAnsi=NULL,*pBlockSizeAnsi=NULL;
	char *pPartDllAnsi=NULL;
    int rendercheck=0,capturecheck=0;
#ifdef _UNICODE
    int execansisize=0,dllansisize=0,paramansisize=0,dumpansisize=0,bufnumansisize=0,blocksizeansisize=0;
#endif
    int ret;
    CEdit* pEdt=NULL;
    CString errstr;
    CButton* pCheck=NULL;
    int iOperation=PCMCAP_AUDIO_NONE;
    BOOL bret;
    int bufnum,blocksize;



    pEdt = (CEdit*)this->GetDlgItem(IDC_EDT_EXE);
    pEdt->GetWindowText(this->m_strExec);
    pEdt = (CEdit*)this->GetDlgItem(IDC_EDT_PARAMETER);
    pEdt->GetWindowText(this->m_strParam);
    pEdt = (CEdit*)this->GetDlgItem(IDC_EDT_DLL);
    pEdt->GetWindowText(this->m_strDll);
    pEdt = (CEdit*)this->GetDlgItem(IDC_EDT_DUMP);
    pEdt->GetWindowText(this->m_strDump);
    pEdt = (CEdit*)this->GetDlgItem(IDC_EDT_BUFNUM);
    pEdt->GetWindowText(this->m_strBufNum);
    pEdt = (CEdit*)this->GetDlgItem(IDC_EDT_BLOCKSIZE);
    pEdt->GetWindowText(this->m_strBlockSize);


#ifdef _UNICODE
    ret = UnicodeToAnsi((wchar_t*)((const WCHAR*)this->m_strExec),&pExecAnsi,&execansisize);
    if(ret < 0)
    {
        errstr.Format(TEXT("can not Get exec string"));
        AfxMessageBox(errstr);
        goto free_release;
    }

    ret = UnicodeToAnsi((wchar_t*)((const WCHAR*)this->m_strParam),&pParamAnsi,&paramansisize);
    if(ret < 0)
    {
        errstr.Format(TEXT("can not get param string"));
        AfxMessageBox(errstr);
        goto free_release;
    }

    ret = UnicodeToAnsi((wchar_t*)((const WCHAR*)this->m_strDll),&pDllAnsi,&dllansisize);
    if(ret < 0)
    {
        errstr.Format(TEXT("can not get dll string"));
        AfxMessageBox(errstr);
        goto free_release;
    }

    ret = UnicodeToAnsi((wchar_t*)((const WCHAR*)this->m_strDump),&pDumpAnsi,&dumpansisize);
    if(ret < 0)
    {
        errstr.Format(TEXT("can not get dump string"));
        AfxMessageBox(errstr);
        goto free_release;
    }

    ret = UnicodeToAnsi((wchar_t*)((const WCHAR*)this->m_strBufNum),&pBufNumAnsi,&bufnumansisize);
    if(ret < 0)
    {
        errstr.Format(TEXT("can not get bufnum string"));
        AfxMessageBox(errstr);
        goto free_release;
    }

    ret = UnicodeToAnsi((wchar_t*)((const WCHAR*)this->m_strBlockSize),&pBlockSizeAnsi,&blocksizeansisize);
    if(ret < 0)
    {
        errstr.Format(TEXT("can not get blocksize string"));
        AfxMessageBox(errstr);
        goto free_release;
    }

#else
    pExecAnsi = (const char*) this->m_strExec;
    pDllAnsi = (const char*) this->m_strDll;
    pParamAnsi = (const char*) this->m_strParam;
    pDumpAnsi = (const char*) this->m_strDump;
    pBufNumAnsi = (const char*)this->m_strBufNum;
    pBlockSizeAnsi = (const char*) this->m_strBlockSize;
#endif


    pCheck = this->GetDlgItem(IDC_CHK_RENDER);
    rendercheck = pCheck->GetCheck();
    pCheck = this->GetDlgItem(IDC_CHK_CAPTURE);
    capturecheck = pCheck->GetCheck();


    bufnum = atoi(pBufNumAnsi);
    blocksize = atoi(pBlockSizeAnsi);
    if(bufnum < 1 || blocksize < 0x1000)
    {
        errstr.Format(TEXT("bufnum %d < 1 or blocksize %d < 0x1000"),bufnum,blocksize);
        AfxMessageBox(errstr);
        goto free_release;
    }

	pPartDllAnsi = strrchr(pDllAnsi,'\\');
	if (pPartDllAnsi == NULL)
		{
			
		}


    this->StopCapper();



    /*now to get the text  */
    this->m_pCapper = new CPcmCapper();
    this->m_pDemoCallBack = new CPcmCapDemoCallBack();

    /*now to make the dump file*/
    ret = this->m_pDemoCallBack->OpenFile(pDumpAnsi);
    if(ret < 0)
    {
        errstr.Format(TEXT("can not open (%s) error(%d)"),pDumpAnsi,ret);
        goto fail;
    }

    if(rendercheck == 0 && capturecheck == 0)
    {
        iOperation = PCMCAP_AUDIO_NONE;
    }
    else if(rendercheck == 0 && capturecheck)
    {
        iOperation = PCMCAP_AUDIO_CAPTURE;
    }
    else if(rendercheck  && capturecheck == 0)
    {
        iOperation = PCMCAP_AUDIO_RENDER;
    }
    else
    {
        iOperation = PCMCAP_AUDIO_BOTH;
    }

	/*now to create the process*/




    bret= this->m_pCapper->Start(this->m_hProc,iOperation,bufnum,blocksize,this->m_pDemoCallBack,NULL);
    if(!bret)
    {
    	ret = LAST_ERROR_CODE();
    	errstr.Format(TEXT("could not start %d operation error(%d)"),iOperation,ret);
        goto fail;
    }

	/*ok all is ok*/




    return ;

close_handle:
    if(this->m_hProc)
    {
        CloseHandle(this->m_hProc);
    }
    this->m_hProc = NULL;

fail:
    this->StopCapper();
free_release:
#ifdef _UNICODE
    UnicodeToAnsi(NULL,&pExecAnsi,&execansisize);
    UnicodeToAnsi(NULL,&pDllAnsi,&dllansisize);
    UnicodeToAnsi(NULL,&pParamAnsi,&paramansisize);
    UnicodeToAnsi(NULL,&pDumpAnsi,&dumpansisize);
    UnicodeToAnsi(NULL,&pBufNumAnsi,&bufnumansisize);
    UnicodeToAnsi(NULL,&pBlockSizeAnsi,&blocksizeansisize);
#endif
    return;

}

void CcontroldemoDlg::StopCapper()
{
    if(this->m_pCapper)
    {
        delete this->m_pCapper;
    }
    this->m_pCapper = NULL;

    if(this->m_pDemoCallBack)
    {
        delete this->m_pDemoCallBack;
    }
    this->m_pDemoCallBack = NULL;
    this->m_hProc = NULL;
    return ;
}
