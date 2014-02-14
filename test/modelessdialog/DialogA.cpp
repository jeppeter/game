// DialogA.cpp : implementation file
//

#include "stdafx.h"
#include "ModelessDialogTrackerDemo.h"
#include "DialogA.h"


// DialogA dialog

IMPLEMENT_DYNAMIC(DialogA, CDialog)
DialogA::DialogA(ModelessDialogTracker& tracker)
: ModelessDialogHelper(tracker, *this)
{
}

DialogA::~DialogA()
{
}

void DialogA::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DialogA, CDialog)
END_MESSAGE_MAP()


// DialogA message handlers
