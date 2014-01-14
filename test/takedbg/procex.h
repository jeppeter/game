
#ifndef __PROC_EX_H__
#define __PROC_EX_H__


#ifdef __cplusplus
extern "C" {
#endif

int GetModuleInsertedProcess(const char* pPartDll,unsigned int **ppPids,int *pPidsSize);

/*0 for success negative for error code*/
int EnableCurrentDebugPriv();
/*0 for success  negative for error code*/
int DisableCurrentDebugPriv();


#ifdef __cplusplus
};
#endif

#endif /*__PROC_EX_H__*/

