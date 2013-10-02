
#ifndef __PCMCAP_CAPPER_H__
#define __PCMCAP_CAPPER_H__

#include <pcmcap_callback.h>


typedef struct
{
	HANDLE m_hThread;
	unsigned int m_ThreadId;
	HANDLE m_hExitEvt;
	int m_ThreadRunning;
	int m_ThreadExited;	
} THREAD_CONTROL_t;

class CPcmCapper
{
public:
    CPcmCapper();
    ~CPcmCapper();

    BOOL Start(HANDLE hProc, int iOperation, int iBufNum,int iBlockSize, IPcmCapperCallback * pPcc, LPVOID lpParam);
    BOOL Stop();

    BOOL SetAudioOperation(int iOperation);
    int GetAudioOperation() const;

protected:

private:
	BOOL __SetOperationNone();
	BOOL __SetOperationCapture();
	BOOL __SetOperationRender();
	BOOL __SetOperationBoth();
	BOOL __SetOperationInner(PCMCAP_CONTROL_t* pControl,DWORD* pRetCode);
	BOOL __StopOperation(int iOperation);
	BOOL __StartOperation(int iOperation);
	int __CreateMap();
	int __FreeAllEvent();
	void __DestroyMap();
	int __CreateEvent();
	void __DestroyEvent();

	int __StartThread();
	void __StopThread();
	static void* ThreadFunc(void* arg);
	void* __ThreadImpl();
	void __AudioRenderBuffer(int idx);

private:
    HANDLE m_hProc;     // ���̾��
    DWORD m_ProcessId;  // processid of the m_hProc
    int m_iOperation;	// �Խ��̽��еĲ���    
    IPcmCapperCallback * m_pPcmCapperCb;
    LPVOID m_lpParam;
	unsigned int m_BufNum;
	unsigned int m_BufBlockSize;
	
	THREAD_CONTROL_t m_ThreadControl;

	HANDLE m_hMapFile;
	unsigned char *m_pMapBuffer;
	unsigned char m_MapBaseName[128];
	
	unsigned char m_FreeEvtBaseName[128];
	unsigned char m_FillEvtBaseName[128];
	HANDLE m_pFreeEvt;
	HANDLE m_pFillEvt;
};


#endif /*__PCMCAP_CAPPER_H__*/

