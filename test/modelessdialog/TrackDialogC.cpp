#include "StdAfx.h"
#include "trackdialogc.h"
#include "resource.h"
#include "ModelessDialogTrackerDemoDlg.h"

TrackDialogC::TrackDialogC(CModelessDialogTrackerDemoDlg& d)
:
mainDlg(d)
{
}

TrackDialogC::~TrackDialogC(void)
{
}

void TrackDialogC::OnDialogClosed()
{
	mainDlg.OnDlgCClosed();
}
