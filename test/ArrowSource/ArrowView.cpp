///////////////////////////////////////////////////////////////////////////////
// ArrowView.cpp : implementation of the CArrowView class
//
// Written by Mayank Malik (mayankthecadman@yahoo.com)
// www.mayankmalik.cjb.net
// Copyright (c) 2001.
//
// This code may be freely distributable in any application.  If
// you make any changes to the source, please let me know so that
// I might make a better version of the class.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Arrow.h"

#include "ArrowDoc.h"
#include "ArrowView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <math.h> // for sine / cos/tan functions

/////////////////////////////////////////////////////////////////////////////
// CArrowView

IMPLEMENT_DYNCREATE(CArrowView, CView)

BEGIN_MESSAGE_MAP(CArrowView, CView)
	//{{AFX_MSG_MAP(CArrowView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArrowView construction/destruction

CArrowView::CArrowView()
{
	m_Drag = false;//for checking mouse drag mode - initially false
	MotionFix = 0;
}

CArrowView::~CArrowView()
{
}

BOOL CArrowView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CArrowView drawing

void CArrowView::OnDraw(CDC* pDC)
{
	CArrowDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CArrowView diagnostics

#ifdef _DEBUG
void CArrowView::AssertValid() const
{
	CView::AssertValid();
}

void CArrowView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CArrowDoc* CArrowView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CArrowDoc)));
	return (CArrowDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Functions Used to Draw Arrow
void DrawArrow(CDC *pdc,CPoint m_One, CPoint m_Two)
{
	double slopy , cosy , siny;
	double Par = 10.0;//length of Arrow (>)
	slopy = atan2( (long double)( m_One.y - m_Two.y ),
		(long double)( m_One.x - m_Two.x ) );
	cosy = cos( slopy );
	siny = sin( slopy );

	//draw a line between the 2 endpoint
	pdc->MoveTo( m_One );
	pdc->LineTo( m_Two );
	
	//here is the tough part - actually drawing the arrows
	//a total of 6 lines drawn to make the arrow shape
	pdc->MoveTo( m_One);
	pdc->LineTo( m_One.x + int( - Par * cosy - ( Par / 2.0 * siny ) ),
		m_One.y + int( - Par * siny + ( Par / 2.0 * cosy ) ) );
	pdc->LineTo( m_One.x + int( - Par * cosy + ( Par / 2.0 * siny ) ),
		m_One.y - int( Par / 2.0 * cosy + Par * siny ) );
	pdc->LineTo( m_One );
	/*/-------------similarly the the other end-------------/*/
	pdc->MoveTo( m_Two );
	pdc->LineTo( m_Two.x + int( Par * cosy - ( Par / 2.0 * siny ) ),
		m_Two.y + int( Par * siny + ( Par / 2.0 * cosy ) ) );
	pdc->LineTo( m_Two.x + int( Par * cosy + Par / 2.0 * siny ),
		m_Two.y - int( Par / 2.0 * cosy - Par * siny ) );
	pdc->LineTo( m_Two );
	
}

void CArrowView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_Drag = true;// for mouse drag check
	PointOrigin = point;// value when mouse drag starts
	CView::OnLButtonDown(nFlags, point);
}

void CArrowView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_Drag = false;// for mouse drag check
	MotionFix=0;
	CView::OnLButtonUp(nFlags, point);
}

void CArrowView::OnMouseMove(UINT nFlags, CPoint point) 
{
	
	// only draw arrow if drag mode and different points
	if (m_Drag && PointOrigin!=point )
	{
		CClientDC ClientDC (this);
		ClientDC.SetROP2(R2_NOT);

		if (MotionFix) DrawArrow(&ClientDC,PointOrigin,PointOld);
		MotionFix++;	
//MotionFix is used to prevent redrawing in case it is the First Element 

		DrawArrow(&ClientDC,PointOrigin,point);
		
	}
	PointOld = point;
	CView::OnMouseMove(nFlags, point);
}
