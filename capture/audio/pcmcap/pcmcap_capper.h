
#ifndef __PCMCAP_CAPPER_H__
#define __PCMCAP_CAPPER_H__

#include <pcmcap_callback.h>


typedef struct
{
	HANDLE m_hThread;
	unsigned int m_ThreadId;
	int m_ThreadRunning;
	int m_ThreadExited;	
} THREAD_CONTROL_t;

class CPcmCapper
{
public:
    CPcmCapper();
    ~CPcmCapper();


    enum PCMCAPPER_OPERATION
    {
        PCMCAPPER_OPERATION_NONE = 0,	// ���Խ�����������ץȡ�򲥷ŵĶ���
        PCMCAPPER_OPERATION_CAPTURE, 	// �Խ�����������ץȡ�Ķ���
        PCMCAPPER_OPERATION_RENDER, 	// �Խ����������в��ŵĶ��������Ž���ϵͳȥ��ɣ������Ԥ��
        PCMCAPPER_OPERATION_BOTH, 		// ����������ͬʱ����
    };

    BOOL Start(HANDLE hProc, int iOperation, int iBufNum,int iBlockSize, IPcmCapperCallback * pPcc, LPVOID lpParam);
    BOOL Stop();

    BOOL SetAudioOperation(int iOperation);
    int GetAudioOperation() const;

protected:

private:
	BOOL __SetOperationNone();
	BOOL __SetOperationInner(PCMCAP_CONTROL_t* pControl,DWORD* pRetCode);
	int __CreateMap();
	void __DestroyMap();
	int __CreateEvent();
	void __DestroyEvent();

private:
    HANDLE m_hProc;     // ���̾��
    DWORD m_ProcessId;  // processid of the m_hProc
    THREAD_CONTROL_t m_ThreadControl;
    int m_iOperation;	// �Խ��̽��еĲ���
    
    IPcmCapperCallback * m_pPcmCapperCb;
    LPVOID m_lpParam;

	unsigned int m_BufNum;
	unsigned int m_BufBlockSize;
	HANDLE m_hMapFile;
	unsigned char *m_pMapBuffer;
	unsigned char m_FreeEvtBaseName[128];
	unsigned char m_FillEvtBaseName[128];
	unsigned char m_MapBaseName[128];
	HANDLE m_pFreeEvt;
	HANDLE m_pFillEvt;
};


#endif /*__PCMCAP_CAPPER_H__*/

