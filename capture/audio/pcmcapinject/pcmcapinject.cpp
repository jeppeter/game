// pcmcapinject.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "pcmcapinject.h"


// ���ǵ���������һ��ʾ��
PCMCAPINJECT_API int npcmcapinject=0;

// ���ǵ���������һ��ʾ����
PCMCAPINJECT_API int fnpcmcapinject(void)
{
	return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� pcmcapinject.h
Cpcmcapinject::Cpcmcapinject()
{
	return;
}
