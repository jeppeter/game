
#ifndef  __PCMCAP_DEMO_CALLBACK_H__
#define  __PCMCAP_DEMO_CALLBACK_H__

#include <pcmcap_callback.h>


class CPcmCapDemoCallBack : public IPcmCapperCallback
{
public:
	CPcmCapDemoCallBack();
	virtual ~CPcmCapDemoCallBack();
	virtual VOID WaveOpenCb(LPVOID lpParam);
	virtual VOID WaveInCb(PCMCAP_AUDIO_BUFFER_t* pPcmItem, LPVOID lpParam);
	virtual VOID WaveCloseCb(LPVOID lpParam);
	int OpenFile(const char* fname);
	void CloseFile();

private:
	FILE* m_fp;
	
};


#endif /*__PCMCAP_DEMO_CALLBACK_H__*/


