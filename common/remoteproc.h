#ifndef __REMOTE_PROC_H__
#define __REMOTE_PROC_H__


#ifdef __cplusplus
extern "C"{
#endif

int ProcRead(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen);
int ProcWrite(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen,int force);
int ProcKill(unsigned int processid,int force);
#ifdef __cplusplus
}
#endif

#endif /*__REMOTE_PROC_H__*/
