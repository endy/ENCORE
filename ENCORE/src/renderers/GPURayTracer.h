#pragma once
#include "renderer.h"
#include "ShaderManager.h"
#include "FrameBufferObject.h"

enum
{
    // vertex shaders
    VS_EYE_RAY_GENERATOR = VERTEX_SHADER_BASE_ID,

    // fragment shaders
    FS_EYE_RAY_GENERATOR = FRAGMENT_SHADER_BASE_ID,
    FS_TRAVERSER,
    FS_PHONG,
    FS_DISPLAY
};

class GPURayTracer :
    public Renderer
{
public:
    GPURayTracer(void);
    virtual ~GPURayTracer(void);

    void usage( void );
    void init( Scene* l_pScene, AccelerationStructure* l_pAccelStruct, Camera* l_pCamera );
    void configure( Options* l_pOptions );
    void render( void );
    void deinit( void );
    void keyboard( unsigned char key );

private:
    void init_FBOs();
    void init_shaders();

	void Workspace();

    // shaders
	void EyeKernel();
    void ShadeKernel();
	void TIKernel();
    void DisplayKernel();

private:
    void printTextureValues(GLenum texture);

    GLuint m_glQuery;

    GPUAccelerationStructureData m_ASData;

    // manager of raytracing shaders 
   	CShaderManager* m_pShaderManager;

    GLenum filterMode;
    const static GLenum texInternalFormat = GL_RGBA32F_ARB;
    const static GLenum texTarget = GL_TEXTURE_RECTANGLE_ARB;

    CFrameBufferObject m_FBO;

    GLuint tex[10]; // texture 0 is used for ray direction from eyekernal
	// tex[0] = eye ray dir     Ray Start
	// tex[1] = voxel           Eye Ray Dir -> Reflected Ray Dir
	// tex[2] = hitinfo         Hit Information
	// tex[3] = tmax -> color   Refracted Ray Dir
	// tex[4] = s ray start     Shadow Ray Dir
	// tex[5] = s ray dir       tMax
	// tex[6] = normal          Voxel
	// tex[7] = hitpos

	int maxCoordS;
	int maxCoordT;
    
    unsigned int m_pixelRows, m_pixelCols;
    std::list<Triangle*> *triangleList;
};
