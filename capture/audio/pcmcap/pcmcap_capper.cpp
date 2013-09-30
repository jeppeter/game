

#include "pcmcap_capper.h"

CPcmCapper::CPcmCapper()
{
	m_hProc = NULL;
	m_iState = PCMCAPPER_STATE_OPEN;
	m_iOperation = PCMCAPPER_OPERATION_RENDER;
	m_iMaxdelay = 0;
	m_iTimestamp = 0;
	m_pPcmCapperCb = NULL;
	m_lpParam = NULL;
}

