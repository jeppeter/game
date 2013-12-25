// SnapCursorDemoDoc.cpp : implementation of the CSnapCursorDemoDoc class
//

#include "stdafx.h"
#include "SnapCursorDemo.h"

#include "SnapCursorDemoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSnapCursorDemoDoc

IMPLEMENT_DYNCREATE(CSnapCursorDemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CSnapCursorDemoDoc, CDocument)
	//{{AFX_MSG_MAP(CSnapCursorDemoDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSnapCursorDemoDoc construction/destruction

CSnapCursorDemoDoc::CSnapCursorDemoDoc()
{
	// TODO: add one-time construction code here

}

CSnapCursorDemoDoc::~CSnapCursorDemoDoc()
{
}

BOOL CSnapCursorDemoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSnapCursorDemoDoc serialization

void CSnapCursorDemoDoc::Serialize(CArchive& ar)
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
// CSnapCursorDemoDoc diagnostics

#ifdef _DEBUG
void CSnapCursorDemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSnapCursorDemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSnapCursorDemoDoc commands
