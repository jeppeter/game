
// mfcmesDlg.h : header file
//

#pragma once


// CmfcmesDlg dialog
class CmfcmesDlg : public CDialogEx
{
// Construction
public:
	CmfcmesDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCMES_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnExitDialog();
	afx_msg void OnMessage();
	DECLARE_MESSAGE_MAP()
};
