
#ifndef __PCMCAP_CAPPER_H__
#define __PCMCAP_CAPPER_H__

#include <pcmcap_callback.h>

class CPcmCapper
{
public:
    CPcmCapper();
    ~CPcmCapper();


    enum PCMCAPPER_OPERATION
    {
        PCMCAPPER_OPERATION_NONE = 0,	// 不对进程声音进行抓取或播放的动作
        PCMCAPPER_OPERATION_CAPTURE, 	// 对进程声音进行抓取的动作
        PCMCAPPER_OPERATION_RENDER, 	// 对进程声音进行播放的动作（播放交给系统去完成，不予干预）
        PCMCAPPER_OPERATION_BOTH, 		// 对上述两者同时进行
    };

    BOOL Start(HANDLE hProc, int iOperation, int iBufNum, IPcmCapperCallback * pPcc, LPVOID lpParam);
    BOOL Stop();

    BOOL SetAudioOperation(int iOperation);
    int GetAudioOperation() const;

protected:

private:
	BOOL __SetOperationNone();

private:
    HANDLE m_hProc;     // 进程句柄
    DWORD m_ProcessId;  // processid of the m_hProc
    int m_iState;		// 进程声音状态
    int m_iOperation;	// 对进程进行的操作
    
    IPcmCapperCallback * m_pPcmCapperCb;
    LPVOID m_lpParam;

	unsigned int m_BufNum;
	unsigned char m_FreeEvtBaseName[128];
	unsigned char m_FillEvtBaseName[128];
	HANDLE m_pFreeEvt;
	HANDLE m_pFillEvt;
};


#endif /*__PCMCAP_CAPPER_H__*/

