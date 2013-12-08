///////////////////////////////////////////////////////////////////////////////////////////////////
///
///     ENCORE Rendering App
///
///     Copyright 2013, Brandon Light
///     All rights reserved.
///
///////////////////////////////////////////////////////////////////////////////////////////////////


////// ENCORE HEADERS //////
// renderers
#include "OpenGLRenderer.h"
#include "PhotonMapperCPU.h"

#ifdef WIN32
#include "GPURayTracer.h"
#endif



// acceleration structures
#include "BasicAS.h"
#include "UniformGrid.h"
#include "Kdtree.h"
#include "BVH.h"

#include "SimpleProfiler.h"

#ifdef WIN32
#include <windows.h>
#endif

// other includes
#include "Options.h"

#include "DynamicScene.h"


#ifndef WIN32
typedef unsigned int DWORD;
#define GetTickCount() 0
#define Sleep(x)
#endif

using namespace std;


#include <iostream>
#include <fstream>
#include <vector>

#include "Common.h"
#include "Singleton.h"
#include "Logfile.h"


/// IVY HEADERS
#include "EncoreApp.h"
#include "IvyUtils.h"
#include "IvyImporter.h"
#include "IvyCamera.h"

#include "IvyWindow.h"
#include "GLTexture.h"
#include "GLShader.h"
#include "GLMesh.h"

#include "IvyGL.h"

using namespace Ivy;


///////// GLOBAL VARIABLES  ////////////////////////////////////////////////////

// renderers
enum RENDERER {OPENGL, CPURT, GPURT, PMCPU };
OpenGLRenderer *ogl = NULL;
RayTracer* rt = NULL;
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
std::string g_configFile = "config/pmConfigSimpleIrrTest.txt";
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
int g_MaxFrames = 0;
bool g_RepeatRender = false;

////////////////////////////////////////////////

Point3f mousePos;
int MOUSE_STATE;
DynamicScene scn;
Camera cam;
Options g_ConfigOptions;
unsigned long RenderCount = 0;

// TESTING
fstream g_TestDataFile;
std::string g_BatchName;
std::string g_TestName;

std::string g_OutputDir = ".\\";




///////////////////////////////////////////////////////////////////////////////////////////////////
/// EncoreApp::EncoreApp
///
/// @brief
///     Constructor
/// @return
///     N/A
///////////////////////////////////////////////////////////////////////////////////////////////////
EncoreApp::EncoreApp()
    :
    IvyApp(),
    m_useEgl(FALSE)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// EncoreApp::~EncoreApp
///
/// @brief
///     Destructor
/// @return
///     N/A
///////////////////////////////////////////////////////////////////////////////////////////////////
EncoreApp::~EncoreApp()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// EncoreApp::Configure
///
/// @brief
///     
/// @return
///     N/A
///////////////////////////////////////////////////////////////////////////////////////////////////
bool EncoreApp::Configure(
    unsigned int argc,
    const char** argv)
{
    bool ret = IvyApp::Configure(argc, argv);

    // read ENCORE args

    g_outputImageFilename = "output.png";

    ///@todo Use app config rather than read the command line parameter
    if (argc >= 3)
    {
        g_outputImageFilename = argv[2];
    }

    g_BatchName = "defaultBatch";
    g_TestName  = "defaultTest";


    // initialize configs
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

    /// read configs and set globals
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

    return ret;
}

EncoreApp* EncoreApp::Create()
{
    EncoreApp* pApp = new EncoreApp();

    if (pApp->Init() == false)
    {
        pApp->Destroy();
        pApp = NULL;
    }

    return pApp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// EncoreApp::Destroy
///
/// @brief
///     Destroys the instance
/// @return
///     N/A
///////////////////////////////////////////////////////////////////////////////////////////////////
void EncoreApp::Destroy()
{
    IvyApp::Destroy();
}


bool EncoreApp::Init()
{
    bool success = IvyApp::Init();

    return success;
}

void EncoreApp::ReceiveEvent(
    const Event* pEvent)
{
    IvyApp::ReceiveEvent(pEvent);

    switch (pEvent->GetType())
    {
        case EventTypeWindowResize:
            glViewport(0, 0, m_pWindow->GetDrawableArea().right, m_pWindow->GetDrawableArea().bottom);
            break;
        default:
            break;
    }

}

void EncoreApp::Run()
{

    ogl = new OpenGLRenderer();
    rt = new RayTracer(IMAGE_WIDTH, IMAGE_HEIGHT);
    rt->SetImageFilename(g_outputImageFilename);
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

    ///@TODO refactor this blob of code
    //*
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
        m_pRenderer = rt;
        break;
    case PMCPU:
        printf("RENDERER: Photon Mapper\n");
        m_pRenderer = pm;
        break;
    }

    m_pRenderer->configure( &g_ConfigOptions );
    m_pRenderer->init(&scn, m_pAccelStruct, &cam);
    //*/

    InitGL2();

    BOOL quit = FALSE;
    char windowName[256];
    sprintf(windowName, "ENCORE (Frame %d)", RenderCount);
    m_pWindow->SetWindowName(&windowName[0]);

    m_pWindow->Show();
    
    GLShader* pVSShader = GLShader::CreateFromFile(IvyVertexShader, "VertexShader", "shaders/gl2.vert");
    GLShader* pFSShader = GLShader::CreateFromFile(IvyFragmentShader, "FragmentShader", "shaders/gl2.frag");

    GLProgram* pProgram = GLProgram::Create();
    pProgram->AttachShader(pVSShader);
    pProgram->AttachShader(pFSShader);

 //   pProgram->Link();
 //   pProgram->Bind();

    struct CameraBufferData
    {
#if XNA_MATH
        XMMATRIX worldMatrix;
        XMMATRIX viewMatrix;
        XMMATRIX projectionMatrix;
#else
        IvyMatrix4x4 worldMatrix;
        IvyMatrix4x4 viewMatrix;
        IvyMatrix4x4 projectionMatrix;
#endif
    };


 //   glViewport(0, 0, m_screenWidth, m_screenHeight);

    GLubyte indices[] = {0, 1, 2, 0, 2, 3 };
    while (!quit)
    {
        ProcessUpdates();

        const IvyGamepadState* pGamepad = GetGamepadState();
        const KeyboardState* pKeys = GetKeyboardState();

        if (pGamepad->ButtonPressed[IvyGamepadButtons::ButtonA] || pKeys->Pressed[Key_0])
        {
            glClearColor(0.0f, 4.0f, 0.0f, 1.0f);
        }
        else if (pGamepad->ButtonPressed[IvyGamepadButtons::ButtonB] || pKeys->Pressed[Key_1])
        {
            glClearColor(0.4f, 0.0f, 0.0f, 1.0f);
        }
        else
        {
            glClearColor(0.4f, 1.0f, 0.4f, 1.0f);
        }
        

        if (pKeys->Pressed[Key_W])
        {
            m_pCamera->Move(Ivy::Point3(0, 0, 0.05f), 0, 0);
        }
        else if (pKeys->Pressed[Key_S])
        {
            m_pCamera->Move(Ivy::Point3(0, 0, -0.05f), 0, 0);
        }

        if (pKeys->Pressed[Key_A])
        {
            m_pCamera->Move(Ivy::Point3(-0.05f, 0, 0), 0, 0);
        }
        if (pKeys->Pressed[Key_D])
        {
            m_pCamera->Move(Ivy::Point3(0.05f, 0, 0), 0, 0);
        }

       // glClear(GL_COLOR_BUFFER_BIT);

        if(!m_pRenderer->IsRefineInProgress())
        {
            RenderCount++;

            sprintf(windowName, "ENCORE (Frame %d)", RenderCount);
            m_pWindow->SetWindowName(&windowName[0]);
        }

            m_pRenderer->Refine();


        if(m_pRenderer->IsImageComplete())
        {
            if(g_MaxFrames > 0 && g_MaxFrames <= RenderCount)
            {
                quit = true;
            }
        }

        IvySwapBuffers();
    }



    pProgram->Destroy();

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
}

void EncoreApp::ProcessKeyUpdates()
{
    // ensures that renderer is only init'ed once
    bool bDeinitRenderer = false;
    bool bInitRenderer = false;

    /*
    // quit program
    if ( key == animateCameraKey )
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
        m_pRenderer->deinit();
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
    */
}

void EncoreApp::ProcessMouseUpdates() 
{
    /*
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

    int dx,dy;
    if(MOUSE_STATE)
    {
        dx = (x-mousePos.X());
        dy = (y-mousePos.Y());
        mousePos = Point3f(x,y,0);
        cam.rotateAboutLook(dx,dy);
    }
    */
}
