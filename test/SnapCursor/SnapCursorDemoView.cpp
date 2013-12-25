// SnapCursorDemoView.cpp : implementation of the CSnapCursorDemoView class
//

#include "stdafx.h"
#include "SnapCursorDemo.h"

#include "SnapCursorDemoDoc.h"
#include "SnapCursorDemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSnapCursorDemoView

IMPLEMENT_DYNCREATE(CSnapCursorDemoView, CFormView)

BEGIN_MESSAGE_MAP(CSnapCursorDemoView, CFormView)
	//{{AFX_MSG_MAP(CSnapCursorDemoView)
	ON_BN_CLICKED(IDC_RADIO_SNAPINCREMENT_5, OnRadioSnapincrement5)
	ON_BN_CLICKED(IDC_RADIO_SNAPINCREMENT_10, OnRadioSnapincrement10)
	ON_BN_CLICKED(IDC_RADIO_SNAP_INCREMENT_50, OnRadioSnapIncrement50)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSnapCursorDemoView construction/destruction

CSnapCursorDemoView::CSnapCursorDemoView()
	: CFormView(CSnapCursorDemoView::IDD)
{
	//{{AFX_DATA_INIT(CSnapCursorDemoView)
	m_RadioSnapIncrement = -1;
	//}}AFX_DATA_INIT
	// TODO: add construction code here

}

CSnapCursorDemoView::~CSnapCursorDemoView()
{
}

void CSnapCursorDemoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSnapCursorDemoView)
	DDX_Radio(pDX, IDC_RADIO_SNAPINCREMENT_5, m_RadioSnapIncrement);
	//}}AFX_DATA_MAP
}

BOOL CSnapCursorDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CSnapCursorDemoView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
    m_RadioSnapIncrement = 1;  // Init radio increment selection to 10 units selection
    UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CSnapCursorDemoView printing

BOOL CSnapCursorDemoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSnapCursorDemoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSnapCursorDemoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CSnapCursorDemoView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CSnapCursorDemoView diagnostics

#ifdef _DEBUG
void CSnapCursorDemoView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSnapCursorDemoView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSnapCursorDemoDoc* CSnapCursorDemoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSnapCursorDemoDoc)));
	return (CSnapCursorDemoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSnapCursorDemoView message handlers

void CSnapCursorDemoView::OnRadioSnapincrement5() 
{
	// TODO: Add your control notification handler code here
	CSnapCursorDemoDoc* pDoc = GetDocument();
    pDoc->m_pSceneView->SetViewSnapIncrement(5);
}

void CSnapCursorDemoView::OnRadioSnapincrement10() 
{
	// TODO: Add your control notification handler code here
	CSnapCursorDemoDoc* pDoc = GetDocument();
    pDoc->m_pSceneView->SetViewSnapIncrement(10);
	
}

void CSnapCursorDemoView::OnRadioSnapIncrement50() 
{
	// TODO: Add your control notification handler code here
	CSnapCursorDemoDoc* pDoc = GetDocument();
    pDoc->m_pSceneView->SetViewSnapIncrement(50);	
	
}

