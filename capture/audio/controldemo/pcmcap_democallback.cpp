
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

        if(fp)
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
    memset(&(this->m_Format),0,sizeof(this->m_Format));
}

CPcmCapDemoCallBack::~CPcmCapDemoCallBack()
{
    this->CloseFile();
    this->__StopPcmPlay();
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
    for(i=0; i<this->m_PointerVecs.size(); i++)
    {
        if(pSearchPointer == this->m_PointerVecs[i])
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        pWriteFp = this->m_FpVecs[findidx];
        writesize = this->m_WriteBlockSizeVecs[findidx];
    }
    else
    {
        _snprintf_s((char*)fnametmp,sizeof(fnametmp),_TRUNCATE,"%s.0x%p",this->m_FileNameBase,pSearchPointer);
        fopen_s(&pOpenFp,(const char*)fnametmp,"w+b");
        if(pOpenFp == NULL)
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
    if(ret != 1)
    {
        ret = LAST_ERROR_CODE();
        _snprintf_s((char*)fnametmp,sizeof(fnametmp),_TRUNCATE,"%s.0x%p",this->m_FileNameBase,pSearchPointer);
        ERROR_INFO("can not write %s error(%d)\n",fnametmp,ret);
        goto fail;
    }

    writesize += pPcmItem->m_AudioData.m_DataLen;
    if(writesize >= 0x40000)
    {
        fflush(pWriteFp);
        writesize = 0;
    }
    if(findidx >= 0)
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
    if(findidx >= 0)
    {
        FILE* fp=NULL;
        assert((unsigned int)findidx < this->m_FpVecs.size());
        VEC_SIZE_EQUAL();
        fp = this->m_FpVecs[findidx];
        this->m_FpVecs.erase(this->m_FpVecs.begin() + findidx);
        this->m_PointerVecs.erase(this->m_PointerVecs.begin() + findidx);
        this->m_WriteBlockSizeVecs.erase(this->m_WriteBlockSizeVecs.begin() + findidx);
        if(fp)
        {
            fclose(fp);
        }
        fp = NULL;
        VEC_SIZE_EQUAL();
    }

    if(pOpenFp)
    {
        fclose(pOpenFp);
    }
    pOpenFp = NULL;
    pWriteFp = NULL;
    SetLastError(ret);
    return -ret;
}


void CPcmCapDemoCallBack::__StopPcmPlay()
{
	return;
}

int CPcmCapDemoCallBack::__StartPcmPlay(PCM_AUDIO_FORMAT_t * pFormat)
{
	return 1;
}

int CPcmCapDemoCallBack::__PcmPlay(PCM_AUDIO_FORMAT_t * pFormat,unsigned char * pBuffer,int bytes)
{
	int ret;
	WAVEFORMATEX* pFormatEx=NULL;
	int size;

	/*now to change the format */
	pFormatEx = (WAVEFORMATEX*) pFormat->m_Format;
	size = sizeof(*pFormatEx);
	size += pFormatEx->cbSize;
	if (size > sizeof(pFormat->m_Format))
	{
		ret = ERROR_INSUFFICIENT_BUFFER;
		ERROR_INFO("the format size %d > sizeof(%d)\n",size,sizeof(pFormat->m_Format));
		goto fail;
	}

	/*now to test if this is 2 channels*/
	if (pFormatEx->nChannels != 2)
	{
		ret = ERROR_INVALID_PARAMETER;
		ERROR_INFO("channels %d not valid\n",pFormatEx->nChannels);
		goto fail;
	}

	if (pFormatEx->wFormatTag != WAVE_FORMAT_EXTENSIBLE && pFormatEx->wFormatTag != WAVE_FORMAT_PCM)
	{
		ret = ERROR_INVALID_PARAMETER;
		ERROR_INFO("formattag 0x%x not supported\n",pFormatEx->wFormatTag);
		goto fail;
	}


	

	return 0;
fail:
	SetLastError(ret);
	return -ret;
}

void CPcmCapDemoCallBack::__InnerPcmPlay(PCMCAP_AUDIO_BUFFER_t * pPcmItem,LPVOID lpParam)
{
	int ret ;

	ret = this->__StartPcmPlay(&(pPcmItem->m_Format));
	if (ret < 0)
	{
		this->__StopPcmPlay();
		return ;
	}

	ret = this->__PcmPlay(&(pPcmItem->m_Format),pPcmItem->m_AudioData.m_Data,pPcmItem->m_AudioData.m_DataLen);
	if (ret < 0)
	{
		this->__StopPcmPlay();
		return ;
	}

	return ;
}



VOID CPcmCapDemoCallBack::WaveInCb(PCMCAP_AUDIO_BUFFER_t * pPcmItem,LPVOID lpParam)
{
#if 1
    this->__WriteFile(pPcmItem,lpParam);
#else
	this->__InnerPcmPlay(pPcmItem,lpParam);

#endif
    return ;
}

