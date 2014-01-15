
// hidecursor.h : main header file for the hidecursor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// ChidecursorApp:
// See hidecursor.cpp for the implementation of this class
//

class ChidecursorApp : public CWinApp
{
public:
	ChidecursorApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern ChidecursorApp theApp;
