#include "stdafx.h"
#include "ModelessDialogTrackerDemo.h"
#include "ModelessDialogTrackerDemoDlg.h"

//////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
#include "DialogA.h"
void CModelessDialogTrackerDemoDlg::OnPopA()
{
	if (trackA.IsAlreadyPopped())
		return;

	DialogA* pnewdlg = new DialogA(trackA);	// note the passing of the tracker.
	pnewdlg->Create(pnewdlg->IDD,NULL);	
}
//////////////////////////////////////////////////////
#include "DialogB.h"
void CModelessDialogTrackerDemoDlg::OnPopB()
{
	if (trackB.IsAlreadyPopped())
		return;

	DialogB* pnewdlg = new DialogB(trackB);	// note the passing of the tracker.
	pnewdlg->Create(pnewdlg->IDD,NULL);	
}
void CModelessDialogTrackerDemoDlg::OnNotifyDialogB()
{
	if (!trackB.IsPopped())
	{
		AfxMessageBox(_T("Cannot notify a dialog that is not UP!"));
		return;
	}
	DialogB& dlg = (DialogB&)*trackB.GetDlg();
	dlg.IncrementCounter(4);

}
/////////////////////////////////////////////////////////
#include "DialogC.h"
void CModelessDialogTrackerDemoDlg::OnPopC()
{
	if (trackC.IsAlreadyPopped())
		return;

	DialogC* pnewdlg = new DialogC(trackC);	// note the passing of the tracker.
	pnewdlg->Create(pnewdlg->IDD,NULL);	
	ctrlStatusDlgC.SetWindowText(_T("Open."));
}
void CModelessDialogTrackerDemoDlg::OnDlgCClosed()
{
	ctrlStatusDlgC.SetWindowText(_T("Closed."));
}

///////////////////////////////////////////////////////
void CModelessDialogTrackerDemoDlg::OnCloseABC()
{
	trackA.CloseDialog();
	trackB.CloseDialog();
	trackC.CloseDialog();
}
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////


/// generic wizard stuff follows.......

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CModelessDialogTrackerDemoDlg dialog



CModelessDialogTrackerDemoDlg::CModelessDialogTrackerDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelessDialogTrackerDemoDlg::IDD, pParent)
	, trackC(*this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CModelessDialogTrackerDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STATUS, ctrlStatusDlgC);
}

BEGIN_MESSAGE_MAP(CModelessDialogTrackerDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnPopA)
	ON_BN_CLICKED(IDC_BUTTON2, OnPopB)
	ON_BN_CLICKED(IDC_BUTTON3, OnPopC)
	ON_BN_CLICKED(IDC_BUTTON4, OnCloseABC)
	ON_BN_CLICKED(IDC_BUTTON5, OnNotifyDialogB)
END_MESSAGE_MAP()


// CModelessDialogTrackerDemoDlg message handlers

BOOL CModelessDialogTrackerDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ctrlStatusDlgC.SetWindowText(_T("Closed"));
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CModelessDialogTrackerDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CModelessDialogTrackerDemoDlg::OnPaint() 
{
	if (IsIconic())
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CModelessDialogTrackerDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
