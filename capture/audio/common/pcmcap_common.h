

#ifndef  __PCM_CAP_COMMON_H__
#define  __PCM_CAP_COMMON_H__

typedef struct
{
    int m_DataLen;
    int m_DataSize;
    unsigned int m_Data[4];
} PCM_AUDIO_DATA_t;

typedef struct
{
    int m_Format;
    int m_Channels;
    int m_SampleRate;
    int m_BitsPerSample;
    float m_Volume;
	PCM_AUDIO_DATA_t m_AudioData;
} PCMCAP_AUDIO_BUFFER_t;

#define  PCMCAP_AUDIO_NONE             0
#define  PCMCAP_AUDIO_CAPTURE          1
#define  PCMCAP_AUDIO_RENDER           2
#define  PCMCAP_AUDIO_BOTH             3


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
}PCMCAP_CONTROL_t;




#endif /*__PCM_CAP_COMMON_H__*/

