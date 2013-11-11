// pecheck.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <Imagehlp.h>

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))

#define DEBUG_INFO(...) do{fprintf(stdout,"[%s:%d]\t",__FILE__,__LINE__);fprintf(stdout,__VA_ARGS__);}while(0)
#define ERROR_INFO(...) do{fprintf(stderr,"[%s:%d]\t",__FILE__,__LINE__);fprintf(stderr,__VA_ARGS__);}while(0)


#pragma comment(lib,"Imagehlp.lib")

DWORD CheckFile(char* pFile)
{
    unsigned char* pMapFile=NULL;
    DWORD fsize=0;
    FILE* fp=NULL;
    int ret;
    DWORD crc=0,hdrcrc=0;
    PIMAGE_NT_HEADERS pNtHeader=NULL;

    fopen_s(&fp,pFile,"r+b");
    if(fp == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("Openfile(%s) error(%d)\n",pFile,ret);
        goto fail;
    }

    fseek(fp,0,SEEK_END);
    fsize = ftell(fp);
    fseek(fp,0,SEEK_SET);
    pMapFile = (unsigned char*)malloc(fsize);
    if(pMapFile == NULL)
    {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    ret = fread(pMapFile,fsize,1,fp);
    if(ret != 1)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("can not read file(%s) size %d error(%d)\n",pFile,fsize,ret);
        goto fail;
    }

    pNtHeader = ::CheckSumMappedFile(pMapFile,fsize,&hdrcrc,&crc);
    if(pNtHeader == NULL)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("can not check sum for (%s:%d) error(%d)\n",pFile,fsize,ret);
        goto fail;
    }

    DEBUG_INFO("(%s:%d) header crc %d:0x%08x crc %d:0x%08x\n",pFile,fsize,hdrcrc,hdrcrc,crc,crc);


    if(pMapFile)
    {
        free(pMapFile);
    }
    pMapFile = NULL;
    if(fp)
    {
        fclose(fp);
    }
    fp = NULL;

    SetLastError(0);
    return crc;
fail:
    if(pMapFile)
    {
        free(pMapFile);
    }
    pMapFile = NULL;
    if(fp)
    {
        fclose(fp);
    }
    fp = NULL;
    SetLastError(ret);
    return 0;
}

int main(int argc, char* argv[])
{

    int i;
    if(argc < 2)
    {
        fprintf(stderr,"%s exe...\n",argv[0]);
        exit(3);
    }

    for(i=1; i<argc; i++)
    {
        CheckFile(argv[i]);
    }

    return 0;
}

