#ifdef __REMOTE_PROC_H__
#define __REMOTE_PROC_H__




extern "C" int ProcRead(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen);
extern "C" int ProcWrite(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen,int force);

#endif /*__REMOTE_PROC_H__*/