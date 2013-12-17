// Arrow.h : main header file for the ARROW application
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


#if !defined(AFX_ARROW_H__6DD9DA05_AFB6_11D5_A717_B30F1B7DD97F__INCLUDED_)
#define AFX_ARROW_H__6DD9DA05_AFB6_11D5_A717_B30F1B7DD97F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CArrowApp:
// See Arrow.cpp for the implementation of this class
//

class CArrowApp : public CWinApp
{
public:
	CArrowApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArrowApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CArrowApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARROW_H__6DD9DA05_AFB6_11D5_A717_B30F1B7DD97F__INCLUDED_)
