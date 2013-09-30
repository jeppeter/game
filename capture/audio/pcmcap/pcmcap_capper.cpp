

#include "pcmcap_capper.h"

CPcmCapper::CPcmCapper()
{
    m_hProc = NULL;
    m_iOperation = PCMCAPPER_OPERATION_NONE;
    m_pPcmCapperCb = NULL;
    m_lpParam = NULL;
    m_BufNum = 0;
    memset(&(m_FreeEvtBaseName),0,sizeof(m_FreeEvtBaseName));
    memset(&(m_FillEvtBaseName),0,sizeof(m_FillEvtBaseName));
    m_pFillEvt = NULL;
    m_pFreeEvt = NULL;
}


BOOL CPcmCapper::Stop()
{
    int lasterr=0;
    BOOL bret,totalbret=True;
    int i;

    if(this->m_hProc)
    {
        /*now to set the */
        bret = this->__SetOperationNone();
        if(!bret)
        {
            totalbret = False;
            lasterr = lasterr ? lasterr : (LAST_ERROR_CODE());
        }
    }

    for(i=0; i<this->m_BufNum; i++)
    {
        /*now to close handle*/
        if(this->m_pFillEvt)
        {
            if(this->m_pFillEvt[i])
            {
                CloseHandle(this->m_pFillEvt[i]);
            }
            this->m_pFillEvt[i] = NULL;
        }

        if(this->m_pFreeEvt)
        {
            if(this->m_pFreeEvt[i])
            {
                CloseHandle(this->m_pFreeEvt[i]);
            }
            this->m_pFreeEvt[i] = NULL;
        }
    }

    if(this->m_pFillEvt)
    {
        free(this->m_pFillEvt);
    }
    this->m_pFillEvt = NULL;

    if(this->m_pFreeEvt)
    {
        free(this->m_pFreeEvt);
    }
    this->m_pFreeEvt = NULL;


}

CPcmCapper::~CPcmCapper()
{
    this->Stop();
}