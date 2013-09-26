

#ifndef  __PCM_CAP_COMMON_H__
#define  __PCM_CAP_COMMON_H__

typedef struct
{
} PCMCAP_AUDIO_BUFFER_t;

typedef struct
{
}PCMCAP_AUDIO_NOTIFY_t;

#define  PCMCAP_AUDIO_GET             1
#define  PCMCAP_AUDIO_SET             2
#define  PCMCAP_AUDIO_NOTIFY_START    3
#define  PCMCAP_AUDIO_NOTIFY_STOP     4
#define  PCMCAP_AUDIO_START           5
#define  PCMCAP_AUDIO_STOP            6

typedef struct 
{
	unsigned int m_Cmd;
} PCMCAP_COMMAND_t;




#endif /*__PCM_CAP_COMMON_H__*/

