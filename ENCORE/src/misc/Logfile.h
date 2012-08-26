//////////////////////////////////////////////////////////////////////////
// FRAMEWORK
// 
// Framework description and license goes here.
// 
// Developed By
//      Brandon Light:   brandon.light@gmail.com
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>

class Logfile
{
public:
    Logfile(){};
    virtual ~Logfile(){};

    void Open(std::string logFilename);
    void Close();

    void Write(std::string output);
    void WriteLine(std::string line);

    std::fstream& Stream(){ return m_LogStream; }

private:
    std::fstream m_LogStream;

};

