
#ifndef __TIME_TICKS_H__
#define __TIME_TICKS_H__

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL InitializeTicks(unsigned int*pStartTick,unsigned int *pCurTick,unsigned int *pEndTick,int timeout);
BOOL GetCurrentTicks(unsigned int *pCurTick);
unsigned int LeftTicks(unsigned int *pStartTick,unsigned int *pCurTick,unsigned int *pEndTick,int timeout);

#ifdef __cplusplus
}
#endif


#endif /*__TIME_TICKS_H__*/

