// DialogB.cpp : implementation file
//

#include "stdafx.h"
#include "ModelessDialogTrackerDemo.h"
#include "DialogB.h"


// DialogB dialog

IMPLEMENT_DYNAMIC(DialogB, CDialog)
DialogB::DialogB(ModelessDialogTracker& tracker)
: ModelessDialogHelper(tracker, *this)
, value(42)
{
}

DialogB::~DialogB()
{
}

void DialogB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, value);
}


BEGIN_MESSAGE_MAP(DialogB, CDialog)
END_MESSAGE_MAP()


// DialogB message handlers

void DialogB::IncrementCounter(const int amt)
{
	UpdateData(true);
	value += amt;
	UpdateData(false);
}
