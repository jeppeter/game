
#include "pcmcap_democallback.h"

CPcmCapDemoCallBack::CPcmCapDemoCallBack()
{
    m_fp = NULL;
}

CPcmCapDemoCallBack::~CPcmCapDemoCallBack()
{
    if(m_fp)
    {
        fclose(m_fp);
    }
    m_fp = NULL;
}

