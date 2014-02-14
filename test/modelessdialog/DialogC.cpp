// DialogC.cpp : implementation file
//

#include "stdafx.h"
#include "ModelessDialogTrackerDemo.h"
#include "DialogC.h"


// DialogC dialog

IMPLEMENT_DYNAMIC(DialogC, CDialog)
DialogC::DialogC(ModelessDialogTracker& tracker)
: ModelessDialogHelper(tracker, *this)
{
}

DialogC::~DialogC()
{
}

void DialogC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DialogC, CDialog)
END_MESSAGE_MAP()


// DialogC message handlers
