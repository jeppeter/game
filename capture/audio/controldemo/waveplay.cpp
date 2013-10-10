#include "stdafx.h"
#include "waveplay.h"
#pragma comment(lib,"Winmm")

CViWavePlay::CViWavePlay()
{
	ZeroMemory(&m_Waveformat,sizeof(WAVEFORMATEX)); 
	memset(&m_waveCaps,0,sizeof(m_waveCaps));
	m_bDevOpen = FALSE;
	m_bThread = FALSE;
	m_hWave = 0;
	m_hThread = 0;
	m_ThreadID = 0;
	m_BufferQueue = 0;
	InitializeCriticalSection(&m_Lock);
}

CViWavePlay::~CViWavePlay()
{
	Stop();
	DeleteCriticalSection(&m_Lock);
}

UINT CViWavePlay::GetDeviceNum()
{
	return waveOutGetNumDevs();
}

WAVEOUTCAPS* CViWavePlay::GetDeviceCap()
{
	MMRESULT mRet = waveOutGetDevCaps(WAVE_MAPPER,&m_waveCaps,sizeof(m_waveCaps));
	if( mRet == MMSYSERR_NOERROR )
		return &m_waveCaps;
	return NULL;
}
// ���Ͳ������÷��� longf120823
// 	_wfx.nSamplesPerSec  = 44100;  /* sample rate */
// 	_wfx.wBitsPerSample  = 16;     /* sample size */
// 	_wfx.nChannels       = 2;      /* channels    */
// 	_wfx.cbSize          = 0;      /* size of _extra_ info */
// 	_wfx.wFormatTag      = WAVE_FORMAT_PCM;
// 	_wfx.nBlockAlign     = (_wfx.wBitsPerSample * _wfx.nChannels) >> 3;
// 	_wfx.nAvgBytesPerSec = _wfx.nBlockAlign * _wfx.nSamplesPerSec;
BOOL CViWavePlay::Open(PWAVEFORMATEX pWaveformat)
{
	if( m_bDevOpen )
	{
		return FALSE;
	}
	memcpy(&m_Waveformat,pWaveformat,sizeof(WAVEFORMATEX));
	m_Waveformat.nBlockAlign     = (m_Waveformat.wBitsPerSample * m_Waveformat.nChannels) >> 3;
	m_Waveformat.nAvgBytesPerSec = m_Waveformat.nBlockAlign * m_Waveformat.nSamplesPerSec;

	MMRESULT mRet;
	//WAVEFORMATEX wfx;

	//lphWaveOut: PHWaveOut;   {���ڷ����豸�����ָ��; ��� dwFlags=WAVE_FORMAT_QUERY, ����Ӧ�� nil}
	//uDeviceID: UINT;         {�豸ID; ����ָ��Ϊ: WAVE_MAPPER, ������������ݸ����Ĳ��θ�ʽѡ����ʵ��豸}
	//lpFormat: PWaveFormatEx; {TWaveFormat �ṹ��ָ��; TWaveFormat ����Ҫ����Ĳ��θ�ʽ}
	//dwCallback: DWORD        {�ص�������ַ�򴰿ھ��; ����ʹ�ûص�����, ��Ϊ nil}
	//dwInstance: DWORD        {���ص�������ʵ������; �����ڴ���}
	//dwFlags: DWORD           {��ѡ��}// long120823
	mRet = waveOutOpen(0,WAVE_MAPPER,&m_Waveformat,0,0,WAVE_FORMAT_QUERY);
	if( mRet != MMSYSERR_NOERROR )
	{
		return FALSE;
	}

	mRet = waveOutOpen(&m_hWave,WAVE_MAPPER,&m_Waveformat,m_ThreadID,0,CALLBACK_THREAD);
	if( mRet != MMSYSERR_NOERROR )
	{
		return FALSE;
	}

	m_bDevOpen = TRUE;

	return TRUE;
}

void CViWavePlay::Close()
{
	if (!m_bDevOpen)
	{
		return;
	}

	if(!m_hWave)
	{
		return;
	}

	MMRESULT mRet = waveOutClose(m_hWave);
	if( mRet != MMSYSERR_NOERROR )
	{
		return;
	}
	m_hWave = 0;
	m_bDevOpen = FALSE;
}

DWORD WINAPI CViWavePlay::ThreadProc(LPVOID lpParameter)
{
	CViWavePlay *pWaveOut;
	pWaveOut = (CViWavePlay *)lpParameter;

	MSG msg;
	while(GetMessage(&msg,0,0,0))
	{
		switch(msg.message )
		{
		case WOM_OPEN:
			break;
		case WOM_CLOSE:
			break;
		case WOM_DONE:
			WAVEHDR* pWaveHead = (WAVEHDR*)msg.lParam;
			waveOutUnprepareHeader((HWAVEOUT)msg.wParam,pWaveHead,sizeof(WAVEHDR));
			pWaveOut->SubBuffer();
			delete []pWaveHead->lpData;
			delete pWaveHead;
			break;
		}
	}
	return msg.wParam;
}

BOOL CViWavePlay::StartThread()
{
	if( m_bThread )
	{
		return FALSE;
	}

	m_hThread = CreateThread(0,0,ThreadProc,this,0,&m_ThreadID);

	if( !m_hThread )
	{
		return FALSE;
	}
	m_bThread = TRUE;

	return TRUE;
}

void CViWavePlay::StopThread()
{
	if (!m_bThread)
	{
		return;
	}

	if(m_hThread)
	{
		int t=50;
		DWORD ExitCode;
		BOOL bEnd=FALSE;
		PostThreadMessage(m_ThreadID,WM_QUIT,0,0);
		while(t)
		{
			GetExitCodeThread(m_hThread,&ExitCode);
			if(ExitCode!= STILL_ACTIVE)
			{
				bEnd=TRUE;
				break;
			}
			else
				Sleep(10);
			t--;
		}
		if(!bEnd)
		{
			TerminateThread(m_hThread,0);
		}
		m_hThread = 0;
	}
	m_bThread = FALSE;
}

BOOL CViWavePlay::Start(PWAVEFORMATEX pWaveformat)
{
	if (NULL==pWaveformat)
	{
		return FALSE;
	}
	if( !StartThread())
	{
		return FALSE;
	}
	if( !Open(pWaveformat))
	{
		StopThread();
		return FALSE;
	}
	return TRUE;
}

BOOL CViWavePlay::PlayAudio(char* buf,unsigned int  nSize)
{
	if( !m_bDevOpen )
	{
		return FALSE;
	}

	if( GetBufferNum() >= 5 )//������������������������ 
	{
		return FALSE;
	}
	MMRESULT mRet;
	char*	lpData = NULL;
	WAVEHDR* pWaveHead = new WAVEHDR;

	ZeroMemory(pWaveHead,sizeof(WAVEHDR));

	lpData = new char[nSize];

	pWaveHead->dwBufferLength = nSize;
	memcpy(lpData,buf,nSize);
	pWaveHead->lpData = lpData;

	mRet = waveOutPrepareHeader(m_hWave,pWaveHead,sizeof(WAVEHDR));
  	if( mRet != MMSYSERR_NOERROR )
	{
		return FALSE;
	}

	mRet = waveOutWrite(m_hWave,pWaveHead,sizeof(WAVEHDR));
  	if( mRet != MMSYSERR_NOERROR )
	{
		return FALSE;
	}

	AddBuffer();
	return TRUE;
}

void CViWavePlay::Stop()
{
	Close();
	StopThread();
}


int CViWavePlay::GetBufferNum()
{
	int nRet = 5;
	EnterCriticalSection(&m_Lock);
	nRet = m_BufferQueue;
	LeaveCriticalSection(&m_Lock);
	return nRet;
}

void CViWavePlay::AddBuffer()
{
	EnterCriticalSection(&m_Lock);
	m_BufferQueue++;
	LeaveCriticalSection(&m_Lock);
}

void CViWavePlay::SubBuffer()
{
	EnterCriticalSection(&m_Lock);
	m_BufferQueue--;
	LeaveCriticalSection(&m_Lock);
}
