#pragma once

#include "ModelessDialogTracker.h"
#include "TrackDialogC.h"

class CModelessDialogTrackerDemoDlg : public CDialog
{
public:
	//////////////////////////////////////////////////////////////
	ModelessDialogTracker trackA, trackB;	// standard trackers for the dialogs
	//// use a derived class to get the closed event for dialogC
	TrackDialogC trackC;
	void OnDlgCClosed();
	///////////////////////////////////////////////////


	// generic wizard stuff follows.....

	CModelessDialogTrackerDemoDlg(CWnd* pParent = NULL);	// standard constructor
	enum { IDD = IDD_MODELESSDIALOGTRACKERDEMO_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPopA();
	afx_msg void OnPopB();
	afx_msg void OnPopC();
	afx_msg void OnCloseABC();
	afx_msg void OnNotifyDialogB();
	CStatic ctrlStatusDlgC;
};
