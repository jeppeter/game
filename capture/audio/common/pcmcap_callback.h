
#ifndef __PCMCAP_CALLBACK_H__
#define __PCMCAP_CALLBACK_H__

#include <Windows.h>
#include <pcmcap_common.h>

#define PCMITEM_MAX_SIZE 10240


class CPcmCapper;


class IPcmCapperCallback
{
public:
    friend class CPcmCapper;
protected:
    virtual VOID WaveOpenCb(LPVOID lpParam) = 0;				        // 声音被打开时调用，首次对某进程Inject不调用
    virtual VOID WaveInCb(PCMCAP_AUDIO_BUFFER_t* pPcmItem, LPVOID lpParam) = 0;	    // 有声音数据回传时调用
    virtual VOID WaveCloseCb(LPVOID lpParam) = 0;				        // 被Inject进程关闭或被Inject进程被关闭声音时调用
};


#endif /*__PCMCAP_CALLBACK_H__*/

