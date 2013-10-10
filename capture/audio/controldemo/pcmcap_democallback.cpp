
#include "pcmcap_democallback.h"
#include <Windows.h>
#include <output_debug.h>
#include <assert.h>

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))

CPcmCapDemoCallBack::CPcmCapDemoCallBack()
{
	assert(m_FpVecs.size() == 0);
	assert(m_PointerVecs.size() == 0);
	assert(m_WriteBlockSizeVecs.size() ==0);
    m_pPlay = NULL;
    memset(&m_Format,0,sizeof(m_Format));
	memset(&m_FileNameBase,0,sizeof(m_FileNameBase));
}

#define  VEC_SIZE_EQUAL() \
do\
{\
	assert(this->m_FpVecs.size() == this->m_PointerVecs.size());\
	assert(this->m_PointerVecs.size() == this->m_WriteBlockSizeVecs.size());\
}while(0)

void CPcmCapDemoCallBack::CloseFile()
{
	VEC_SIZE_EQUAL();

	while(this->m_FpVecs.size() > 0)
	{
		FILE* fp=this->m_FpVecs[0];
		void* pPointer= this->m_PointerVecs[0];
		int wsize= this->m_WriteBlockSizeVecs[0];

		this->m_FpVecs.erase(this->m_FpVecs.begin());
		this->m_PointerVecs.erase(this->m_PointerVecs.begin());
		this->m_WriteBlockSizeVecs.erase(this->m_WriteBlockSizeVecs.begin());
		VEC_SIZE_EQUAL();

		if (fp)
		{
			fclose(fp);
		}
		fp = NULL;
		pPointer = NULL;
		wsize = 0;
	}

	memset(this->m_FileNameBase,0,sizeof(this->m_FileNameBase));
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
    this->CloseFile();
	strncpy_s((char*)this->m_FileNameBase,sizeof(this->m_FileNameBase),fname,_TRUNCATE);

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

int CPcmCapDemoCallBack::__WriteFile(PCMCAP_AUDIO_BUFFER_t * pPcmItem,LPVOID lpParam)
{
	unsigned int i;
	int findidx=-1;
	int removeidx=-1;
	FILE *pOpenFp=NULL,*pWriteFp=NULL;
	int writesize;
	void* pPointer=NULL,*pSearchPointer=NULL;
	int ret=1;
	unsigned char fnametmp[150];
	
	/*now first to make sure file is opened*/
	pSearchPointer = (void*)pPcmItem->m_AudioData.m_Pointer;
	for (i=0;i<this->m_PointerVecs.size();i++)
	{
		if (pSearchPointer == this->m_PointerVecs[i])
		{
			findidx = i;
			break;
		}
	}

	if (findidx >= 0)
	{
		pWriteFp = this->m_FpVecs[findidx];
		writesize = this->m_WriteBlockSizeVecs[findidx];
	}
	else
	{
		_snprintf_s((char*)fnametmp,sizeof(fnametmp),_TRUNCATE,"%s.0x%p",this->m_FileNameBase,pSearchPointer);
		fopen_s(&pOpenFp,(const char*)fnametmp,"w+b");
		if (pOpenFp == NULL)
		{
			ret = LAST_ERROR_CODE();
			ERROR_INFO("could not open %s error(%d)\n",fnametmp,ret);
			goto fail;
		}

		pWriteFp = pOpenFp;
		writesize = 0;
	}

	/*now to write*/
	ret = fwrite(pPcmItem->m_AudioData.m_Data,pPcmItem->m_AudioData.m_DataLen,1,pWriteFp);
	if (ret != 1)
	{
		ret = LAST_ERROR_CODE();
		_snprintf_s((char*)fnametmp,sizeof(fnametmp),_TRUNCATE,"%s.0x%p",this->m_FileNameBase,pSearchPointer);
		ERROR_INFO("can not write %s error(%d)\n",fnametmp,ret);
		goto fail;
	}

	writesize += pPcmItem->m_AudioData.m_DataLen;
	if (writesize >= 0x40000)
	{
		fflush(pWriteFp);
		writesize = 0;
	}
	if (findidx >= 0)
	{
		this->m_WriteBlockSizeVecs[findidx]=writesize;
	}
	else
	{
		this->m_FpVecs.push_back(pOpenFp);
		this->m_WriteBlockSizeVecs.push_back(writesize);
		this->m_PointerVecs.push_back(pSearchPointer);
	}

	return 0;
fail:
	if (findidx >= 0)
	{
		FILE* fp=NULL;
		assert((unsigned int)findidx < this->m_FpVecs.size());
		VEC_SIZE_EQUAL();
		fp = this->m_FpVecs[findidx];
		this->m_FpVecs.erase(this->m_FpVecs.begin() + findidx);
		this->m_PointerVecs.erase(this->m_PointerVecs.begin() + findidx);
		this->m_WriteBlockSizeVecs.erase(this->m_WriteBlockSizeVecs.begin() + findidx);
		if (fp)
		{
			fclose(fp);
		}
		fp = NULL;
		VEC_SIZE_EQUAL();
	}	

	if (pOpenFp)
	{
		fclose(pOpenFp);
	}
	pOpenFp = NULL;
	pWriteFp = NULL;
	SetLastError(ret);
	return -ret;
}

VOID CPcmCapDemoCallBack::WaveInCb(PCMCAP_AUDIO_BUFFER_t * pPcmItem,LPVOID lpParam)
{
	this->__WriteFile(pPcmItem,lpParam);
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

