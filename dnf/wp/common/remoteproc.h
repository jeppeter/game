#ifdef __REMOTE_PROC2_H__
#define __REMOTE_PROC2_H__


#ifdef __cplusplus
extern "C"{
#endif

int ProcRead2(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen);
int ProcWrite(unsigned int processid,void* pRemoteAddr,unsigned char* pData,int datalen,int force);
#ifdef __cplusplus
}
#endif

#endif /*__REMOTE_PROC2_H__*/
