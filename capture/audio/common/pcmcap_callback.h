
#ifndef __PCMCAP_CALLBACK_H__
#define __PCMCAP_CALLBACK_H__

#define PCMITEM_MAX_SIZE 10240

typedef struct 
{
    int format;				        // 声音格式
    int channel;			        // 声道数
    int bitspersample;			    // 每个采样的的比特数
    int samplerate;			        // 采样率
    int volume;				        // 当前的声音大小
    uint8_t data[PCMITEM_MAX_SIZE];	// 实际声音数据
    int len;				        // 实际声音数据的长度
} PCMITEM;

class CPcmCapper;


class IPcmCapperCallback
{
public:
    friend class CPcmCapper;
protected:
    virtual VOID WaveOpenCb(LPVOID lpParam) = 0;				        // 声音被打开时调用，首次对某进程Inject不调用
    virtual VOID WaveInCb(PCMITEM * pPcmItem, LPVOID lpParam) = 0;	    // 有声音数据回传时调用
    virtual VOID WaveCloseCb(LPVOID lpParam) = 0;				        // 被Inject进程关闭或被Inject进程被关闭声音时调用
};


#endif /*__PCMCAP_CALLBACK_H__*/

