// SnapCursorDemo.h : main header file for the SNAPCURSORDEMO application
//

#if !defined(AFX_SNAPCURSORDEMO_H__514FEDCB_D238_4971_BF91_25BBB1EC1369__INCLUDED_)
#define AFX_SNAPCURSORDEMO_H__514FEDCB_D238_4971_BF91_25BBB1EC1369__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSnapCursorDemoApp:
// See SnapCursorDemo.cpp for the implementation of this class
//

class CSnapCursorDemoApp : public CWinApp
{
public:
	CSnapCursorDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSnapCursorDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSnapCursorDemoApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNAPCURSORDEMO_H__514FEDCB_D238_4971_BF91_25BBB1EC1369__INCLUDED_)
