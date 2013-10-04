
// controldemoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "pcmcap_democallback.h"
#include <pcmcap_capper.h>

// CcontroldemoDlg �Ի���
class CcontroldemoDlg : public CDialogEx
{
// ����
public:
	CcontroldemoDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CcontroldemoDlg();
	CString m_strExec;
	CString m_strParam;
	CString m_strDll;
	CString m_strDump;
	CString m_strBufNum;
	CString m_strBlockSize;

// �Ի�������
	enum { IDD = IDD_CONTROLDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
