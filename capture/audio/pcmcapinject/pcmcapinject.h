// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 PCMCAPINJECT_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// PCMCAPINJECT_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

#ifndef __PCMCAP_INJECT_H__
#define __PCMCAP_INJECT_H__
#ifdef PCMCAPINJECT_EXPORTS
#define PCMCAPINJECT_API __declspec(dllexport)
#else
#define PCMCAPINJECT_API __declspec(dllimport)
#endif

extern "C" PCMCAPINJECT_API int PcmCapInject_SetAudioOperation(int iOperation);
extern "C" PCMCAPINJECT_API int PcmCapInject_SetAudioState(int iState);
extern "C" int PcmCapInjectInit(void);
extern "C" void PcmCapInjectFini(void);

#endif /*__PCMCAP_INJECT_H__*/
