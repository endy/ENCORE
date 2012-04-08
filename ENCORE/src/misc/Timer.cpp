#include "Timer.h"
#include <stdio.h>

#ifdef WIN32

Timer::Timer(void)
{
    record[0].QuadPart = record[1].QuadPart = 0;
    if(QueryPerformanceFrequency(&ticksPerSecond))
    {
        qpc = true;
        toMicro = (double)1/ticksPerSecond.QuadPart*1000000;
    }
    else
    {
        printf("can't use qpc\n");
        qpc = false;
        toMicro = 1000;
    }

}

void Timer::start()
{
    if(qpc)
        QueryPerformanceCounter(&record[0]);
    else
        record[0].QuadPart = GetTickCount();
}

void Timer::stop()
{
    if(qpc)
        QueryPerformanceCounter(&record[1]);
    else
        record[1].QuadPart = GetTickCount();
}

#endif


