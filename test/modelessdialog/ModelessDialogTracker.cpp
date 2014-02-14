#include "stdafx.h"
#include "ModelessDialogTracker.h"

ModelessDialogTracker::ModelessDialogTracker()
{
	pDlg = NULL;
}
ModelessDialogTracker::~ModelessDialogTracker()
{
	CloseDialog();
}
CDialog* ModelessDialogTracker::GetDlg() const
{
	return (pDlg);
}
void ModelessDialogTracker::CloseDialog()
{
	if (IsPopped())
	{
		GetDlg()->DestroyWindow();
	}
}
BOOL ModelessDialogTracker::IsPopped() const
{
	return (pDlg!=NULL);
}
BOOL ModelessDialogTracker::IsAlreadyPopped() const
{
	if (IsPopped())
	{
		pDlg->SetActiveWindow();
		return TRUE;
	}
	return FALSE;
}

void ModelessDialogTracker::StartTracking( CDialog& dlg )
{
	pDlg = &dlg;
}
void ModelessDialogTracker::StopTracking()
{
	pDlg = NULL;

	OnDialogClosed();			// allows for overriding and getting notification of a closing.
}



////////////////////////////////////////////////////////////////////////

ModelessDialogHelper::ModelessDialogHelper(ModelessDialogTracker& tracker, CDialog& dlg)
{
	pDlgTracker = &tracker;
	pDlgTracker->StartTracking(dlg);
}

ModelessDialogHelper::~ModelessDialogHelper()
{
	pDlgTracker->StopTracking();
}

///////////////////////////////////////////////////////////////////////////