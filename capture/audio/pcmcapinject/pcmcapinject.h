// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� PCMCAPINJECT_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// PCMCAPINJECT_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

#ifndef __PCMCAP_INJECT_H__
#define __PCMCAP_INJECT_H__
#ifdef PCMCAPINJECT_EXPORTS
#define PCMCAPINJECT_API __declspec(dllexport)
#else
#define PCMCAPINJECT_API __declspec(dllimport)
#endif

// �����Ǵ� pcmcapinject.dll ������
class PCMCAPINJECT_API Cpcmcapinject {
public:
	Cpcmcapinject(void);
	// TODO: �ڴ�������ķ�����
};

extern PCMCAPINJECT_API int npcmcapinject;

PCMCAPINJECT_API int;

#endif /*__PCMCAP_INJECT_H__*/
