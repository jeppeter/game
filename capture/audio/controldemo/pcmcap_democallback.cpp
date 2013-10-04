
#include "pcmcap_democallback.h"

CPcmCapDemoCallBack::CPcmCapDemoCallBack()
{
    m_fp = NULL;
    m_WriteBlockSize = 0;
}

void CPcmCapDemoCallBack::CloseFile()
{
    if(m_fp)
    {
        fclose(m_fp);
    }
    m_fp = NULL;
    m_WriteBlockSize = 0;
    return;
}

CPcmCapDemoCallBack::~CPcmCapDemoCallBack()
{
    this->CloseFile();
}


int CPcmCapDemoCallBack::OpenFile(const char * fname)
{
    int ret;
    this->CloseFile();
    this->m_fp = fopen(fname,"w");
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

    return ;
}

