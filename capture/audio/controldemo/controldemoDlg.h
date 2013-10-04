
// controldemoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

// CcontroldemoDlg �Ի���
class CcontroldemoDlg : public CDialogEx
{
// ����
public:
	CcontroldemoDlg(CWnd* pParent = NULL);	// ��׼���캯��
	CString m_strExec;
	CString m_strParam;
	CString m_strDll;
	CString m_strDump;

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
	void StartCapper();
	void StopCapper();



private:
	CPcmCapDemoCallBack* m_pDemoCallBack;
	CPcmCapper *m_pCapper;
};
