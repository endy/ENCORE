/****************************************************************************
Brandon Light
06/10/2006

PhotonMapperGPU.h

Rendering class that uses a programmable GPU to render images using a 
photon mapping algorithm

****************************************************************************/

#pragma once

#include "FrameBufferObject.h"

#include "Renderer.h"
#include "PhotonMap.h"
#include "Photon.h"
#include "Color.h"

#include "ShaderManager.h"
#include "TextureMaker.h"

#include "UniformGrid.h"

using encore::Color;


using encore::CShaderManager;

enum 
{
	///// VERTEX SHADERS //////////////////////////
    // GPU photon mapping shaders
    STENCIL_SORT    = encore::VERTEX_SHADER_BASE_ID,

	///// FRAGMENT SHADERS ///////////////////////
    // GPU photon mapping shaders
    PHOTON_TRACER = encore::FRAGMENT_SHADER_BASE_ID,
    BITONIC_SORT,
    RADIANCE_ESTIMATOR,
    // GPU ray tracing shaders
    RAY,
	RAY_GENERATOR,
	NEW_TRAVERSER,
	PHONG,
	SHADOW,
	DISPLAY,
};


class PhotonMapperGPU : public Renderer
{
public:	// METHODS

	PhotonMapperGPU(Scene* in_scene, int in_pixelRows, int in_pixelCols, int globalPhotonCount, int causticsPhotonCount);

	virtual ~PhotonMapperGPU();

	//  Render a frame of the scene
	virtual void RenderFrame();

    // Renderer interface
    virtual void usage( void ) {}
    virtual void configure( Options* l_pOptions ) {}
    virtual void init(Scene* scn, AccelerationStructure* accelStruct, Camera* cam){}
    virtual void render(void){}
    virtual void deinit(void){}
    virtual void keyboard( unsigned char key ) {}

public:  // DATA

	// Rendering Properties
	// TODO: make these properties flags that use a properties function w/ enumeration
	bool	ShowTraces;
	bool	ShowPhotons;
	bool	Reflections;
	size_t	MaxBlockIndex;
	bool	RussianRoulette;
	int		MaxBounces;

private: // METHODS
		
	void Initialize();

	//////////  High Level Photon Map Methods ///////////////

    void TracePhotonsKernel();

    void SortPhotonsKernel();

    void EstimateRadianceKernel();

    void RenderImage();
    
    // return a ray cast from the eye to the given pixel coord
	Ray CalcEyeRay(int pixelRow, int pixelColumn);

	// clear photon map
	void ResetPhotonMaps();
	
	//// SHADER CODE ///////////////////////////

    enum RAYTYPE { SHADOW = 0, REFLECT, REFRACT };

    void printTextureValues(GLenum texture);
    void InitGridData();

    double GetTexMemUsage(int w, int h);
    void RaytraceFrame(Scene inScn);

    // general
    void GLInit();
	void CleanupRayTracer();

    // shaders
	void Workspace();
	void EyeKernel();
    void ShadeKernel();
    void ShadowKernel();
    void Sec_RayKernel(RAYTYPE type);
	void TIKernel();
	void GridKernel();
    void DisplayKernel();


	//// DEBUG RENDERING ALGORITHMS ////////////

	
private:  // DATA

	CShaderManager* m_ShaderManager;

 	// photon maps...
	PhotonMap* m_globalPhotonMap;
	PhotonMap* m_causticsPhotonMap;

	Color m_backgroundColor;

	// maximum number of pixel rows/cols for display
	int m_pixelRows, m_pixelCols;
	// array of pixel block sizes used for progressive rendering 
	vector<int> m_blockSizes;

	bool m_photonMapsBuilt;

	const int GLOBAL_PHOTON_COUNT;
	const int CAUSTIC_PHOTON_COUNT;

	// misc rendering data
	vector< vector<Point3f> > photonPaths;

	// texture to render the frame to
	TextureGL<GLfloat> *m_Image;


	 //////////  NEW or REFACTORED DATA FROM GPURT ////////////////////////////

    CFrameBufferObject m_FBO;

    //////////  GLOBAL DATA FROM GPURT ////////////////////////////
    
    const static GLenum texInternalFormat = GL_RGBA32F_ARB;
    const static GLenum texTarget = GL_TEXTURE_RECTANGLE_ARB;
    GLenum filterMode;

    GLuint tex[10]; // texture 0 is used for ray direction from eyekernal
    GLuint vtex[3];  // tex id for vertex
    GLuint ntex[3];  // tex id for normal per vertex
    GLuint ctex; // grid cell texture

    unsigned int GpuTraverseLoop;


    ////////////////////   END  //////////////////////////////////

    // class shader info
	bool doshadow;
	unsigned int texWidth;
	unsigned int texHeight;
	int maxCoordS;
	int maxCoordT;
	// tex[0] = eye ray dir
	// tex[1] = voxel
	// tex[2] = hitinfo
	// tex[3] = tmax -> color
	// tex[4] = s ray start
	// tex[5] = s ray dir
	// tex[6] = normal
	// tex[7] = hitpos
	int displaytype;

	UniformGrid ug;

    bool m_Done;
    bool m_PrintTIData;
	
};
