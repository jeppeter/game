// SnapCursorDemoDoc.h : interface of the CSnapCursorDemoDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNAPCURSORDEMODOC_H__2D25AB81_2F99_4523_927F_A22C29AC2644__INCLUDED_)
#define AFX_SNAPCURSORDEMODOC_H__2D25AB81_2F99_4523_927F_A22C29AC2644__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SceneView.h"

class CSnapCursorDemoDoc : public CDocument
{
protected: // create from serialization only
	CSnapCursorDemoDoc();
	DECLARE_DYNCREATE(CSnapCursorDemoDoc)

// Attributes
public:
   CSceneView *m_pSceneView;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSnapCursorDemoDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSnapCursorDemoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSnapCursorDemoDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNAPCURSORDEMODOC_H__2D25AB81_2F99_4523_927F_A22C29AC2644__INCLUDED_)
