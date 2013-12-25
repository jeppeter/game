#include "MyHyperLink.h"


// ControlContainerDlg.h : header file
//

#if !defined(AFX_CONTROLCONTAINERDLG_H__66566118_2BFB_40F5_8C3A_C1234F248CB6__INCLUDED_)
#define AFX_CONTROLCONTAINERDLG_H__66566118_2BFB_40F5_8C3A_C1234F248CB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CControlContainerDlg dialog

class CControlContainerDlg : public CDialog
{
// Construction
public:
	LRESULT OnChildFire(WPARAM wparam,LPARAM lparam);
	CControlContainerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CControlContainerDlg)
	enum { IDD = IDD_CONTROLCONTAINER_DIALOG };

	// Here we Create the Members of CMyHyperLink
	//steps: Forst Create the Control variables of tye CStatic
	//Via the Classwizard, After theat Change  the CStatic  With
	//CMyHyperLink as shown below..
	//remember to include #include "MyHyperLink.h" on the top of this file
	//
	CMyHyperLink	m_Static3;
	CMyHyperLink	m_Static2;
	CMyHyperLink	m_Static1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlContainerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CControlContainerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLCONTAINERDLG_H__66566118_2BFB_40F5_8C3A_C1234F248CB6__INCLUDED_)
