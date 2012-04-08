#pragma once
#include <string>
#include <map>
#include <fstream>

class Options
{
public:
    Options(void);
    ~Options(void);

    bool doesOptionExist( std::string optionName );
    std::string getOption( std::string optionName );

    bool addOption( std::string optionName, std::string optionValue );

    void LogOption(std::string optionName, std::fstream *logfile);

private:
    std::map<std::string, std::string> m_Options;
};
