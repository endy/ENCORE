
#include "ShaderManager.h"

#include <cstdio>
#include <cstdlib>

void reportError(const char* error)
{
#ifdef WIN32
    MessageBox(NULL, error, "Error", MB_OK);
#elif __APPLE__
    printf(error);
#endif
}

namespace encore
{
    CShaderManager::CShaderManager()
    {
	    Initialize();
    }

    CShaderManager::~CShaderManager()
    {
#ifndef STUB_SHADERMANAGER
	    while(!m_Shaders.empty())
	    {
		    // remove shader from the list
		    std::pair<int, CShader*> shaderPair = *m_Shaders.begin();
    		
		    // delete shader
            UnloadShader( shaderPair.first );
	    }

	    cgDestroyContext(m_Context);
#endif // STUB_SHADERMANAGER
    }

    void CShaderManager::Initialize()
    {
#ifndef STUB_SHADERMANAGER
	    // TODO: Should Cg errors cause an exit??  That seems very harsh.
	    // figure out a way to do a graceful failure later on -Brandon

	    // Get Cg Context
	    m_Context = cgCreateContext();
	    if(m_Context == NULL)
	    {
		    reportError("Unable to get Cg Context");
		    exit(1);
	    }
	    // Get Latest Vertex Profile
	    //g_VertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);	
	    m_VertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);	
	    if(m_VertexProfile == CG_PROFILE_UNKNOWN)
	    {
		    CGerror Error = cgGetError();
		    reportError( cgGetErrorString(Error));
            exit(1);
	    }
	    cgGLSetOptimalOptions(m_VertexProfile);

	    // Get Latest Fragment Profile
	    m_FragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
	    if(m_FragmentProfile == CG_PROFILE_UNKNOWN)
	    {
		    CGerror Error = cgGetError();
		    reportError(cgGetErrorString(Error));
		    exit(1);
	    }
	    cgGLSetOptimalOptions(m_FragmentProfile);	
#endif // STUB_SHADERMANAGER
    }

    void CShaderManager::LoadVertexShader(int shaderID, string filename, string entryFunction /* = "" */)
    {
#ifndef STUB_SHADERMANAGER
	    CGprogram program;
    	
        if( filename.length() == 0 )
        {
            reportError("Can't load an empty filename.");
            return;
        }

	    if(entryFunction.length() == 0)
	    {
		    program = cgCreateProgramFromFile(
					    m_Context,					// context
					    CG_SOURCE,					// type: source or object
					    filename.c_str(),			// program text/data
					    m_VertexProfile,			// profile
					    NULL,						// entry function
					    0);							// args
	    }
	    else
	    {
		    program = cgCreateProgramFromFile(
					    m_Context,					// context
					    CG_SOURCE,					// type: source or object
					    filename.c_str(),			// program text/data
					    m_VertexProfile,			// profile
					    entryFunction.c_str(),		// entry function
					    0);							// args
	    }

	    if (program == NULL)
	    {
		    CGerror Error = cgGetError();
		    reportError(cgGetErrorString(Error));
		    exit(1);
	    }
	    cgGLLoadProgram(program);

	    // store this program as a CShader
	    m_Shaders[shaderID] = new CShader(CShader::VERTEX, shaderID, m_VertexProfile);
	    m_Shaders[shaderID]->SetProgram(program);
#endif // STUB_SHADERMANAGER
    }
    void CShaderManager::LoadFragmentShader(int shaderID, string filename, string entryFunction /* = "" */)
    {
#ifndef STUB_SHADERMANAGER
	    CGprogram program;
        
        if( filename.length() == 0 )
        {
            reportError("Can't load an empty filename.");
            return;
        }

	    if(entryFunction.length() == 0)
	    {
		    program = cgCreateProgramFromFile(
					    m_Context,					// context
					    CG_SOURCE,					// type: source or object
					    filename.c_str(),			// program text/data
					    m_FragmentProfile,			// profile
					    NULL,						// entry function
					    NULL);						// args
	    }
	    else
	    {
				    program = cgCreateProgramFromFile(
					    m_Context,					// context
					    CG_SOURCE,					// type: source or object
					    filename.c_str(),			// program text/data
					    m_FragmentProfile,			// profile
					    entryFunction.c_str(),		// entry function
					    NULL);						// args
	    }

	    if (program == NULL)
	    {
		    CGerror Error = cgGetError();
		    reportError(cgGetErrorString(Error));
		    exit(1);
	    }
	    cgGLLoadProgram(program);

	    // store this program as a CShader
	    m_Shaders[shaderID] = new CShader(CShader::FRAGMENT, shaderID, m_FragmentProfile);
	    m_Shaders[shaderID]->SetProgram(program);
#endif // STUB_SHADERMANAGER
    }

    CShader& CShaderManager::GetShader(int shaderID)
    {
	    return *(m_Shaders[shaderID]);
    }

    void CShaderManager::UnloadShader(int shaderID)
    {
#ifndef STUB_SHADERMANAGER

        cgDestroyProgram(m_Shaders[shaderID]->GetProgram());
    	
	    // remove the shader from the shaders list
	    CShader *shader = m_Shaders[shaderID];
	    m_Shaders.erase(shaderID);

	    // delete the shader
	    delete shader;
	    shader = NULL;	
#endif // STUB_SHADERMANAGER
    }
}
