// ArrowDoc.cpp : implementation of the CArrowDoc class
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArrowDoc

IMPLEMENT_DYNCREATE(CArrowDoc, CDocument)

BEGIN_MESSAGE_MAP(CArrowDoc, CDocument)
	//{{AFX_MSG_MAP(CArrowDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArrowDoc construction/destruction

CArrowDoc::CArrowDoc()
{
	// TODO: add one-time construction code here

}

CArrowDoc::~CArrowDoc()
{
}

BOOL CArrowDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CArrowDoc serialization

void CArrowDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CArrowDoc diagnostics

#ifdef _DEBUG
void CArrowDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CArrowDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArrowDoc commands
