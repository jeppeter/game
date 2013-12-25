// SnapCursorDemoView.h : interface of the CSnapCursorDemoView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNAPCURSORDEMOVIEW_H__AF2C387E_C51C_4573_918F_945606FDD0E6__INCLUDED_)
#define AFX_SNAPCURSORDEMOVIEW_H__AF2C387E_C51C_4573_918F_945606FDD0E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSnapCursorDemoDoc;
class CSceneView;

class CSnapCursorDemoView : public CFormView
{
protected: // create from serialization only
	CSnapCursorDemoView();
	DECLARE_DYNCREATE(CSnapCursorDemoView)

public:
	//{{AFX_DATA(CSnapCursorDemoView)
	enum { IDD = IDD_SNAPCURSORDEMO_FORM };
	int		m_RadioSnapIncrement;
	//}}AFX_DATA

// Attributes
public:
	CSnapCursorDemoDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSnapCursorDemoView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSnapCursorDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSnapCursorDemoView)
	afx_msg void OnRadioSnapincrement5();
	afx_msg void OnRadioSnapincrement10();
	afx_msg void OnRadioSnapIncrement50();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SnapCursorDemoView.cpp
inline CSnapCursorDemoDoc* CSnapCursorDemoView::GetDocument()
   { return (CSnapCursorDemoDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNAPCURSORDEMOVIEW_H__AF2C387E_C51C_4573_918F_945606FDD0E6__INCLUDED_)
