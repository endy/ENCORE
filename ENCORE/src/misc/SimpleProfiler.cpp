#include "SimpleProfiler.h"

#ifdef WIN32
#include <Windows.h>
#else
#define GetTickCount() 0
#endif

#include <iostream>

SimpleProfiler::SimpleProfiler()
{
    Reset();
}

SimpleProfiler::~SimpleProfiler()
{

}


void SimpleProfiler::Start()
{
    ULINT startTime = GetTickCount();
    m_StartTimes.push_back(startTime);
}

void SimpleProfiler::Stop(bool includeInAverage /* = false */)
{
    if(!IsTiming())
    {
        return;
    }

    ULINT endTime = GetTickCount();
    ULINT startTime = m_StartTimes.back();

    ULINT elapsed = endTime - startTime;

    if(includeInAverage)
    {
        m_RunningAverage = ((m_RunningAverage * m_TimeSamples) + elapsed) / (m_TimeSamples + 1);
        ++m_TimeSamples;
    }
    
    m_Times.push_back(elapsed);
    m_StartTimes.pop_back();
}

void SimpleProfiler::Reset()
{
    m_Times.clear();
    m_StartTimes.clear();

    m_RunningAverage = 0;
    m_TimeSamples = 0;
}

void SimpleProfiler::PrintLastTime(std::string message /* = "" */, std::fstream *outputFile /*  = NULL */, bool printNewline /* = true */)
{
    ULINT lastTime = m_Times.back();
    std::cout << message << "\t" << lastTime  << " ms" << std::endl;

    if(outputFile)
    {
        *outputFile << message << lastTime;
        if(printNewline)
        {
            *outputFile << std::endl;
        } 
    }
}

void SimpleProfiler::PrintAverageTime(std::string message  /* = "" */, std::fstream *outputFile /*  = NULL */)
{
    std::cout << message << "\t" << m_RunningAverage << " ms" << std::endl;

    if(outputFile)
    {
        *outputFile << message << "\t" << m_RunningAverage << " ms" << std::endl;
    }
}

void SimpleProfiler::PrintSumOfTimes(std::string message /* =  */, std::fstream *outputFile /* = NULL */)
{
    std::list<ULINT>::iterator timeIter = m_Times.begin();

    ULINT sum = 0;
    while(timeIter != m_Times.end())
    {
        sum += *timeIter;
        timeIter++;
    }

    std::cout << message << "\t" << sum << " ms" << std::endl;

    if(outputFile)
    {
        *outputFile << message << "\t" << sum << " ms" << std::endl;
    }
}



