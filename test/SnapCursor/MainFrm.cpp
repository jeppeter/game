// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SnapCursorDemo.h"
#include "SnapCursorDemoView.h"
#include "SceneView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	int Xmax;
	int Ymax;
	int Xmid;
	int Ymid;

	Xmax = GetSystemMetrics(SM_CXSCREEN);  	// Resolution of the screen
	Ymax = GetSystemMetrics(SM_CYSCREEN);

	Xmid = Xmax / 2; // Calculate the midpoint of the screen
	Ymid = Ymax / 2;

	int XnormL;
	int YnormT;
	int XSize;
	int YSize;

	XSize = (Xmax * 9) / 10;		// Size this app to be 9/10 the size of the screen
	YSize = (Ymax * 9) / 10;		//  (use this precidence for integer arithmetic)

//    XSize = Xmax;
//	YSize = Ymax;

	XnormL = Xmid - XSize / 2;	// Center it by subtracting half the size of the
	YnormT = Ymid - YSize / 2;	//  App from the midpoint of the screen
    if (CFrameWnd::PreCreateWindow(cs))
    {
		cs.cx = XSize;  // Stuff in the screen placement values 
		cs.cy = YSize; 
		cs.x = XnormL; 
		cs.y = YnormT;
//      cs.style &= ~FWS_ADDTOTITLE;

        return TRUE;
    }
    else
        return FALSE;

}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
    // ** Create the static splitter window
	if(!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	// ** Create two views and insert in to one of the splitter panes
	if(!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CSnapCursorDemoView), CSize(185,100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CSceneView),
		CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}
//    m_pView = (CSceneView *) m_wndSplitter.GetPane(0, 1);


	return TRUE;
	
}
