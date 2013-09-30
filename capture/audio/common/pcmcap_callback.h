
#ifndef __PCMCAP_CALLBACK_H__
#define __PCMCAP_CALLBACK_H__

#define PCMITEM_MAX_SIZE 10240

typedef struct 
{
    int format;				        // ������ʽ
    int channel;			        // ������
    int bitspersample;			    // ÿ�������ĵı�����
    int samplerate;			        // ������
    int volume;				        // ��ǰ��������С
    uint8_t data[PCMITEM_MAX_SIZE];	// ʵ����������
    int len;				        // ʵ���������ݵĳ���
} PCMITEM;

class CPcmCapper;


class IPcmCapperCallback
{
public:
    friend class CPcmCapper;
protected:
    virtual VOID WaveOpenCb(LPVOID lpParam) = 0;				        // ��������ʱ���ã��״ζ�ĳ����Inject������
    virtual VOID WaveInCb(PCMITEM * pPcmItem, LPVOID lpParam) = 0;	    // ���������ݻش�ʱ����
    virtual VOID WaveCloseCb(LPVOID lpParam) = 0;				        // ��Inject���̹رջ�Inject���̱��ر�����ʱ����
};


#endif /*__PCMCAP_CALLBACK_H__*/

