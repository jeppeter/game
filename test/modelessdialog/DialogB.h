#pragma once

#include "ModelessDialogTracker.h"
// DialogB dialog

class DialogB : public CDialog, ModelessDialogHelper
{
	DECLARE_DYNAMIC(DialogB)

public:
	///////////////////////////////////////////
	// stuff specific to the modeless dialog.
	DialogB(ModelessDialogTracker& tracker);   
	BOOL Create(UINT nID, CWnd* pWnd)           	
	{ return CDialog::Create(nID,pWnd);	}
	void PostNcDestroy()
	{ delete this; }
	void OnCancel()								// make sure dialog is only closed with DestroyWindow.. not CDialog::OnOK or CDialog::OnCancel
	{ DestroyWindow(); }
	///////////////////////////////////////////
	virtual ~DialogB();

	void IncrementCounter(const int amt);

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int value;
};
