

#ifndef __WIN_PROC_H__
#define __WIN_PROC_H__

#include <Windows.h>
#include <cap_common.h>

#ifdef __cplusplus
extern "C" {
#endif



/*******************************************************
*  to get the proc windows for it
*  input :
*          hProc  the process handler ,it must get the query_information rights
*                      otherwise will failed
*
*          pphWnds  the output store windows handle stored
*          pSize  the size of pointer *pphWnds
*
*  output :
*          number of phWnds in the phWnds on success ,otherwise negative error code
*          and GetLastError() for the error code
*
*  remark :
*          *pphWnds must be malloc and free used ,not other allocate
*          and if hProc ==NULL ,it will free *pphWnds return 0
*          so make sure the
*
*******************************************************/
int GetProcWindHandles(HANDLE hProc,HWND **pphWnds,int *pSize);

/*******************************************************
*  to get the windows top windows for it
*  input :
*        pWnds      windows handler to query
*        wndnum    windows number
*        ppTopWnds top window handler to store
*        pTopSize   *ppTopWnds size
*  output :
*        return number stored in the *ppTopWnds
*        otherwise negative error code ,and get the GetLastError() to find the
*        error code
*
*
*   remark:
*        *ppTopWnds must be NULL or malloced buffer pointer
*        and it will be freed when expand size
*        if pWnds == NULL ,it will free *ppTopWnds
*
*******************************************************/
int GetTopWinds(HWND *pWnds,int wndnum,HWND **ppTopWnds,int *pTopSize);


/*******************************************************
*  input :
*      hwnd for window to test
*
*  output:
*      0 for not 1 for fullscreen
*
*  remark :
*      if fail ,just return 0
*******************************************************/
extern "C" int IsWndFullScreen(HWND hwnd);

/*******************************************************
*
*  input:
*      hwnd  the windows to get the buffer
*      pData  the buffer to get the bitmap for the buffer
*      iLen length of the data
*      pFormat  format stored of data
*      pWidth  width of the pData of bmp
*      pHeight height of the pData of bmp
*
*
*  output:
*       return the filled length of the pData if success
*       otherwise negative error code
*
*  remark :
*      for pData for it ,and it will fill in it
*  
*******************************************************/
extern "C" int GetWindowBmpBuffer(HWND hwnd,uint8_t *pData,int iLen,int* pFormat,int* pWidth,int* pHeight);

#ifdef __cplusplus
}
#endif



#endif  /*__WIN_PROC_H__*/

