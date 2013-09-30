

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

BOOL CPcmCapper::__SetOperationNone()
{
    if(this->m_hProc == NULL)
    {
        /*it is not set*/
        return TRUE;
    }

	/*now to call the */
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
    this->m_iOperation = PCMCAPPER_OPERATION_NONE;

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

	memset(&(this->m_FillEvtBaseName),0,sizeof(this->m_FillEvtBaseName));
	memset(&(this->m_FreeEvtBaseName),0,sizeof(this->m_FreeEvtBaseName));

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

    if(lasterr)
    {
        SetLastError(lasterr);
    }
    else
    {
        SetLastError(0);
    }
    return totalbret;
}

CPcmCapper::~CPcmCapper()
{
    this->Stop();
}