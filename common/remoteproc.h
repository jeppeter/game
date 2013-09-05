#ifndef __REMOTE_PROC_H__
#define __REMOTE_PROC_H__


#ifdef __cplusplus
extern "C"{
#endif

int ProcRead(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen);
int ProcWrite(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen,int force);
int ProcKill(unsigned int processid,int force);
int ProcEnum(const char* exename,unsigned int**ppPids,int *pSize);
int ProcMemorySize(unsigned int processid,unsigned int *pMemSize);

/******************************************
* input
*          enable == 0 disabled ,enable == 1 for enabled
* return value:
*         1 origin is enabled
*         0 origin is disabled
*         negative error code
******************************************/
int EnableDebugLevel(int enble);

#ifdef __cplusplus
};
#endif

#endif /*__REMOTE_PROC_H__*/
