
#pragma once

#pragma comment( lib, "cg.lib" )		// Search For Cg.lib While Linking
#pragma comment( lib, "cggl.lib" )		// Search For CgGL.lib While Linking

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#ifdef WIN32
#include <GL/glut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "Shader.h"
#include <map>

namespace encore
{
    enum
    {
	    // vertex shader ID range: 1000 < ID < 1999
	    VERTEX_SHADER_BASE_ID = 1000,
	    // fragment shader ID range: 2000 < ID < 2999
	    FRAGMENT_SHADER_BASE_ID = 2000,
    };

    class CShaderManager
    {
    public:
	    CShaderManager();

	    ~CShaderManager();
	    // loads a vertex shader
	    void LoadVertexShader(int shaderID, string filename, string entryFunction = "");
	    // loads a fragment shader
	    void LoadFragmentShader(int shaderID, string filename, string entryFunction = "");
	    // return a reference to the shader with the given ID
	    CShader& GetShader(int shaderID);
	    // unload the shader with the given shaderID
	    void UnloadShader(int shaderID);

    private:
	    // initialize all shader data
	    void Initialize();
    	
    private:
	    // Cg context
	    CGcontext m_Context;
	    // Cg Vertex Profile -- specific to this manager
	    CGprofile m_VertexProfile;
	    // Cg Fragment Profile -- specific to this manager
	    CGprofile m_FragmentProfile;

	    std::map<int, CShader*> m_Shaders;

    };
}
