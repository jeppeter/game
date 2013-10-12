#ifndef __PCMPLAYER_H__
#define __PCMPLAYER_H__

#include <queue>
extern "C" {
#include "libswresample/swresample.h"
#include "libavutil/fifo.h"
};

#define AUDIO_MIN_SIZE 512

class CPcmPlayer
{
public:
	CPcmPlayer();
	~CPcmPlayer();

	BOOL Start(int iChannel, int iChannelLayout, int iSampleFmt, int iSampleRate, int iBufNum, int iBufSize);
	VOID Stop();
	BOOL PutData(char * pData, int iLen, int iChannel, int iChannelLayout, int iSampleFmt, int iSampleRate);

protected:
    static void CALLBACK PcmPlayProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
    void PcmPlay(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

private:
    BOOL m_bStop;
    int m_iSmpFmt;
    int m_iChLo;
    HWAVEOUT m_hWo;
    WAVEFORMATEX m_tWfx;
    std::vector<LPWAVEHDR> m_vWh; 
    int m_iSrcChannel;
    int m_iSrcChannelLayout;
    int m_iSrcSampleFmt;
    int m_iSrcSampleRate;
    SwrContext * m_pSc;
    AVFifoBuffer * m_pFb;

    CRITICAL_SECTION m_tAcs;
    CRITICAL_SECTION m_tCs;
};

#endif