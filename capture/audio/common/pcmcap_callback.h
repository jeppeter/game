
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
    virtual VOID WaveOpenCb(LPVOID lpParam) = 0;				        // ��������ʱ���ã��״ζ�ĳ����Inject������
    virtual VOID WaveInCb(PCMCAP_AUDIO_BUFFER_t* pPcmItem, LPVOID lpParam) = 0;	    // ���������ݻش�ʱ����
    virtual VOID WaveCloseCb(LPVOID lpParam) = 0;				        // ��Inject���̹رջ�Inject���̱��ر�����ʱ����
};


#endif /*__PCMCAP_CALLBACK_H__*/

