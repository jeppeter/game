// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� PCMCAPINJECT_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// PCMCAPINJECT_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

#ifndef __PCMCAP_INJECT_H__
#define __PCMCAP_INJECT_H__

#include <pcmcap_common.h>
#ifdef PCMCAPINJECT_EXPORTS
#define PCMCAPINJECT_API __declspec(dllexport)
#else
#define PCMCAPINJECT_API __declspec(dllimport)
#endif

extern "C" PCMCAPINJECT_API int HandleAudioOperation(PCMCAP_CONTROL_t* pControl);
extern "C" int PcmCapInjectInit(HMODULE hModule);
extern "C" void PcmCapInjectFini(void);

#endif /*__PCMCAP_INJECT_H__*/
