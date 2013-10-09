
#include "pcmcap_democallback.h"
#include <Windows.h>
#include <output_debug.h>
#include <assert.h>

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))

CPcmCapDemoCallBack::CPcmCapDemoCallBack()
{
    m_fp = NULL;
    m_WriteBlockSize = 0;
    m_pPlay = NULL;
    memset(&m_Format,0,sizeof(m_Format));
}

void CPcmCapDemoCallBack::CloseFile()
{
    if(this->m_fp)
    {
        fclose(this->m_fp);
    }
    this->m_fp = NULL;
    this->m_WriteBlockSize = 0;
    return;
}

void CPcmCapDemoCallBack::__StopPlay()
{
    if(this->m_pPlay)
    {
        this->m_pPlay->Stop();
        delete this->m_pPlay;
    }
    this->m_pPlay = NULL;
    memset(&(this->m_Format),0,sizeof(this->m_Format));
}

CPcmCapDemoCallBack::~CPcmCapDemoCallBack()
{
    this->CloseFile();
    this->__StopPlay();
}

void CPcmCapDemoCallBack::__SetWaveFormatEx(WAVEFORMATEX * pwfx,PCM_AUDIO_FORMAT_t * pFormat)
{
    pwfx->nSamplesPerSec = pFormat->m_SampleRate;
    pwfx->wBitsPerSample = pFormat->m_BitsPerSample;
    pwfx->nChannels = pFormat->m_Channels;
    pwfx->cbSize = 0;
    pwfx->wFormatTag = WAVE_FORMAT_PCM;
    if(pwfx->wBitsPerSample == 32)
    {
        pwfx->wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    }
    pwfx->nBlockAlign 	= (pwfx->wBitsPerSample * pwfx->nChannels) >> 3;
    pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;

    return;
}

int CPcmCapDemoCallBack::__StartPlay(PCM_AUDIO_FORMAT_t * pFormat)
{
    WAVEFORMATEX wfx;
    BOOL bret;
    int ret;
    if(this->m_pPlay == NULL)
    {
        this->m_pPlay = new CViWavePlay();
        memset(&wfx,0,sizeof(wfx));
        this->__SetWaveFormatEx(&wfx,pFormat);
        bret = this->m_pPlay->Start(&wfx);
        if(!bret)
        {
            ret = -LAST_ERROR_CODE();
            DEBUG_INFO("wfx.nSamplesPerSec %d\n",wfx.nSamplesPerSec);
            DEBUG_INFO("wfx.wBitsPerSample %d\n",wfx.wBitsPerSample);
            DEBUG_INFO("wfx.nChannels %d\n",wfx.nChannels);
            DEBUG_INFO("wfx.cbSize %d\n",wfx.cbSize);
            DEBUG_INFO("wfx.wFormatTag %d\n",wfx.wFormatTag);
            DEBUG_INFO("wfx.nBlockAlign %d\n",wfx.nBlockAlign);
            DEBUG_INFO("wfx.nAvgBytesPerSec %d\n",wfx.nAvgBytesPerSec);
            //ERROR_INFO("can not start error(%d)\n",ret);
            this->__StopPlay();
            return ret;
        }
		DEBUG_INFO("wfx.nSamplesPerSec %d\n",wfx.nSamplesPerSec);
		DEBUG_INFO("wfx.wBitsPerSample %d\n",wfx.wBitsPerSample);
		DEBUG_INFO("wfx.nChannels %d\n",wfx.nChannels);
		DEBUG_INFO("wfx.cbSize %d\n",wfx.cbSize);
		DEBUG_INFO("wfx.wFormatTag %d\n",wfx.wFormatTag);
		DEBUG_INFO("wfx.nBlockAlign %d\n",wfx.nBlockAlign);
		DEBUG_INFO("wfx.nAvgBytesPerSec %d\n",wfx.nAvgBytesPerSec);
		DEBUG_BUFFER(&wfx,sizeof(wfx));
		memcpy(&(this->m_Format),pFormat,sizeof(*pFormat));
        return 0;
    }

    /*now to compare whether changed*/
    if(memcmp(&(this->m_Format),pFormat,sizeof(*pFormat))==0)
    {
        return 0;
    }

	DEBUG_INFO("Change Format\n");

    this->__StopPlay();
    assert(this->m_pPlay == NULL);
    this->m_pPlay = new CViWavePlay();
	memset(&wfx,0,sizeof(wfx));	
    this->__SetWaveFormatEx(&wfx,pFormat);
    bret = this->m_pPlay->Start(&wfx);
    if(!bret)
    {
        ret = -LAST_ERROR_CODE();
		DEBUG_INFO("wfx.nSamplesPerSec %d\n",wfx.nSamplesPerSec);
		DEBUG_INFO("wfx.wBitsPerSample %d\n",wfx.wBitsPerSample);
		DEBUG_INFO("wfx.nChannels %d\n",wfx.nChannels);
		DEBUG_INFO("wfx.cbSize %d\n",wfx.cbSize);
		DEBUG_INFO("wfx.wFormatTag %d\n",wfx.wFormatTag);
		DEBUG_INFO("wfx.nBlockAlign %d\n",wfx.nBlockAlign);
		DEBUG_INFO("wfx.nAvgBytesPerSec %d\n",wfx.nAvgBytesPerSec);
        ERROR_INFO("can not start error(%d)\n",ret);
        this->__StopPlay();
        return ret;
    }
	DEBUG_INFO("wfx.nSamplesPerSec %d\n",wfx.nSamplesPerSec);
	DEBUG_INFO("wfx.wBitsPerSample %d\n",wfx.wBitsPerSample);
	DEBUG_INFO("wfx.nChannels %d\n",wfx.nChannels);
	DEBUG_INFO("wfx.cbSize %d\n",wfx.cbSize);
	DEBUG_INFO("wfx.wFormatTag %d\n",wfx.wFormatTag);
	DEBUG_INFO("wfx.nBlockAlign %d\n",wfx.nBlockAlign);
	DEBUG_INFO("wfx.nAvgBytesPerSec %d\n",wfx.nAvgBytesPerSec);
	DEBUG_BUFFER(&wfx,sizeof(wfx));
	memcpy(&(this->m_Format),pFormat,sizeof(*pFormat));
    return 0;
}

int CPcmCapDemoCallBack::__Play(unsigned char * pBuffer,int bytes)
{
    BOOL bret;
    int ret;
    if(this->m_pPlay == NULL)
    {
        ret = ERROR_INVALID_PARAMETER;
        goto fail;
    }

    bret = this->m_pPlay->PlayAudio((char*)pBuffer,bytes);
    if(!bret)
    {
        ret = LAST_ERROR_CODE();
        DEBUG_INFO("could not play %d error(%d)\n",bytes,ret);
        DEBUG_BUFFER(pBuffer,bytes > 16 ? 16 : bytes);
        goto fail;
    }

    return 0;
fail:
    this->__StopPlay();
    SetLastError(ret);
    return -ret;
}


int CPcmCapDemoCallBack::OpenFile(const char * fname)
{
    int ret;
    this->CloseFile();
    fopen_s(&(this->m_fp),fname,"w+b");
    if(this->m_fp == NULL)
    {
        ret = LAST_ERROR_CODE();
        return -ret;
    }
    this->m_WriteBlockSize = 0;

    return 0;
}

VOID CPcmCapDemoCallBack::WaveOpenCb(LPVOID lpParam)
{
    DEBUG_INFO("open sound\n");
    return;
}

VOID CPcmCapDemoCallBack::WaveCloseCb(LPVOID lpParam)
{
    DEBUG_INFO("close sound\n");
    return;
}

VOID CPcmCapDemoCallBack::WaveInCb(PCMCAP_AUDIO_BUFFER_t * pPcmItem,LPVOID lpParam)
{
    int bytes;
    int ret;
    bytes = pPcmItem->m_AudioData.m_DataLen;
	//DEBUG_BUFFER(pPcmItem->m_AudioData.m_Data,bytes > 16 ? 16 : bytes);	
    if(this->m_fp)
    {
        //DEBUG_INFO("wave in bytes %d\n",bytes);
        //DEBUG_BUFFER(pPcmItem->m_AudioData.m_Data,bytes > 16 ? 16:bytes );
        ret = fwrite(pPcmItem->m_AudioData.m_Data,bytes,1,this->m_fp);
        if(ret != 1)
        {
            ret = LAST_ERROR_CODE();
            ERROR_INFO("could not write file error(%d)\n",ret);
        }
        else
        {
            this->m_WriteBlockSize += bytes;
            if(this->m_WriteBlockSize > 0x40000)
            {
                /*we should flush buffer when enough big ,so flush it*/
                fflush(this->m_fp);
                this->m_WriteBlockSize = 0;
            }
        }
    }
#if 0
    ret = this->__StartPlay(&(pPcmItem->m_Format));
    if(ret < 0)
    {
        this->__StopPlay();
        return ;
    }

    ret = this->__Play(pPcmItem->m_AudioData.m_Data,bytes);
    if(ret < 0)
    {
        this->__StopPlay();
        return ;
    }
#endif
    return ;
}

