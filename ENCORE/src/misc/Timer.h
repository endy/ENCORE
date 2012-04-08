#pragma once

#ifdef WIN32
#include <windows.h>
#endif

class Timer
{
public:
#ifdef WIN32
    Timer(void);
    ~Timer(void) {}

    void start();
    void stop();
    double queryAccuracy() const // return microsecond per tick
    { return toMicro; }
    double getTime() const // return micro second (1,000,000 micro per second)
    { return (record[1].QuadPart-record[0].QuadPart)*toMicro; } 
#else
    Timer(){}
    ~Timer(){}
    
    void start(){};
    void stop(){};
    double queryAccuracy() const { return 0.0; }
    double getTime() const { return 0.0; }
#endif

private:
#ifdef WIN32
    LARGE_INTEGER ticksPerSecond;
    double toMicro;
    LARGE_INTEGER record[2];
    bool qpc;
#endif
};
