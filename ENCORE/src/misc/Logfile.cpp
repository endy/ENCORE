
#include "Logfile.h"

using namespace std;

void Logfile::Open(string logFilename)
{
    m_LogStream.open(logFilename.c_str(), ios_base::out);
}

void Logfile::Close()
{
    m_LogStream.close();
}

void Logfile::Write(string output)
{
    m_LogStream << output;
}

void Logfile::WriteLine(string line)
{
    m_LogStream << line << std::endl;
}
