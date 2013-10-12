#include <windows.h>
#include "pcmplayer.h"

extern "C" {
#include "libavutil/samplefmt.h"
#include "libavutil/channel_layout.h"
};

CPcmPlayer::CPcmPlayer()
    : m_bStop(TRUE), m_iChLo(0), m_hWo(NULL), \
    m_iSrcChannel(0), m_iSrcChannelLayout(0), m_iSrcSampleFmt(0), m_iSrcSampleRate(0), \
    m_pSc(NULL), m_pFb(NULL)
{
    InitializeCriticalSection(&m_tCs);
    InitializeCriticalSection(&m_tAcs);
}

CPcmPlayer::~CPcmPlayer()
{
    if (!m_bStop)
        Stop();
    
    DeleteCriticalSection(&m_tAcs);
    DeleteCriticalSection(&m_tCs);
}

BOOL CPcmPlayer::Start( int iChannel, int iChannelLayout, int iSampleFmt, int iSampleRate, int iBufNum, int iBufSize )
{
    LPWAVEHDR lpWh = NULL;

    if (iChannel < 1 || iChannel > 7 || av_get_bytes_per_sample((AVSampleFormat)iSampleFmt) < 8 || av_get_bytes_per_sample((AVSampleFormat)iSampleFmt) > 32 || \
        iSampleRate < 0 || iBufNum < 0 || iBufSize < AUDIO_MIN_SIZE || iBufSize % (iChannel * av_get_bytes_per_sample((AVSampleFormat)iSampleFmt)) != 0)
        return FALSE;

    EnterCriticalSection(&m_tCs);

    // 检查状态
    if (!m_bStop)
    {
        LeaveCriticalSection(&m_tCs);
        return TRUE;
    }

    m_pFb = av_fifo_alloc(iBufSize * iBufSize);
    if (m_pFb == NULL)
        goto fail;

    // 初始化声音格式结构
    m_tWfx.wFormatTag = WAVE_FORMAT_PCM;
    m_tWfx.nChannels = (WORD)iChannel;
    m_tWfx.wBitsPerSample = (WORD)av_get_bytes_per_sample((AVSampleFormat)iSampleFmt);
    m_tWfx.nSamplesPerSec = (DWORD)iSampleRate;
    m_tWfx.nBlockAlign = (WORD)(m_tWfx.nChannels * m_tWfx.wBitsPerSample * 0.125);
    m_tWfx.nAvgBytesPerSec = m_tWfx.nSamplesPerSec * m_tWfx.nBlockAlign;
    m_tWfx.cbSize = 0;
    m_iSmpFmt = iSampleFmt;
    m_iChLo = iChannelLayout;

    // 打开声音设备
    if (waveOutOpen(&m_hWo, WAVE_MAPPER, &m_tWfx, (DWORD_PTR)CPcmPlayer::PcmPlayProc, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
        goto fail;

    // 初始化声音缓存
    for (int i = 0; i < iBufNum; i++)
    {
        lpWh = (LPWAVEHDR)malloc(sizeof(WAVEHDR));
        if (lpWh == NULL)
            goto fail;
        memset(lpWh, 0, sizeof(WAVEHDR));
        m_vWh.push_back(lpWh);
        lpWh->lpData = (LPSTR)malloc(iBufSize);
        if (lpWh->lpData == NULL)
            goto fail;
        memset(lpWh->lpData, 0, iBufSize);
        lpWh->dwBufferLength = iBufSize;
        lpWh->dwFlags = 0;
        lpWh->dwLoops = 0;
        lpWh->dwUser = iBufSize;
        if (waveOutPrepareHeader(m_hWo, lpWh, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
            goto fail;
    }
    if (m_vWh.size() == 0)
        goto fail;

    // 设置标志位
    m_bStop = FALSE;

    // 写入音频数据
    for(int i = 0; i < (int)m_vWh.size(); i++)
        waveOutWrite(m_hWo, m_vWh[i], sizeof(WAVEHDR));

    LeaveCriticalSection(&m_tCs);

    return TRUE;

fail:
    m_bStop = TRUE;
    if (m_hWo != NULL)
    {
        EnterCriticalSection(&m_tAcs);
        waveOutReset(m_hWo);
        LeaveCriticalSection(&m_tAcs);
        for (int i = 0; i < (int)m_vWh.size(); i++)
        {
            lpWh = m_vWh[i];
            waveOutUnprepareHeader(m_hWo, lpWh, sizeof(WAVEHDR));
            if (lpWh->lpData != NULL)
                free(lpWh->lpData);
            lpWh->lpData = NULL;
            free(lpWh);
        }
        waveOutClose(m_hWo);
        m_hWo = NULL;
    }
    if (m_pFb != NULL)
    {
        av_fifo_free(m_pFb);
        m_pFb = NULL;
        m_iSrcChannel = 0;
        m_iSrcChannelLayout = 0;
        m_iSrcSampleFmt = 0;
        m_iSrcSampleRate = 0;
    }

    LeaveCriticalSection(&m_tCs);

    return FALSE;
}

VOID CPcmPlayer::Stop()
{
    WAVEHDR * lpWh = NULL;

    EnterCriticalSection(&m_tCs);

    m_bStop = TRUE;
    if (m_hWo != NULL)
    {
        EnterCriticalSection(&m_tAcs);
        waveOutReset(m_hWo);
        LeaveCriticalSection(&m_tAcs);
        for (int i = 0; i < (int)m_vWh.size(); i++)
        {
            lpWh = m_vWh[i];
            waveOutUnprepareHeader(m_hWo, lpWh, sizeof(WAVEHDR));
            if (lpWh->lpData != NULL)
                free(lpWh->lpData);
            lpWh->lpData = NULL;
            free(lpWh);
        }
        waveOutClose(m_hWo);
        m_hWo = NULL;
    }
    if (m_pFb != NULL)
    {
        av_fifo_free(m_pFb);
        m_pFb = NULL;
    }
    if (m_pSc != NULL)
    {
        swr_free(&m_pSc);
        m_pSc = NULL;
    }

    LeaveCriticalSection(&m_tCs);
}

BOOL CPcmPlayer::PutData( char * pData, int iLen, int iChannel, int iChannelLayout, int iSampleFmt, int iSampleRate )
{
    BOOL bRet = FALSE;

    if (m_bStop || pData == NULL || iLen <= 0 || iLen % (iChannel * av_get_bytes_per_sample((AVSampleFormat)iSampleFmt)) != 0 || \
        iChannel < 1 || iChannel > 7 || av_get_bytes_per_sample((AVSampleFormat)iSampleFmt) < 8 || av_get_bytes_per_sample((AVSampleFormat)iSampleFmt) > 32 || iSampleRate <= 0)
        return FALSE;
    
    EnterCriticalSection(&m_tCs);

    if (m_bStop)
    {
        LeaveCriticalSection(&m_tCs);
        return FALSE;
    }

    if (m_pSc == NULL || m_iSrcChannel != iChannel || m_iSrcChannelLayout != iChannelLayout || m_iSrcSampleFmt != iSampleFmt || m_iSrcSampleRate != iSampleRate)
    {
        m_pSc = swr_alloc_set_opts(m_pSc, m_iChLo, (AVSampleFormat)m_iSmpFmt, m_tWfx.nSamplesPerSec, iChannelLayout, (AVSampleFormat)iSampleFmt, iSampleRate, 0, NULL);
        if (m_pSc == NULL)
        {
            LeaveCriticalSection(&m_tCs);
            return FALSE;
        }
        m_iSrcChannel = iChannel;
        m_iSrcChannelLayout = iChannelLayout;
        m_iSrcSampleFmt = iSampleFmt;
        m_iSrcSampleRate = iSampleRate;
    }

    int iOut = iLen / (m_iSrcChannel * av_get_bytes_per_sample((AVSampleFormat)m_iSrcSampleFmt) / 8);
    int iOutLen = iOut * m_tWfx.nChannels * m_tWfx.wBitsPerSample / 8;
    uint8_t * pOutData = (uint8_t *)malloc(iOutLen);
    if (pOutData == NULL)
    {
        free(pOutData);
        LeaveCriticalSection(&m_tCs);
        return FALSE;
    }
    if (swr_convert(m_pSc, &pOutData, iOutLen, (const uint8_t **)&pData, iLen) < 0)
    {
        free(pOutData);
        LeaveCriticalSection(&m_tCs);
        return FALSE;
    }
    bRet = av_fifo_generic_write(m_pFb, pOutData, iOutLen, NULL) > 0 ? TRUE : FALSE;

    LeaveCriticalSection(&m_tCs);

    return bRet;
}

void CALLBACK CPcmPlayer::PcmPlayProc( HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2 )
{
    CPcmPlayer * pPp = (CPcmPlayer *)dwInstance;
    if (pPp == NULL)
        return;
    
    pPp->PcmPlay(hwo, uMsg, dwParam1, dwParam2);
}

void CPcmPlayer::PcmPlay( HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2 )
{
    if (m_bStop || uMsg == WOM_OPEN || uMsg == WOM_CLOSE)
        return;

    WAVEHDR * lpWh = (WAVEHDR *)dwParam1;

    EnterCriticalSection(&m_tAcs);
    if (m_bStop || waveOutUnprepareHeader(hwo, lpWh, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
    {
        LeaveCriticalSection(&m_tAcs);
        return;
    }
    LeaveCriticalSection(&m_tAcs);

    if (av_fifo_size(m_pFb) < 0)
    {
        memset(lpWh->lpData, 0, AUDIO_MIN_SIZE);
        lpWh->dwBufferLength = AUDIO_MIN_SIZE;
    }
    else
    {
        EnterCriticalSection(&m_tCs);
        if (m_bStop)
        {
            LeaveCriticalSection(&m_tCs);
            return;
        }
        lpWh->dwBufferLength = (DWORD)av_fifo_size(m_pFb) < lpWh->dwUser ? av_fifo_size(m_pFb) : lpWh->dwUser;
        if (av_fifo_generic_read(m_pFb, lpWh->lpData, lpWh->dwBufferLength, NULL) < 0)
        {
            LeaveCriticalSection(&m_tCs);
            return;
        }
        LeaveCriticalSection(&m_tCs);
    }
    lpWh->dwFlags = 0;
    lpWh->dwLoops = 0;

    if (m_bStop)
        return;
    if (waveOutPrepareHeader(hwo, lpWh, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
        return;

    waveOutWrite(hwo, lpWh, sizeof(WAVEHDR));
}

