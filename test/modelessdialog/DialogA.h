#pragma once
#include "ModelessDialogTracker.h"

class DialogA : public CDialog, ModelessDialogHelper
{
	DECLARE_DYNAMIC(DialogA)

public:
	///////////////////////////////////////////
	// stuff specific to the modeless dialog.
	DialogA(ModelessDialogTracker& tracker);   
	BOOL Create(UINT nID, CWnd* pWnd)           	
	{ return CDialog::Create(nID,pWnd);	}
	void PostNcDestroy()
	{ delete this; }
	void OnCancel()								// make sure dialog is only closed with DestroyWindow.. not CDialog::OnOK or CDialog::OnCancel
	{ DestroyWindow(); }
	///////////////////////////////////////////
	virtual ~DialogA();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
