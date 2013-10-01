
#include "timeticks.h"


#define MAX_TICKS          0xffffffff
#define TICKS_PER_SECOND   1000

BOOL InitializeTicks(unsigned int*pStartTick,unsigned int *pCurTick,unsigned int *pEndTick,int timeout)
{
    unsigned int stick,ctick,etick;

    if(timeout >= (MAX_TICKS/TICKS_PER_SECOND))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    stick = GetTickCount();
    ctick = stick;
    etick = stick + timeout*TICKS_PER_SECOND;

    *pStartTick = stick;
    *pCurTick = ctick;
    *pEndTick = etick;
    return TRUE;
}


BOOL GetCurrentTicks(unsigned int * pCurTick)
{
    *pCurTick = GetTickCount();
    return TRUE;
}

unsigned int LeftTicks(unsigned int * pStartTick,unsigned int * pCurTick,unsigned int * pEndTick,int timeout)
{
    unsigned int stick,ctick,etick,totalticks=timeout * TICKS_PER_SECOND;
    unsigned int leftticks;


    SetLastError(0);
    if(timeout >= (MAX_TICKS/TICKS_PER_SECOND))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    stick = *pStartTick;
    ctick = *pCurTick;
    etick = *pEndTick;

    if(etick >= stick)
    {
        if((etick - stick) != totalticks)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }
        if(ctick >= etick)
        {
            return 0;
        }
        else
        {
            return (etick - ctick);
        }
    }

    /*now it is the time for calculate ,we just make sure*/
    if((etick < stick) && (ctick >= etick) && (ctick < stick))
    {
        return 0;
    }

    /*now to calculate*/
    if(ctick > stick)
    {
        leftticks = etick ;
        leftticks += (MAX_TICKS - ctick + 1)
    }
    else
    {
        leftticks = (etick - ctick);
    }

    return leftticks;
}

