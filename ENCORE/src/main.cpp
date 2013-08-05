//////////////////////////////////////////////////////////////////////////
// ENCORE PROJECT
// 
// Project description and license goes here.
// 
// Developed By
//      Chen-Hao Chang:  chocobo7@WPI.EDU
//      Brandon Light:   brandon@alum.wpi.edu
//      Peter Lohrman:   plohrmann@alum.WPI.EDU
//////////////////////////////////////////////////////////////////////////



#ifdef WIN32
#ifndef __GNUG__
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // __GCC__
#endif

#include "EncoreApp.h"
#include "Singleton.h"
#include "Logfile.h"

std::string g_logFilename = "encorelog.txt";

// use this to help check for memory leaks
#define DumpLeaks()                                  \
{                                                    \
    printf("%s : line %d\n" , __FILE__, __LINE__);   \
    _CrtDumpMemoryLeaks();                           \
}


int main(int argc, const char** argv)
{
#ifdef WIN32
#ifndef __GNUG__
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
#endif

    // BEGIN LOG FILE
    Singleton<Logfile>::Create();
    Singleton<Logfile>::Get().Open(g_logFilename);

    std::cout << "ENCORE v1.0" << std::endl;
    std::cout << "Developed by: Chen-Hao Chang, Brandon Light, and Peter Lohrman" << std::endl;

    EncoreApp::Configure(argc, argv);

    EncoreApp* pEncoreApp = EncoreApp::Create();

    pEncoreApp->Run();

    Singleton<Logfile>::Get().Close();
    Singleton<Logfile>::Release();

    return 0;
}
