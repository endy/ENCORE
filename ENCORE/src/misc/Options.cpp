#include "Options.h"
#include <assert.h>

Options::Options(void)
{
}

Options::~Options(void)
{
}

bool Options::addOption( std::string optionName, std::string optionValue )
{
    if ( optionName != "" && optionValue != "" )
    {
        if ( m_Options.find( optionName ) == m_Options.end() )
        {
            // this option is new
            m_Options[ optionName ] = optionValue;
        }
        else
        {
            // this options is already in the map.
            assert( "Can't add Option, the option has already been set." );
            return false;
        }
    }
    else
    {
        assert( "Can't add Option, either name or value is empty." );
        return false;
    }
    return true;
}

bool Options::doesOptionExist( std::string optionName )
{
    std::map<std::string, std::string>::iterator op;
    return ( m_Options.find( optionName ) != m_Options.end() );
}

std::string Options::getOption( std::string optionName )
{
    return m_Options[ optionName ];
}

void Options::LogOption(std::string optionName, std::fstream *logfile)
{
    if(logfile)
    {
        *logfile << optionName << "\t\t" << m_Options[ optionName ] << std::endl;
    }
}