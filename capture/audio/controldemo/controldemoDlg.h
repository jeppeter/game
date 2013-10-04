
// controldemoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "pcmcap_democallback.h"
#include <pcmcap_capper.h>

// CcontroldemoDlg 对话框
class CcontroldemoDlg : public CDialogEx
{
// 构造
public:
	CcontroldemoDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CcontroldemoDlg();
	CString m_strExec;
	CString m_strParam;
	CString m_strDll;
	CString m_strDump;
	CString m_strBufNum;
	CString m_strBlockSize;

// 对话框数据
	enum { IDD = IDD_CONTROLDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCheckBoxClick();
	afx_msg void OnBtnExe();
	afx_msg void OnBtnDll();
	afx_msg void OnBtnDump();
	afx_msg void OnBtnStart();
	void StartCapper();
	void StopCapper();



private:
	CPcmCapDemoCallBack* m_pDemoCallBack;
	CPcmCapper *m_pCapper;
	HANDLE m_hProc;
};
