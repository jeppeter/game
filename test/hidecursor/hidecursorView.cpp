
// hidecursorView.cpp : implementation of the ChidecursorView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "hidecursor.h"
#endif

#include "hidecursorDoc.h"
#include "hidecursorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ChidecursorView

IMPLEMENT_DYNCREATE(ChidecursorView, CView)

BEGIN_MESSAGE_MAP(ChidecursorView, CView)
END_MESSAGE_MAP()

// ChidecursorView construction/destruction

ChidecursorView::ChidecursorView()
{
	// TODO: add construction code here

}

ChidecursorView::~ChidecursorView()
{
}

BOOL ChidecursorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// ChidecursorView drawing

void ChidecursorView::OnDraw(CDC* /*pDC*/)
{
	ChidecursorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// ChidecursorView diagnostics

#ifdef _DEBUG
void ChidecursorView::AssertValid() const
{
	CView::AssertValid();
}

void ChidecursorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

ChidecursorDoc* ChidecursorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ChidecursorDoc)));
	return (ChidecursorDoc*)m_pDocument;
}
#endif //_DEBUG


// ChidecursorView message handlers
