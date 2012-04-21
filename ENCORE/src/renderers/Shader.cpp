
#include "Shader.h"

#ifdef WIN32
#include "Windows.h"
#endif

#ifndef WIN32
#define STUB_CSHADER
#endif

namespace encore
{
    CShader::CShader(ShaderType type, int shaderID, CGprofile& profile):
	    m_Profile(profile)
    {
	    m_Type = type;
	    m_ShaderID = shaderID;
    }

    CShader::~CShader()
    {

    }

    CShader& CShader::operator=(const CShader& rhs)
    {
	    m_Type       = rhs.m_Type;
	    m_ShaderID   = rhs.m_ShaderID;
	    m_Profile    = rhs.m_Profile;
	    m_Program    = rhs.m_Program;

	    m_Parameters.insert(rhs.m_Parameters.begin(), rhs.m_Parameters.end());

	    return *this;
    }

    void CShader::Bind()
    {
// stub	    cgGLBindProgram(m_Program);
    }

    void CShader::Unbind()
    {
// stub	    cgGLUnbindProgram(m_Profile);
    }

    void CShader::Enable()
    {
// stub	    cgGLEnableProfile(m_Profile);
    }

    void CShader::Disable()
    {
// stub	    cgGLDisableProfile(m_Profile);
    }
#ifndef STUB_CSHADER
    CGparameter& CShader::GetNamedParameter(string name, bool isTextureParam /* = false */)
    {

	    m_Parameters[name] = cgGetNamedParameter(m_Program, name.c_str());

	    if (m_Parameters[name] == NULL)
	    {
		    // remove this parameter from the parameter list
		    m_Parameters.erase(name);

		    string error = "Failed to get parameter '" + name + "'\n";
            #ifdef WIN32
		    MessageBox(NULL, error.c_str(), "Error loading Shader Parameter", MB_OK);
            #elif __APPLE__
            printf( error.c_str() );
            #endif
		    // HACK: this is pretty harsh--do we want to fail like this?
//		    exit(1);
	    }

        if(isTextureParam)
        {
            m_TextureParamNames.push_back(name);
        }

	    return m_Parameters[name];
    }

    void CShader::EnableTextureParameters()
    {
	    std::list<string>::iterator iter = m_TextureParamNames.begin();
	    while(iter != m_TextureParamNames.end())
	    {
		    string name = *iter;
		    cgGLEnableTextureParameter(m_Parameters[name]);
		    iter++;
	    }
    }

    void CShader::DisableTextureParameters()
    {		
	    std::list<string>::iterator iter = m_TextureParamNames.begin();

	    while(iter != m_TextureParamNames.end())
	    {
		    string name = *iter;
		    cgGLDisableTextureParameter(m_Parameters[name]);
		    iter++;
	    }
    }
#endif // STUB_CSHADER
}
