
// hidecursorView.h : interface of the ChidecursorView class
//

#pragma once


class ChidecursorView : public CView
{
protected: // create from serialization only
	ChidecursorView();
	DECLARE_DYNCREATE(ChidecursorView)

// Attributes
public:
	ChidecursorDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~ChidecursorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in hidecursorView.cpp
inline ChidecursorDoc* ChidecursorView::GetDocument() const
   { return reinterpret_cast<ChidecursorDoc*>(m_pDocument); }
#endif

