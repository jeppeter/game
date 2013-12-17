// ArrowDoc.h : interface of the CArrowDoc class
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

#if !defined(AFX_ARROWDOC_H__6DD9DA0B_AFB6_11D5_A717_B30F1B7DD97F__INCLUDED_)
#define AFX_ARROWDOC_H__6DD9DA0B_AFB6_11D5_A717_B30F1B7DD97F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CArrowDoc : public CDocument
{
protected: // create from serialization only
	CArrowDoc();
	DECLARE_DYNCREATE(CArrowDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArrowDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CArrowDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CArrowDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARROWDOC_H__6DD9DA0B_AFB6_11D5_A717_B30F1B7DD97F__INCLUDED_)
