/****************************************************************************
Brandon Light
06/10/2006

Shader.h

Wrapper class for a Cg shader.  Supports vertex and fragment shaders alike
****************************************************************************/

#pragma once

#ifdef WIN32
//#pragma comment( lib, "cg.lib" )
//#pragma comment( lib, "cggl.lib" )
#elif __APPLE__
#include <GLUT/glut.h>
#endif

#include <string>
#include <map>
#include <list>

using std::string;

#define STUB_SHADER 1


namespace encore 
{
    class CShader
    {
    public:  enum ShaderType { VERTEX, FRAGMENT };

    public:
	    ~CShader();

	    // return the shader type...
	    ShaderType GetType(){ return m_Type; }
	    // bind the shader to its profile
	    void Bind();
	    // unbind the shader from its profile
	    void Unbind();
	    // activate the shader via enabling its profile
	    void Enable();
	    // deactivate the shader by disabling its profile
	    void Disable();

#ifndef STUB_SHADER
	    // return a named parameter cooresponding to the shader
	    CGparameter& GetNamedParameter(string name, bool isTextureParam = false);
#endif // _STUB_SHADER
   
        // enables texture parameters
	    void EnableTextureParameters();
	    // disables texture parameters
	    void DisableTextureParameters();

    private:
	    friend class CShaderManager;

#ifndef STUB_SHADER
	    CShader(ShaderType type, int shaderID, CGprofile& profile);
	    CShader(const CShader& s);

	    CGprogram GetProgram(){ return m_Program; }
	    void SetProgram(CGprogram program){ m_Program = program; }
#endif // STUB_SHADER

    private:
	    // enumerated type of the shader
	    ShaderType m_Type;
	    // the id of the shader
	    int m_ShaderID;

#ifndef STUB_SHADER
	    // the shader profile
	    CGprofile  &m_Profile;
	    // the shader program itself
	    CGprogram  m_Program;
	    // parameters
	    std::map<string, CGparameter> m_Parameters;
	    // texture parameter names
	    std::list<string> m_TextureParamNames;
#endif // STUB_SHADER
    };
}
