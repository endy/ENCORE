//////////////////////////////////////////////////////////////////////////
// ENCORE PROJECT
// 
// Encore Batch - a program that creates scripts for batch jobs 
// for testing
//
// Developed By
//      Brandon Light:   brandon@alum.wpi.edu
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>

const int LINE_SIZE = 512;

std::string g_testDir               = "tests/";
std::string g_resultsDir            = "results/";
std::string g_imagesDir             = "images/";
std::string g_exePath               = "encore.exe";

std::string g_batchFilename         = "default.bat";
std::string g_scriptFilename        = "default.sh";



void printUsage()
{
    std::cerr << "EncoreBatch.exe <batch config>\n" << std::endl;
}

void readHeader(std::fstream& batchConfig)
{
    char *line = new char[LINE_SIZE];

    while(batchConfig.getline(line, LINE_SIZE, '\n'))
    {
        if(strlen(line) == 0){ break; }

        if(line[0] == '#'){ continue; }

        char *delim = "=";
        char *name = strtok(line, delim);
        char *value = strtok(NULL, delim);;
        
        if(strcmp(name, "testDir") == 0)
        {
            g_testDir = value;
        }
        else if(strcmp(name, "resultsDir") == 0)
        {
            g_resultsDir = value;
        }
        else if(strcmp(name, "imagesDir") == 0)
        {
            g_imagesDir = value;
        }
        else if(strcmp(name, "exe") == 0)
        {
            g_exePath = value;
        }
        else if(strcmp(name, "batchFile") == 0)
        {
            g_batchFilename = value;
        }
        else if(strcmp(name, "scriptFile") == 0)
        {
            g_scriptFilename = value;
        }

    }
}

void parseBatch(std::string batchName, std::string controlTestFilename, std::fstream& batchConfig, std::fstream& batchFile, std::fstream& scriptFile)
{
    // create the batch directories
    batchFile << "mkdir " << g_resultsDir << "\\" << batchName << std::endl;

    ////////////////////////////////////
    //write out control test command
    ////////////////////////////////////
    std::string controlTestName(controlTestFilename, 0, controlTestFilename.rfind('.'));

    std::string outDir = g_resultsDir + "\\" + batchName + "\\";

    batchFile << g_exePath << " " << g_testDir << "\\" << controlTestFilename << " " << batchName << " " << controlTestName << " " << outDir << std::endl;

    ///////////////////////////////////
    // script
    ///////////////////////////////////
    scriptFile << "convert " 
               << "\"" << batchName + "/" + controlTestName + "-image.png\"" << " "
               << "\"" << batchName + "/" + controlTestName + ".sgi\"" << std::endl << std::endl;

    std::string testNamesCSV = batchName + "/" + batchName + "testNames.csv";
    std::string testDiffsCSV = batchName + "/" + batchName + "testDiffs.csv";

    /////////////////////////////////////////////////////////////////////////

    char *testBuffer = new char[LINE_SIZE];
    // write out subsequent tests
    while(batchConfig.getline(testBuffer, LINE_SIZE, '\n'))
    {
        // skip comments and break if there are no subsequent tests
        if(strlen(testBuffer) == 0){ break; }
        if(testBuffer[0] == '#'){ continue; }

        std::string testFilename = testBuffer;

        // write out test command to batchfile
        std::string testName(testFilename, 0, testFilename.rfind('.'));
        
        batchFile << g_exePath << " " << g_testDir << "\\" << testFilename << " " << batchName << " " <<  testName << " " << outDir << std::endl;

        ///////////////////////////////////
        // script
        ///////////////////////////////////

        std::string imagesPath = batchName + "/";

        // write out commands for script file
        scriptFile << "convert " 
                   << "\"" << imagesPath + testName + "-image.png\"" << " "
                   << "\"" << imagesPath + testName + ".sgi\"" << std::endl;

        std::string diffName = controlTestName + "_" + testName;
        
        scriptFile << "\necho \"" << testName << ",\" >> " << testNamesCSV << std::endl;
        scriptFile << "./ltdiff -v -o " << "\"" << imagesPath + diffName << ".sgi\" " 
                   << "\"" << imagesPath + controlTestName << ".sgi\" " << "\"" <<imagesPath + testName << ".sgi\" "
                   << " | sed -e 's/.*[ ]*=[ ]*//' >> " << testDiffsCSV
                   << std::endl;

        scriptFile << "convert " << "\"" << imagesPath + diffName << ".sgi\" " << "\"" << imagesPath + diffName << ".png\"" << std::endl << std::endl;
    }

    scriptFile << "rm " << batchName << "/*.sgi" << std::endl;
}

int main(int argc, char **argv)
{
    std::fstream batchConfig;

    if(argc != 2)
    {
        printUsage();
        exit(1);
    }
    else
    {
       batchConfig.open(argv[1], std::ios_base::in);   
    }

    readHeader(batchConfig);

    
    char *batchCommand = new char[LINE_SIZE];

    // create batch file
    std::fstream batchFile(g_batchFilename.c_str(), std::ios_base::out);
    batchFile << "mkdir " << g_resultsDir << std::endl;

    // create script file
    std::fstream scriptFile(g_scriptFilename.c_str(), std::ios_base::out);
    scriptFile << "#!/bin/tcsh" << std::endl << std::endl;
    
    while(batchConfig.getline(batchCommand, LINE_SIZE, '\n'))
    {
        if(strlen(batchCommand) == 0){ continue; }

        if(batchCommand[0] == '#'){ continue; }

        char* delim = " ";

        char *command = strtok(batchCommand, delim);

        if(strcmp(command, "batch") == 0)
        {
            char *batchName = strtok(NULL, delim);
            char *controlTest = strtok(NULL, delim);

            parseBatch(batchName, controlTest, batchConfig, batchFile, scriptFile);
        }
        else
        {
            std::cerr << "Unknown command: " << command << std::endl;
        }

    }

    batchFile.close();
    scriptFile.close();

    return 0;

}