// SceneView.cpp : implementation file
//

#include "stdafx.h"
#include "SnapCursorDemo.h"
#include "SnapCursorDemoDoc.h"
#include "SceneView.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSceneView

IMPLEMENT_DYNCREATE(CSceneView, CView)

CSceneView::CSceneView()
{
    m_bgColor = RGB(0, 0, 0);
    m_LButtonIsDown = FALSE;
    m_MouseHasMoved = FALSE;
    m_X_size = 640;
    m_Y_size = 480;

//ADDTHIS    m_SnapCursor.SetSnapIncrement(10);    // Increment of SnapCursor

}

CSceneView::~CSceneView()
{
}


BEGIN_MESSAGE_MAP(CSceneView, CView)
	//{{AFX_MSG_MAP(CSceneView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSceneView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	CSnapCursorDemoDoc *pDoc = (CSnapCursorDemoDoc *)GetDocument();
    pDoc->m_pSceneView = this;	
}


/////////////////////////////////////////////////////////////////////////////
// CSceneView drawing

void CSceneView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CSceneView diagnostics

#ifdef _DEBUG
void CSceneView::AssertValid() const
{
	CView::AssertValid();
}

void CSceneView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSceneView message handlers

BOOL CSceneView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
    // Reset or ir-initialize snapcursor mechanism ("flip-flop")
    m_SnapCursor.Reset(); // MUST ADD FOR SNAPCURSOR
	CSnapCursorDemoDoc* pDoc = (CSnapCursorDemoDoc *)GetDocument();

    CBrush brSceneView(m_bgColor);         // Create a brush for background color
	CRect rcClient;
	GetClientRect(&rcClient);              // Get size of client area
	pDC->FillRect(rcClient, &brSceneView); //  and fill it with black
	DrawGrid(pDC);        // Redraw grid
    return TRUE;
}

void CSceneView::DrawGrid(CDC *pDC)
{
	CSnapCursorDemoDoc* pDoc = (CSnapCursorDemoDoc *)GetDocument();
    // Paint a grid

    int X_center, Y_center;
    int WideWidth;
    if(m_X_size > 700 || m_Y_size > 500)
        WideWidth = 3;
    else
        WideWidth = 2;

    X_center = (int)m_X_size / 2;
    Y_center = (int)m_Y_size / 2;
    int spacing[2]  = {10, 100};
    int x, y, grid;
    CPen narrowPen, widePen;
    narrowPen.CreatePen(PS_SOLID, 1, RGB(100,100,100));
    widePen.CreatePen(PS_SOLID, WideWidth, RGB(140,140,140));
	pDC->SelectObject(&narrowPen);
    for(grid = 0; grid < 2 ; grid++)
    {
        if(grid > 0)
            pDC->SelectObject(&widePen);           


        for(x = X_center; x > 0; x -= spacing[grid])
        {
            pDC->MoveTo(x, 0);
            pDC->LineTo(x, (int)m_Y_size);
        }
        for(X_center; x < (int)m_X_size; x += spacing[grid])
        {
            pDC->MoveTo(x, 0);
            pDC->LineTo(x, (int)m_Y_size);
        }
        for(y = Y_center; y > 0; y -= spacing[grid])
        {
            pDC->MoveTo(0, y);
            pDC->LineTo((int)m_X_size, y);
        }
        for(Y_center; y < (int)m_Y_size; y += spacing[grid])
        {
            pDC->MoveTo(0, y);
            pDC->LineTo((int)m_X_size, y);
        }
    }
}

void CSceneView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
    CDC* pDC = GetDC();
	CRect rcClient;
	GetClientRect(&rcClient);

    // Get First Snap position for drawing circle
    m_SnapCursor.GetFirstSnapPos(&point);    // MUST ADD FOR SNAPCURSOR

    m_OriginPoint.x = point.x;
    m_OriginPoint.y = point.y;

    // Store the origin to be used during mouse movement
    // to rubber-band circle.
    m_OldPoint = m_OriginPoint;

    m_LButtonIsDown = TRUE;
    m_MouseHasMoved = FALSE;    // Button is depressed, but may not have moved
    ReleaseDC(pDC);                
	
	CView::OnLButtonDown(nFlags, point);
}

void CSceneView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    double radius;
    CDC* pDC = GetDC();
    CPen thePen;
    CPen* pOldPen = pDC->GetCurrentPen();

    thePen.CreatePen(PS_SOLID, 1, RGB(255,255,0));
	pDC->SelectObject(&thePen);


    if(m_LButtonIsDown)
    {

        m_LButtonIsDown = FALSE;              // We're not dragging, anymore

        if(m_MouseHasMoved)             // Don't do an erase if mouse not moved
        {
            m_MouseHasMoved = FALSE;    
            pDC->DPtoLP(&point);        // Map the up click to Logical coords
            // Draw the circle at the final rubberband position on mouse release
            radius = Circle2d(pDC, m_OriginPoint.x, m_OriginPoint.y, m_OldPoint.x, m_OldPoint.y);
            ReleaseDC(pDC);
        }
    }
	
	CView::OnLButtonUp(nFlags, point);
}

void CSceneView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    int old_mode;

    CDC* pDC = GetDC();
    // Display Cursor and get snapped point value
    m_SnapCursor.Draw(pDC, &point);    // MUST ADD FOR SNAPCURSOR

    if(m_LButtonIsDown)
    {
        m_MouseHasMoved = TRUE;
        CDC* pDC = GetDC();

        old_mode = pDC->SetROP2(R2_NOT);
        // Rubberband circle, while mouse is down
        Circle2d(pDC, m_OriginPoint.x, m_OriginPoint.y, m_OldPoint.x, m_OldPoint.y);
        Circle2d(pDC, m_OriginPoint.x, m_OriginPoint.y, point.x, point.y);
        pDC->SetROP2(old_mode);
        m_OldPoint = point;
        ReleaseDC(pDC);                

    }       
	
	CView::OnMouseMove(nFlags, point);
}

//
// Draw a circle from center at Xc, Yc out to Xr, Yr
// Using the Ellipse function
//
double CSceneView::Circle2d(CDC *pDC, int Xc, int Yc, int Xr, int Yr)
{
	double x1, y1, x2, y2, dx, dy;
    double R;

    dx = (Xr - Xc);
    dy = (Yr - Yc);
    R = sqrt(dx * dx + dy * dy);
    x1 = Xc - R;
    y1 = Yc - R;
    x2 = x1 + 2 * R;
    y2 = y1 + 2 * R;
    pDC->SelectStockObject(NULL_BRUSH);
	pDC->Ellipse((int)x1, (int)y1, (int)x2, (int)y2);

    return R;
}


void CSceneView::SetViewSnapIncrement(int nIncrement)
{
    m_SnapCursor.SetSnapIncrement(nIncrement);
    if(nIncrement == 50)
        m_SnapCursor.SetSnapOffset(20, 40);
    else
        m_SnapCursor.SetSnapOffset(0,0);
}


