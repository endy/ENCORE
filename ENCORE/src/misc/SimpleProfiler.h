#pragma once

#include <list>
#include <string>
#include <fstream>

class SimpleProfiler
{
private: typedef unsigned long int ULINT;
public:
    SimpleProfiler();
    virtual ~SimpleProfiler();

    void Start();
    void Stop(bool includeInAverage = false);

    bool IsTiming(){ return (m_StartTimes.size() > 0); }

    // clear the list of times and set all values to their defaults
    void Reset();

    void PrintLastTime(std::string message = "", std::fstream *outputFile = NULL, bool printNewline = true);
    void PrintAverageTime(std::string message = "",std::fstream *outputFile = NULL);

    void PrintSumOfTimes(std::string message = "", std::fstream *outputFile = NULL);

private:
    std::list<ULINT> m_Times;
    std::list<ULINT> m_StartTimes;

    float m_RunningAverage;
    int m_TimeSamples;   
};