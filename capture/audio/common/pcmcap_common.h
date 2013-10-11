

#ifndef  __PCM_CAP_COMMON_H__
#define  __PCM_CAP_COMMON_H__

#define    PCMCAPPER_OPERATION_NONE      0 	// 不对进程声音进行抓取或播放的动作
#define    PCMCAPPER_OPERATION_CAPTURE   1	// 对进程声音进行抓取的动作
#define    PCMCAPPER_OPERATION_RENDER    2 	// 对进程声音进行播放的动作（播放交给系统去完成，不予干预）
#define    PCMCAPPER_OPERATION_BOTH   	 3  // 对上述两者同时进行

typedef unsigned long ptr_type_t;


typedef struct
{
	int m_Pointer;
    int m_DataLen;
    int m_DataSize;
    unsigned char m_Data[4];
} PCM_AUDIO_DATA_t;

#define  FORMAT_EXTEND_SIZE   128

typedef struct
{
	float m_Volume;
	unsigned char m_Format[FORMAT_EXTEND_SIZE];
} PCM_AUDIO_FORMAT_t;

typedef struct
{
	PCM_AUDIO_FORMAT_t m_Format;
	PCM_AUDIO_DATA_t m_AudioData;
} PCMCAP_AUDIO_BUFFER_t;

#define  PCMCAP_AUDIO_NONE             0
#define  PCMCAP_AUDIO_CAPTURE          1
#define  PCMCAP_AUDIO_RENDER           2
#define  PCMCAP_AUDIO_BOTH             3

enum AVSampleFormat {
    AV_SAMPLE_FMT_NONE = -1,
    AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
    AV_SAMPLE_FMT_S16,         ///< signed 16 bits
    AV_SAMPLE_FMT_S32,         ///< signed 32 bits
    AV_SAMPLE_FMT_FLT,         ///< float
    AV_SAMPLE_FMT_DBL,         ///< double

    AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
    AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
    AV_SAMPLE_FMT_FLTP,        ///< float, planar
    AV_SAMPLE_FMT_DBLP,        ///< double, planar

    AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};



typedef struct
{
	unsigned int  m_Operation;       /*operation code*/
	unsigned int  m_Timeout;         /*the timeout for handle waiting*/
	unsigned char m_MemShareName[128];
	unsigned int  m_MemShareSize;
	unsigned int  m_PackSize;        /*this is for the one packet size*/
	unsigned int  m_NumPacks;        /*packets to do*/
	unsigned char m_FreeListSemNameBase[128]; /*the semphore for free in the injected one get the signalled ,the pointer of packet will into the */
	unsigned char m_FillListSemNameBase[128]; /*the semaphore for release in the injected one ,to signal the filled list */
	unsigned char m_StartEvtName[128];        /*for start event notify name*/
	unsigned char m_StopEvtName[128];         /*for stop event notify name*/
}PCMCAP_CONTROL_t;


typedef struct
{
	HANDLE m_hThread;
	unsigned long m_ThreadId;
	HANDLE m_hExitEvt;
	int m_ThreadRunning;
	int m_ThreadExited;	
} THREAD_CONTROL_t;



#endif /*__PCM_CAP_COMMON_H__*/

