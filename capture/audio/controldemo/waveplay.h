#ifndef _WAVEPLAY_4FR567H6_H_
#define _WAVEPLAY_4FR567H6_H_

#include <mmsystem.h>
#include <Mmreg.h>

//“Ù∆µ≤•∑≈
class CViWavePlay
{
public:
	CViWavePlay();
	~CViWavePlay();
public:
	BOOL Start(PWAVEFORMATEX pWaveformat);
	BOOL PlayAudio(char* buf,unsigned int nSize);
	void Stop();
public:
	UINT GetDeviceNum();
	WAVEOUTCAPS* GetDeviceCap();

private:
	static DWORD WINAPI ThreadProc(LPVOID lpParameter);
	inline int GetBufferNum();
	inline void AddBuffer();
	inline void SubBuffer();

	BOOL Open(PWAVEFORMATEX pWaveformat);
	void Close();
	BOOL StartThread();
	void StopThread();

private:
	WAVEOUTCAPS	m_waveCaps;
	BOOL		m_bDevOpen;
	BOOL		m_bThread;
	HWAVEOUT	m_hWave;
	HANDLE		m_hThread;
	DWORD		m_ThreadID;

	WAVEFORMATEX m_Waveformat;

	CRITICAL_SECTION	m_Lock;
	int			m_BufferQueue;
};


#endif //_WAVEPLAY_4FR567H6_H_