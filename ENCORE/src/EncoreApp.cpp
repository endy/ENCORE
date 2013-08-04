

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

EncoreApp::~EncoreApp()
{

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

/*
void FormatDebugOutputAMD(char outStr[], size_t outStrSize, GLenum category, GLuint id,
    GLenum severity, const char *msg)
{
    char categoryStr[32];
    const char *categoryFmt = "UNDEFINED(0x%04X)";
    switch(category)
    {
    case GL_DEBUG_CATEGORY_API_ERROR_AMD:          categoryFmt = "API_ERROR"; break;
    case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:      categoryFmt = "WINDOW_SYSTEM"; break;
    case GL_DEBUG_CATEGORY_DEPRECATION_AMD:        categoryFmt = "DEPRECATION"; break;
    case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD: categoryFmt = "UNDEFINED_BEHAVIOR"; break;
    case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:        categoryFmt = "PERFORMANCE"; break;
    case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:    categoryFmt = "SHADER_COMPILER"; break;
    case GL_DEBUG_CATEGORY_APPLICATION_AMD:        categoryFmt = "APPLICATION"; break;
    case GL_DEBUG_CATEGORY_OTHER_AMD:              categoryFmt = "OTHER"; break;
    }
    _snprintf(categoryStr, 32, categoryFmt, category);
    char severityStr[32];
    const char *severityFmt = "UNDEFINED";
    switch(severity)
    {
    case GL_DEBUG_SEVERITY_HIGH_AMD:   severityFmt = "HIGH";   break;
    case GL_DEBUG_SEVERITY_MEDIUM_AMD: severityFmt = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_LOW_AMD:    severityFmt = "LOW";    break;
    }
    _snprintf(severityStr, 32, severityFmt, severity);
    _snprintf(outStr, outStrSize, "OpenGL: %s [category=%s severity=%s id=%d]",
        msg, categoryStr, severityStr, id);
}
*/
/*
typedef void (APIENTRY *GLDEBUGPROCAMD)(GLuint id,
    GLenum category,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    GLvoid* userParam);

void DebugCallbackAMD(
    GLuint id,
    GLenum category,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    GLvoid *userParam)
{
    (void)length;
    FILE *outFile = (FILE*)userParam;
    char finalMsg[256];
    FormatDebugOutputAMD(finalMsg, 256, category, id, severity, message);
    fprintf(outFile, "%s\n", finalMsg);
}
*/


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
#if !(IVY_GL_ES)
    InitGL2();

    BOOL quit = FALSE;
    m_pWindow->Show();
    
    GLShader* pVSShader = GLShader::CreateFromFile(IvyVertexShader, "VertexShader", "shaders/gl2.vert");
    GLShader* pFSShader = GLShader::CreateFromFile(IvyFragmentShader, "FragmentShader", "shaders/gl2.frag");

    GLProgram* pProgram = GLProgram::Create();
    pProgram->AttachShader(pVSShader);
    pProgram->AttachShader(pFSShader);

    pProgram->Link();
    pProgram->Bind();

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


    glViewport(0, 0, m_screenWidth, m_screenHeight);

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
            m_pCamera->Move(Point3(0, 0, 0.05f), 0, 0);
        }
        else if (pKeys->Pressed[Key_S])
        {
            m_pCamera->Move(Point3(0, 0, -0.05f), 0, 0);
        }

        if (pKeys->Pressed[Key_A])
        {
            m_pCamera->Move(Point3(-0.05f, 0, 0), 0, 0);
        }
        if (pKeys->Pressed[Key_D])
        {
            m_pCamera->Move(Point3(0.05f, 0, 0), 0, 0);
        }

        glClear(GL_COLOR_BUFFER_BIT);


        IvySwapBuffers();
    }

    pProgram->Destroy();

#endif // !(IVY_GL_ES)
}

