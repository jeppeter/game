
#ifndef __PROC_EX_H__
#define __PROC_EX_H__

#ifdef __cplusplus
extern "C" {
#endif

int GetModuleInsertedProcess(const char* pPartDll,unsigned int **ppPids,int *pPidsSize);

#ifdef __cplusplus
};
#endif

#endif /*__PROC_EX_H__*/

