// ControlContainer.h : main header file for the CONTROLCONTAINER application
//

#if !defined(AFX_CONTROLCONTAINER_H__B2789718_91D3_463A_B463_5A254FF2021E__INCLUDED_)
#define AFX_CONTROLCONTAINER_H__B2789718_91D3_463A_B463_5A254FF2021E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CControlContainerApp:
// See ControlContainer.cpp for the implementation of this class
//

class CControlContainerApp : public CWinApp
{
public:
	CControlContainerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlContainerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CControlContainerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLCONTAINER_H__B2789718_91D3_463A_B463_5A254FF2021E__INCLUDED_)
