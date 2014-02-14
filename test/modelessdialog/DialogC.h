#pragma once

#include "ModelessDialogTracker.h"

class DialogC : public CDialog, ModelessDialogHelper
{
	DECLARE_DYNAMIC(DialogC)

public:
	///////////////////////////////////////////
	// stuff specific to the modeless dialog.
	DialogC(ModelessDialogTracker& tracker);   
	BOOL Create(UINT nID, CWnd* pWnd)           	
	{ return CDialog::Create(nID,pWnd);	}
	void PostNcDestroy()
	{ delete this; }
	void OnCancel()								// make sure dialog is only closed with DestroyWindow.. not CDialog::OnOK or CDialog::OnCancel
	{ DestroyWindow(); }
	///////////////////////////////////////////
	virtual ~DialogC();

// Dialog Data
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
