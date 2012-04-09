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
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


// renderers
#include "CPURTRenderer.h"
#include "OpenGLRenderer.h"
#include "PhotonMapperCPU.h"

#ifdef WIN32
#include "GPURayTracer.h"
#endif

// acceleration structures
#include "BasicAS.h"
#include "UniformGrid.h"
#include "KDTree.h"
#include "Bvh.h"

#include "SimpleProfiler.h"

#ifdef WIN32
#include <windows.h>
#endif

#include "Singleton.h"
#include "Logfile.h"

// other includes
#include "Options.h"

#include "DynamicScene.h"

#ifdef WIN32
#include <GL/glut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#endif

#ifndef WIN32
typedef unsigned int DWORD;
#define GetTickCount() 0
#define Sleep(x)
#endif

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

#include "Common.h"

// use this to help check for memory leaks
#define DumpLeaks()                                  \
{                                                    \
    printf("%s : line %d\n" , __FILE__, __LINE__);   \
    _CrtDumpMemoryLeaks();                           \
}


///////// GLOBAL VARIABLES  ////////////////////////////////////////////////////

// renderers
enum RENDERER {OPENGL, CPURT, GPURT, PMCPU };
OpenGLRenderer *ogl = NULL;
CPURTRenderer *crt = NULL;
#ifdef WIN32
GPURayTracer *gpurt = NULL;
#else
OpenGLRenderer *gpurt = NULL;
#endif
PhotonMapperCPU *pm = NULL;
Renderer* m_pRenderer = NULL;

// acceleration structures
enum ACCELSTRUCT {BASIC, UNIFORM, KDTREE, BVH};

AccelerationStructure* m_pAccelStruct = NULL;


// default configuration
RENDERER RENDERMODE = OPENGL;
ACCELSTRUCT ACCELMODE = BASIC;
char OpenGLKey = '1';
char CpuRTKey = '2';
char GpuRTKey = '3';
char PMCPUKey = '4';
char BasicGridKey = 'b';
char UniformGridKey = 'u';
char KDTreeKey = 'k';
char BVHKey = 'd';
char animateCameraKey = 'r';
std::string g_configFile = "config/config.txt";
std::string g_sceneFile = "scenes/scene.txt";
bool g_bAnimateCamera = false;
char * Title = "ENCORE";
int WINDOW_WIDTH = 256;
int WINDOW_HEIGHT = 256;
int WINDOW_TOP = 100;
int WINDOW_LEFT = 100;
int IMAGE_WIDTH = 256;
int IMAGE_HEIGHT = 256;

std::string g_outputImageFilename;
std::string g_logFilename;
int g_MaxFrames = 0;
bool g_RepeatRender = false;

////////  PROTOTYPES ////////////////////////////
void idle();

////////////////////////////////////////////////

// global objects
Point3f mousePos;
int MOUSE_STATE;
DynamicScene scn;
Camera cam;
Options g_ConfigOptions;


unsigned long RenderCount = 0;


// LOGGING
int framecount = 0;

// TESTING
fstream g_TestDataFile;
std::string g_BatchName;
std::string g_TestName;

std::string g_OutputDir = ".\\";

//////////////////////////////////////
void usage( void )
{
    printf("\n\nUsage:  encore.exe <config file> <image name>\n\n");

    printf("\n\n\nClosing in 10 seconds...\n");
    Sleep( 5000 );
}

void quit()
{
    // delete old accelStruct
    if ( m_pAccelStruct )
    {
        delete m_pAccelStruct;
        m_pAccelStruct = NULL;
    }

    if ( m_pRenderer )
    {
        // delete old Renderer
        m_pRenderer->deinit();
        delete m_pRenderer;
        m_pRenderer = NULL;
    }


    exit(0);
}

void parseArguments(int argc, char** argv)
{
    if (argc != 3)
    {
        usage();
    }
    else if(argc == 3)
    {
        // only supported argument is the config file name
        g_configFile = argv[1];
        g_outputImageFilename = argv[2];

        g_BatchName = "defaultBatch";
        g_TestName  = "defaultTest";

        g_logFilename = "default-log.txt";
    } 
}



void parseConfigFile( void )
{
    string var;
    string value;

    fstream file;
    file.open ( g_configFile.c_str(), fstream::in );
    if (!file.good())
    {
        file.close();
        cout << "Can't read config file" << endl;
        Sleep( 5000 );
        exit(0);
    }
    while (!file.eof())
    {
        file >> var;
        if (var.at(0) == '*'){ // comments
            file.ignore(1000,'*');
        }
        else
        {  // a command

            file >> value;
            g_ConfigOptions.addOption( var, value );
        }
    }
    file.close();
}

void configure( void )
{
    // get options that are needed locally
    if ( g_ConfigOptions.doesOptionExist( "TITLE" ) ) { Title = (char*) g_ConfigOptions.getOption( "TITLE" ).c_str(); }
    if ( g_ConfigOptions.doesOptionExist( "WINDOW_TOP" ) ) { WINDOW_TOP = atoi(g_ConfigOptions.getOption( "WINDOW_TOP" ).c_str()); }
    if ( g_ConfigOptions.doesOptionExist( "WINDOW_LEFT" ) ) { WINDOW_LEFT = atoi(g_ConfigOptions.getOption( "WINDOW_LEFT" ).c_str()); }
    if ( g_ConfigOptions.doesOptionExist( "WINDOW_WIDTH" ) ) { WINDOW_WIDTH = atoi(g_ConfigOptions.getOption( "WINDOW_WIDTH" ).c_str()); }
    if ( g_ConfigOptions.doesOptionExist( "WINDOW_HEIGHT" ) ) { WINDOW_HEIGHT = atoi(g_ConfigOptions.getOption( "WINDOW_HEIGHT" ).c_str()); }
    if ( g_ConfigOptions.doesOptionExist( "IMAGE_WIDTH" ) ) { IMAGE_WIDTH = atoi(g_ConfigOptions.getOption( "IMAGE_WIDTH" ).c_str()); }
    if ( g_ConfigOptions.doesOptionExist( "IMAGE_HEIGHT" ) ) { IMAGE_HEIGHT = atoi(g_ConfigOptions.getOption( "IMAGE_HEIGHT" ).c_str()); }
    if ( g_ConfigOptions.doesOptionExist( "SCENE_FILE" ) ) { g_sceneFile = g_ConfigOptions.getOption( "SCENE_FILE" ); }


    if ( g_ConfigOptions.doesOptionExist( "RENDERMODE" ) )
    {
        if ( g_ConfigOptions.getOption( "RENDERMODE" ) == "OpenGL" ) { RENDERMODE = OPENGL; }
        else if ( g_ConfigOptions.getOption( "RENDERMODE" ) == "CpuRT" ) { RENDERMODE = CPURT; }
        else if ( g_ConfigOptions.getOption( "RENDERMODE" ) == "GpuRT" ) { RENDERMODE = GPURT; }
        else if ( g_ConfigOptions.getOption( "RENDERMODE" ) == "PMCPU" ) { RENDERMODE = PMCPU; }
    }
    if ( g_ConfigOptions.doesOptionExist( "ACCELMODE" ) )
    {
        if ( g_ConfigOptions.getOption( "ACCELMODE" ) == "Basic" ) 
        { ACCELMODE = BASIC; }
        else if ( g_ConfigOptions.getOption( "ACCELMODE" ) == "UniformGrid" ) 
        { ACCELMODE = UNIFORM; }
        else if ( g_ConfigOptions.getOption( "ACCELMODE" ) == "KdTree" ) 
        { ACCELMODE = KDTREE; }
        else if ( g_ConfigOptions.getOption( "ACCELMODE" ) == "BVH" ) { ACCELMODE = BVH; }
    }

    if ( g_ConfigOptions.doesOptionExist( "OpenGLKey" ) )
    {
        OpenGLKey = g_ConfigOptions.getOption( "OpenGLKey" ).at(0);
    }
    if ( g_ConfigOptions.doesOptionExist( "CpuRTKey" ) )
    {
        CpuRTKey = g_ConfigOptions.getOption( "CpuRTKey" ).at(0);
    }
    if ( g_ConfigOptions.doesOptionExist( "GpuRTKey" ) )
    {
        GpuRTKey = g_ConfigOptions.getOption( "GpuRTKey" ).at(0);
    }
    if ( g_ConfigOptions.doesOptionExist( "BasicGridKey" ) )
    {
        BasicGridKey = g_ConfigOptions.getOption( "BasicGridKey" ).at(0);
    }
    if ( g_ConfigOptions.doesOptionExist( "UniformGridKey" ) )
    {
        UniformGridKey = g_ConfigOptions.getOption( "UniformGridKey" ).at(0);
    }
    if ( g_ConfigOptions.doesOptionExist( "KDTreeKey" ) )
    {
        KDTreeKey = g_ConfigOptions.getOption( "KDTreeKey" ).at(0);
    }
    if ( g_ConfigOptions.doesOptionExist( "AnimateCameraKey" ) )
    {
        animateCameraKey = g_ConfigOptions.getOption( "AnimateCameraKey" ).at(0);
    }
    if( g_ConfigOptions.doesOptionExist("MaxFrames"))
    {
        g_MaxFrames = atoi(g_ConfigOptions.getOption("MaxFrames").c_str());
    }
    if( g_ConfigOptions.doesOptionExist("RepeatRender"))
    {
        g_RepeatRender = (g_ConfigOptions.getOption("RepeatRender") == "true");
    }

    if ( g_ConfigOptions.doesOptionExist( "AnimateCamera" ) )
    {
        std::string value = g_ConfigOptions.getOption( "AnimateCamera" );
        if ( ( value == "true") || ( value == "TRUE" ) || (value == "yes" ) )
        {
            g_bAnimateCamera = true;
        }
        else
        {
            g_bAnimateCamera = false;
        }
    }
}

void initRenderer( void )
{
    // delete old accelStruct
    if ( m_pAccelStruct )
    {
        delete m_pAccelStruct;
        m_pAccelStruct = NULL;
    }

    // first init AccelStruct
    switch(ACCELMODE)
    {
    case BASIC:
        printf("ACCELERATION STRUCTURE: Basic\n");
        m_pAccelStruct = new BasicAS();
        break;
    case UNIFORM:
        printf("ACCELERATION STRUCTURE: Uniform Grid\n");
        m_pAccelStruct = new UniformGrid();
        break;
    case KDTREE:
        printf("ACCELERATION STRUCTURE: Kd-Tree\n");
        //kdt.useSAH(true);
        m_pAccelStruct = new Kdtree();
        break;
    case BVH:
        printf("ACCELERATION STRUCTURE: Bounding Volume Hierarchy\n");
        m_pAccelStruct = new Bvh();
        break;
    }

    m_pAccelStruct->configure( &g_ConfigOptions );

    // then init renderer
    switch(RENDERMODE)
    {
    case OPENGL:
        printf("RENDERER: OpenGL\n");
        m_pRenderer = ogl;
        break;
    case CPURT:
        printf("RENDERER: CPU Ray Tracer\n");
        m_pRenderer = crt;
        break;
    case GPURT:
        printf("RENDERER: GPU Ray Tracer\n");
        m_pRenderer = gpurt;
        break;
    case PMCPU:
        printf("RENDERER: Photon Mapper\n");
        m_pRenderer = pm;
        break;
    }

    m_pRenderer->configure( &g_ConfigOptions );
    m_pRenderer->init(&scn, m_pAccelStruct, &cam);

}

void deinitRenderer( void )
{
    m_pRenderer->deinit();
}

void myInit( void )
{
    ogl = new OpenGLRenderer();
    crt = new CPURTRenderer();
#ifdef WIN32
    //    gpurt = new GPURayTracer();
#else
    gpurt = new OpenGLRenderer();
#endif
    pm = new PhotonMapperCPU(IMAGE_WIDTH, IMAGE_HEIGHT);
    pm->SetImageFilename(g_outputImageFilename);

    scn.load( g_sceneFile );

    cam = scn.getCamera();
    cam.setViewport(WINDOW_WIDTH, WINDOW_HEIGHT);

    initRenderer();
}

void myReshape(int x, int y) 
{
    WINDOW_WIDTH = x;
    WINDOW_HEIGHT = y;
    cam.setViewport(x,y);
}

void myKey(unsigned char key , int x,int y)
{
    // ensures that renderer is only init'ed once
    bool bDeinitRenderer = false;
    bool bInitRenderer = false;

    // quit program
    if ( key == 'q' || key == 27 || key == 'Q' )
    {
        quit();
    }
    else if ( key == animateCameraKey )
    {
        g_bAnimateCamera = !g_bAnimateCamera;
    }

    // set renderer
    if ( key == OpenGLKey )
    {
        if ( RENDERMODE != OPENGL )
        {
            bDeinitRenderer = true;
            RENDERMODE = OPENGL;
            bInitRenderer = true;
        }
    }
    else if ( key == CpuRTKey )
    {
        if ( RENDERMODE != CPURT )
        {
            bDeinitRenderer = true;
            RENDERMODE = CPURT;
            bInitRenderer = true;
        }
    }
    else if ( key == GpuRTKey )
    {
        if ( RENDERMODE != GPURT )
        {
            bDeinitRenderer = true;
            RENDERMODE = GPURT;
            bInitRenderer = true;
        }
    }
    else if( key == PMCPUKey)
    {
        if( RENDERMODE != PMCPU)
        {
            bDeinitRenderer = true;
            RENDERMODE = PMCPU;
            bInitRenderer = true;
        }
    }

    // set acceleration structure
    if ( key == BasicGridKey )
    {
        if ( ACCELMODE != BASIC )
        {
            ACCELMODE = BASIC;
            bInitRenderer = true;
        }
    }
    else if ( key == UniformGridKey )
    {
        if ( ACCELMODE != UNIFORM ) 
        {
            ACCELMODE = UNIFORM;
            bInitRenderer = true;
        }
    }
    else if ( key == KDTreeKey )
    {
        if ( ACCELMODE != KDTREE ) 
        {
            ACCELMODE = KDTREE;
            bInitRenderer = true;
        }
    }
    else if ( key == BVHKey )
    {
        if ( ACCELMODE != BVH ) 
        {
            ACCELMODE = BVH;
            bInitRenderer = true;
        }
    }
    if ( bDeinitRenderer == true )
    {
        deinitRenderer();
    }
    if ( bInitRenderer == true )
    {
        initRenderer();
    }
    else
    {
        // we still have the same renderer
        // so send along the pressed key
        m_pRenderer->keyboard( key );
        m_pAccelStruct->keyboard( key );
    }

    glutPostRedisplay();
}

void myMouse(int button,int state ,int x ,int y ) 
{
    if(button ==  GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            MOUSE_STATE = 1; // down
            mousePos = Point3f(x,y,0);
        }
        else
        {
            MOUSE_STATE = 0; // up
        }
    }
}

void myMMouse(int x,int y) 
{
    int dx,dy;
    if(MOUSE_STATE)
    {
        dx = (x-mousePos.X());
        dy = (y-mousePos.Y());
        mousePos = Point3f(x,y,0);
        cam.rotateAboutLook(dx,dy);
    }
    glutPostRedisplay();
}

SimpleProfiler frameProf;

void myDisplay( void )
{
    if(!m_pRenderer->IsRefineInProgress())
    {
        frameProf.Start();
        std::string logLine;
        Singleton<Logfile>::Get().Stream() << "----------------- FRAME "
                                           << intToString(framecount) << " -------------------------" << std::endl;

        RenderCount++;
    }

    m_pRenderer->Refine();

    if(m_pRenderer->IsImageComplete())
    {
        frameProf.Stop();
        frameProf.PrintLastTime("Frame Render Time: ");

        framecount++;
        std::string newTitle = Title;
        newTitle += ": Frame " + intToString(framecount);
        glutSetWindowTitle(newTitle.c_str());

        if(g_MaxFrames > 0 && g_MaxFrames <= framecount)
        {
            quit();
        }

        if(!g_RepeatRender)
        {
            glutIdleFunc(idle);
        }
        else
        {
            glutPostRedisplay();
        }
    }
}

void idle( void ) 
{
    if ( g_bAnimateCamera == true )
    {
        cam.rotateAboutLook(1,0);

        glutIdleFunc(myDisplay);

        glutPostRedisplay();
    } 

    Sleep( 10 );
}




int main(int argc, char** argv)
{
    std::cout << "ENCORE v1.0" << std::endl;
    std::cout << "Developed by: Chen-Hao Chang, Brandon Light, and Peter Lohrman" << std::endl;

#ifdef WIN32
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
    parseArguments(argc, argv);

    // BEGIN LOG FILE
    Singleton<Logfile>::Create();
    Singleton<Logfile>::Get().Open(g_logFilename);

    parseConfigFile( );

    configure();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(WINDOW_LEFT, WINDOW_TOP);
    glutCreateWindow(Title);


    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutMouseFunc(myMouse);
    glutMotionFunc(myMMouse);
    glutKeyboardFunc(myKey);
    glutIdleFunc(myDisplay);

    myInit();

    glutMainLoop();

    Singleton<Logfile>::Get().Close();
    Singleton<Logfile>::Release();

    return 0;
}