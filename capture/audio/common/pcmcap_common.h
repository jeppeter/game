

#ifndef  __PCM_CAP_COMMON_H__
#define  __PCM_CAP_COMMON_H__

typedef struct
{
	int m_Format;
	int m_Channels;
	int m_SampleRate;
	int m_BitsPerSample;
	float m_Volume;
	int m_DataLen;
	int m_DataSize;
	unsigned int m_Data[4];
} PCMCAP_AUDIO_BUFFER_t;


#define  PCMCAP_AUDIO_GET             1
#define  PCMCAP_AUDIO_SET             2
#define  PCMCAP_AUDIO_NOTIFY_START    3
#define  PCMCAP_AUDIO_NOTIFY_STOP     4
#define  PCMCAP_AUDIO_START           5
#define  PCMCAP_AUDIO_STOP            6





#endif /*__PCM_CAP_COMMON_H__*/

