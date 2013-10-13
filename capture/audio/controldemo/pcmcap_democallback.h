
#ifndef  __PCMCAP_DEMO_CALLBACK_H__
#define  __PCMCAP_DEMO_CALLBACK_H__

#include <pcmcap_callback.h>
#include <stdio.h>
#include <stdlib.h>
#include "waveplay.h"
#include <vector>

class CPcmCapDemoCallBack : public IPcmCapperCallback
{
public:
	CPcmCapDemoCallBack();
	~CPcmCapDemoCallBack();
	virtual VOID WaveOpenCb(LPVOID lpParam);
	virtual VOID WaveInCb(PCMCAP_AUDIO_BUFFER_t* pPcmItem, LPVOID lpParam);
	virtual VOID WaveCloseCb(LPVOID lpParam);
	int OpenFile(const char* fname);
	void CloseFile();

private:
	int __StartPcmPlay(PCM_AUDIO_FORMAT_t* pFormat);
	int __PcmPlay(PCM_AUDIO_FORMAT_t*pFormat,unsigned char* pBuffer,int bytes);
	void __StopPcmPlay();
	void __InnerPcmPlay(PCMCAP_AUDIO_BUFFER_t *pPcmItem,LPVOID lpParam);
	void __StopPlay();
	
	int __WriteFile(PCMCAP_AUDIO_BUFFER_t * pPcmItem,LPVOID lpParam);
	

private:
	std::vector<FILE*> m_FpVecs;
	std::vector<void*> m_PointerVecs;
	std::vector<int> m_WriteBlockSizeVecs;
	PCM_AUDIO_FORMAT_t m_Format;
	unsigned char m_FileNameBase[128];
};


#endif /*__PCMCAP_DEMO_CALLBACK_H__*/


