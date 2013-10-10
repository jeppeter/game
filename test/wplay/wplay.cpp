// wplay.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "waveplay.h"

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))

#define DEBUG_BUFFER_FMT(buf,len,...) do{DebugBufferFmt(__FILE__,__LINE__,(unsigned char*)(buf),(int)(len),__VA_ARGS__);} while(0)
#define DEBUG_INFO(...) do{fprintf(stdout,__VA_ARGS__);}while(0)

void DebugBufferFmt(const char*file,int lineno,unsigned char* pBuffer,int buflen,const char* fmt,...)
{
    int i;
    fprintf(stdout,"At %s:%d\t",file,lineno);
    if(fmt)
    {
        va_list ap;
        va_start(ap,fmt);
        vfprintf(stdout,fmt,ap);
    }
    for(i=0; i<buflen; i++)
    {
        if((i%16)==0)
        {
            fprintf(stdout,"\n0x%08x:\t",i);
        }
        fprintf(stdout," 0x%02x",pBuffer[i]);
    }
    fprintf(stdout,"\n");
    return;
}

static unsigned char st_FormatEx[] = {0xfe,0xff,0x02,0x00,0x80,0xbb,0x00,0x00,0x00,0xdc,0x05,0x00,0x08,0x00,0x20,0x00,
	0x16,0x00,0x20,0x00,0x03,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x10,0x00,
	0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71
};

int main(int argc, char* argv[])
{
    FILE* fp=NULL;
    CViWavePlay *pPlay=NULL;
	WAVEFORMATEX *pwfx=(WAVEFORMATEX*)st_FormatEx;
    int ret;
    BOOL bret;
    char* pBuffer=NULL;
    int rlen=0;
    int retlen=0;
    int totalread=0;
    int filesize=0;
    char* pCurPtr=NULL;

    if(argc < 2)
    {
        fprintf(stderr,"wplay filetoplay\n");
        ret = -3;
        goto out;
    }

    fopen_s(&fp,argv[1],"r+b");
    if(fp == NULL)
    {
        ret = -(LAST_ERROR_CODE());
        fprintf(stderr,"can not open %s error(%d)\n",argv[1],ret);
        goto out;
    }

    fseek(fp,0,2);
    filesize = ftell(fp);
    rewind(fp);
    pBuffer = (char*)malloc(filesize);
    if(pBuffer == NULL)
    {
        ret = -LAST_ERROR_CODE();
        goto out;
    }

    retlen = fread(pBuffer,filesize,1,fp);
    if(retlen != 1)
    {
        ret = -LAST_ERROR_CODE();
        fprintf(stderr,"can not read %s filesize %d error(%d)\n",
                argv[1],filesize,ret);
        goto out;
    }

    pPlay = new CViWavePlay();

#if 0
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = 32;
    wfx.nChannels  = 2;
    wfx.cbSize = 0;
    wfx.wFormatTag      = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nBlockAlign     = (wfx.wBitsPerSample * wfx.nChannels) >> 3;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

#endif
    rlen = pwfx->nSamplesPerSec * pwfx->wBitsPerSample * pwfx->nChannels / 8 / 10;
	DEBUG_INFO("pwfx->nSamplesPerSec %d\n",pwfx->nSamplesPerSec);
	DEBUG_INFO("pwfx->wBitsPerSample %d\n",pwfx->wBitsPerSample);
	DEBUG_INFO("pwfx->nChannels %d\n",pwfx->nChannels);
	DEBUG_INFO("pwfx->cbSize %d\n",pwfx->cbSize);
	DEBUG_INFO("pwfx->wFormatTag %d\n",pwfx->wFormatTag);
	DEBUG_INFO("pwfx->nBlockAlign %d\n",pwfx->nBlockAlign);
	DEBUG_INFO("pwfx->nAvgBytesPerSec %d\n",pwfx->nAvgBytesPerSec);
	DEBUG_BUFFER_FMT(pwfx,sizeof(st_FormatEx),"wfx sizeof(%d)",sizeof(st_FormatEx));
	SetLastError(0);
    bret = pPlay->Start(pwfx);
    if(!bret)
    {
        ret = -LAST_ERROR_CODE();
        fprintf(stderr,"start error %d\n",ret);
        goto out;
    }

    totalread = 0;
    pCurPtr = pBuffer;
    while(totalread < filesize)
    {
        retlen = rlen;
        if((totalread + retlen)> filesize)
        {
            retlen = filesize - totalread;
        }
        //DEBUG_BUFFER_FMT(pCurPtr,(retlen > 16 ? 16 : retlen),"buffer at %d retlen %d",totalread,retlen);
        bret = pPlay->PlayAudio(pCurPtr,retlen);
        if(!bret)
        {
            ret = -LAST_ERROR_CODE();
            fprintf(stderr,"play audio %d error %d\n",totalread,ret);
            goto out;
        }
        totalread += retlen;
		pCurPtr += retlen;
        Sleep(100);
    }


    ret = 0;

out:
    if(pBuffer)
    {
        free(pBuffer);
    }
    pBuffer = NULL;
    if(pPlay)
    {
        pPlay->Stop();
        delete pPlay;
    }
    pPlay = NULL;
    if(fp)
    {
        fclose(fp);
    }
    fp = NULL;
    return ret;
}

