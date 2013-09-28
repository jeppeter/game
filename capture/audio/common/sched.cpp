
#include <sched.h>
#include <Windows.h>

int SchedOut()
{
    BOOL bret;

    bret = SwitchToThread();
    if(!bret)
    {
        Sleep(10);
    }
    return 0;
}

