// ArrowView.h : interface of the CArrowView class
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

/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARROWVIEW_H__6DD9DA0D_AFB6_11D5_A717_B30F1B7DD97F__INCLUDED_)
#define AFX_ARROWVIEW_H__6DD9DA0D_AFB6_11D5_A717_B30F1B7DD97F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

void DrawArrow(CDC * pdc,CPoint m_One,CPoint m_Two);//draws Arrows b/w given points
class CArrowView : public CView
{
protected: // create from serialization only
	CArrowView();
	DECLARE_DYNCREATE(CArrowView)

// Attributes
public:
	CArrowDoc* GetDocument();

	CPoint PointOld;// previous draw point
	CPoint PointOrigin;// first point
	bool m_Drag;// checks drag mode
	int MotionFix;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArrowView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CArrowView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CArrowView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ArrowView.cpp
inline CArrowDoc* CArrowView::GetDocument()
   { return (CArrowDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARROWVIEW_H__6DD9DA0D_AFB6_11D5_A717_B30F1B7DD97F__INCLUDED_)
