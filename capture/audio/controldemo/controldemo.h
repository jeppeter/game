
// controldemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CcontroldemoApp:
// �йش����ʵ�֣������ controldemo.cpp
//

class CcontroldemoApp : public CWinApp
{
public:
	CcontroldemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CcontroldemoApp theApp;