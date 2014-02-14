#pragma once
#include "modelessdialogtracker.h"
class CModelessDialogTrackerDemoDlg;

class TrackDialogC :
	public ModelessDialogTracker
{
public:
	TrackDialogC(CModelessDialogTrackerDemoDlg& d);
	virtual ~TrackDialogC(void);

	virtual void OnDialogClosed();	// override if you want to do something when closed.
private:
	CModelessDialogTrackerDemoDlg& mainDlg;

};
