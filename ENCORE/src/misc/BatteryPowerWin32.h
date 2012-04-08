#pragma once

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>

#ifdef WIN32
#include <windows.h>

//#include <windows.h>
// need this extern; otherwise, linker get ANGRY!!
extern "C" {
#include <PowrProf.h>
}

# pragma comment(lib, "powrprof.lib")
#endif

//using namespace std;

// this struct take 12 byte
// if you are sampling every 1 sec, you can run for about 24 hour before the virutal memory of this thread run out
// .5 sec for 12 hour and so for
struct myData
{
    time_t rawtime; // time , 8
    long dcrate; // discharge rate in mW , 2
    unsigned long remain; // remaining volt in mWh, 2
};
#define MAXPD 43200

class BatteryPowerWin32
{
public:

#ifdef WIN32
    BatteryPowerWin32(void): sample_rate(1000), status(1), myhandle(NULL) {};
    ~BatteryPowerWin32(void) {};
    bool start(unsigned int rate);
    void stop(std::string filename);
    unsigned int getSampleRate() const { return sample_rate; }
    void print();

    SYSTEM_BATTERY_STATE bs;
    long status;
    myData* powerdata; // data per sample
    int mysize;
private:

    //static DWORD WINAPI ThreadProc(LPVOID lpParameter);
    //static DWORD WINAPI update(LPVOID lpParameter);
    void printfile(std::string filename);    
    unsigned int sample_rate; // mili second per sample
    LPDWORD ThreadId; // id to thread
    HANDLE myhandle;

    DWORD m_totalTime;
#elif __APPLE__
    BatteryPowerWin32(){};
    ~BatteryPowerWin32(){}
    bool start(unsigned int rate){ return true; }
    void stop(std::string filename){};
    unsigned int getSampleRate() const { return 0; }
    void print(){};

#endif

};

