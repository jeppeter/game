
#ifndef  __MEM_SHARE_H__
#define  __MEM_SHARE_H__

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int ReadShareMem(unsigned char* pBasePtr,int offset,unsigned char* pBuffer,int bufsize);
int WriteShareMem(unsigned char* pBasePtr,int offset,unsigned char* pBuffer,int bufsize);
HANDLE CreateMapFile(const char* pMapFileName,int size,int create);
unsigned char* MapFileBuffer(HANDLE hMapFile,int size);
void UnMapFileBuffer(unsigned char** ppBuffer);
void CloseMapFileHandle(HANDLE *pHandle);



#ifdef __cplusplus
}
#endif



#endif /*__MEM_SHARE_H__*/

