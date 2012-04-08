/**************************
Brandon Light


NOTES:

This is a bare-bones texture class.  It makes several assumptions 
which may limit its usefulness for mulitple types of OpenGL textures.  

Current assumptions:
-pixel data is in RGBA format

**************************/

#pragma once

#if __APPLE__
#include <OpenGL/gl.h>
#elif WIN32
#include <Windows.h>
#include <GL/gl.h>
#endif

#include <map>
#include <algorithm>

enum PIXEL_WIDTH
{
    RGB  = 3,
    RGBA = 4
};

class CTexel
{
friend class CTexture;

public:
    CTexel(float r, float g, float b, float a);
    CTexel(float pixel[], int pixelWidth);
    ~CTexel();

public:
    float R(){ return m_Data[0]; }
    float G(){ return m_Data[1]; }
    float B(){ return m_Data[2]; }
    float A(){ return m_Data[3]; }

private:
    float m_Data[4];
};

class CTexture
{
public:
    CTexture(int width, int height, int pixelWidth);
    virtual ~CTexture();

public:
    int GetWidth(){ return m_Width; }
    int GetHeight(){ return m_Height; }
    int GetPixelWidth(){ return m_PixelWidth; }

    CTexel GetPixel(int x, int y);
    void   SetPixel(int x, int y, const CTexel& t);

private:
    float* m_Data;
    
    int m_Width;
    int m_Height;
    int m_PixelWidth;
};


template <class T>
class TextureGL
{
public:

	TextureGL(GLenum type, GLenum target, int width, int height, T* textureData);
	virtual ~TextureGL();

	// creates a texture object using OpenGL
	void Create(bool useDefaultParams = true);

	// binds the texture
	void Bind();

    void SetTextureParameter(GLenum pname, GLint param);
	
	// flag whether the texture is usable
	bool IsValid(){ return m_Valid; }

	int Width(){ return m_WIDTH; }
	int Height(){ return m_HEIGHT; }
	
    GLenum GetType(){ return m_Type; }
    GLenum GetTarget(){ return m_Target; }

	T*	Data(){ return m_TextureData; }

private:  // METHODS
	// destroys the texture object that was created
	void Destroy();

private: // DATA
	bool m_Valid;

	const int m_WIDTH;
	const int m_HEIGHT;
	
	GLuint m_TextureName;
	GLenum m_Type;
    GLenum m_Target;

	T*  m_TextureData;

	const int m_PIXEL_SIZE;

    std::map<GLenum, GLint> m_TextureParams;
};

//////  TYPEDEFS  ///////////
typedef TextureGL<GLfloat> TextureGLf;
typedef TextureGL<GLint> TextureGLi;

////////////////  IMPLEMENTATION //////////////////////////////////////////////////


template <class T> TextureGL<T>::TextureGL(GLenum type, GLenum target, int width, int height, T* textureData):
	m_WIDTH(width), m_HEIGHT(height), m_PIXEL_SIZE(4)
{
	if(textureData == NULL)
	{
		m_Valid = false;
        return;
	}

	m_Type   = type;
    m_Target = target;
	
	int pixelValues = m_WIDTH * m_HEIGHT * m_PIXEL_SIZE;
	m_TextureData = new T[pixelValues]; 

	std::copy(textureData, textureData + pixelValues, m_TextureData);
       
	m_Valid = true;

}

// deallocate m_data
template <class T> TextureGL<T>::~TextureGL()
{
	if(!m_Valid)
	{
		return;
	}
	
	Destroy();

	delete[] m_TextureData;
	m_TextureData = NULL;
}

template <class T> void TextureGL<T>::SetTextureParameter(GLenum pname, GLint param)
{
    m_TextureParams[pname] = param;
}

template <class T> void TextureGL<T>::Create(bool useDefaultParams /* = true */)
{
	if(!m_Valid)
	{
		return;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glGenTextures(1, &m_TextureName);
	glBindTexture(m_Target, m_TextureName);
	
    if(useDefaultParams)
    {   
        glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else
    {
        std::map<GLenum, GLint>::iterator params = m_TextureParams.begin();

        while(params != m_TextureParams.end())
        {
            glTexParameteri(m_Target, params->first, params->second);
            params++;
        }
    }
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_WIDTH, m_HEIGHT, 0, GL_RGBA, m_Type, m_TextureData);
	
}

template <class T> void TextureGL<T>::Bind()
{
	if(m_Valid)
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureName);
	}
}

template <class T> void TextureGL<T>::Destroy()
{
	// delete this GL texture object
	glDeleteTextures(1, &m_TextureName);
}
