#if !defined(AFX_SCENEVIEW_H__7DCE0EB9_8623_4753_8B10_B40A53710FD2__INCLUDED_)
#define AFX_SCENEVIEW_H__7DCE0EB9_8623_4753_8B10_B40A53710FD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SceneView.h : header file
//
#include "SnapCursor.h"  // MUST ADD FOR SNAPCURSOR
/////////////////////////////////////////////////////////////////////////////
// CSceneView view
class CSceneView : public CView
{
protected:
	CSceneView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSceneView)

// Attributes
public:
    COLORREF m_bgColor;
    double m_X_size;
    double m_Y_size;
    BOOL m_LButtonIsDown;
    BOOL m_MouseHasMoved;    // Button is depressed, but may not have moved
    CSnapCursor m_SnapCursor; // MUST ADD FOR SNAPCURSOR
// Operations
public:
	void SetViewSnapIncrement(int nIncrement);
	CPoint m_OldPoint;
	CPoint m_OriginPoint;
	double Circle2d(CDC* pDC, int Xc, int Yc, int Xr, int Yr);
	void DrawGrid(CDC *pDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSceneView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSceneView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CSceneView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENEVIEW_H__7DCE0EB9_8623_4753_8B10_B40A53710FD2__INCLUDED_)
