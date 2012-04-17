/****************************************************************************
Brandon Light
06/10/2006

PhotonMapperGPU.cpp

****************************************************************************/

#include "PhotonMapperGPU.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

#include "Common.h"
#include "Material.h"
#include "TextureManager.h"

#define STUB_PHOTONMAPPERGPU 1

using std::vector;

using encore::CShader;

PhotonMapperGPU::PhotonMapperGPU(Scene* scene, int pixelRows, int pixelCols, int globalPhotonCount, int causticsPhotonCount): 
	GLOBAL_PHOTON_COUNT(globalPhotonCount), 
	CAUSTIC_PHOTON_COUNT(causticsPhotonCount)
{
    m_pixelRows = pixelRows;
    m_pixelCols = pixelCols;

    Initialize();

    // ray tracer code
    m_Done = false;

    m_PrintTIData = false;
}

PhotonMapperGPU::~PhotonMapperGPU()
{
	if(m_Image != NULL)
	{
		delete m_Image;
		m_Image = NULL;
	}

	if(m_ShaderManager != NULL)
	{
		delete m_ShaderManager;
		m_ShaderManager = NULL;
	}

    CleanupRayTracer();
}


void PhotonMapperGPU::Initialize() 
{    
    /*
    ////  INIT GRID  ////////////////////////////////////////////////////////
    texWidth  = m_pixelCols;
    texHeight = m_pixelRows;
    maxCoordS = (texTarget == GL_TEXTURE_RECTANGLE_ARB) ? texWidth  : 1;
    maxCoordT = (texTarget == GL_TEXTURE_RECTANGLE_ARB) ? texHeight : 1;

    ug.setSize(20);  // grid size = 20
    ug.build(*m_scene);

    InitGridData();
    
    ////  INIT FRAME BUFFER OBJECT  ////////////////////////////////////////

    glDeleteTextures(10, tex);
    glGenTextures(10, tex);

    filterMode = (texTarget == GL_TEXTURE_RECTANGLE_ARB) ? GL_NEAREST : GL_LINEAR;

    // init texture
    int i;
    for(i=0; i < 10; i++)
    {
	    glBindTexture(texTarget, tex[i]);
	    glTexImage2D(texTarget, 0, texInternalFormat, texWidth, texHeight, 0, GL_RGBA, GL_FLOAT, 0);
	    GET_GLERROR(0);
	    glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, filterMode);
	    glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, filterMode);
	    glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
    }

    m_FBO.Initalize(GL_FRAMEBUFFER_EXT, texTarget);


    //// INIT SHADERS  /////////////////////////////////////////////////////

	m_ShaderManager = new CShaderManager();
    
    // GPU Photon Mapper Shaders
    m_ShaderManager->LoadFragmentShader(PHOTON_TRACER, "shaders/pm/photonTracer.cg");
    m_ShaderManager->LoadVertexShader(STENCIL_SORT, "shaders/pm/stencilSorter.cg");
    m_ShaderManager->LoadFragmentShader(BITONIC_SORT, "shaders/pm/bitonicSorter.cg");
    m_ShaderManager->LoadFragmentShader(RADIANCE_ESTIMATOR, "shaders/pm/radianceEstimator.cg");


    // GPU Ray Tracing Shaders
	m_ShaderManager->LoadFragmentShader(RAY, "shaders/ray_shader.cg");
	m_ShaderManager->LoadFragmentShader(PHONG, "shaders/phong_shader.cg");
	m_ShaderManager->LoadFragmentShader(NEW_TRAVERSER, "shaders/new_traverse_intersector.cg");
	m_ShaderManager->LoadFragmentShader(RAY_GENERATOR, "shaders/sec_ray_generator.cg");
	m_ShaderManager->LoadFragmentShader(DISPLAY, "shaders/display.cg");
	m_ShaderManager->LoadFragmentShader(SHADOW, "shaders/shadow_shader.cg");

	std::cout << "Compiled all shaders" << std::endl;
    */
}


void PhotonMapperGPU::RenderFrame()
{
    // step 1
    TracePhotonsKernel();

    // step 2
    SortPhotonsKernel();

    // step 3
    EstimateRadianceKernel();

    // step 4
    RenderImage();
    
}



/////  PRIVATE MEMBER FUNCTIONS //////////////////////////////

void PhotonMapperGPU::TracePhotonsKernel()
{
    CShader &tracer = m_ShaderManager->GetShader(PHOTON_TRACER);

    // setup shader parameters

    // bind shader
    tracer.Bind();
    tracer.Enable();

    // execute shader
    Workspace();

    tracer.Disable();
}

void PhotonMapperGPU::SortPhotonsKernel()
{
    // setup shader parameters
    CShader &sorter = m_ShaderManager->GetShader(STENCIL_SORT);

    // bind shader
    sorter.Bind();
    sorter.Enable();

    // execute shader
    Workspace();

    sorter.Disable();
}

void PhotonMapperGPU::EstimateRadianceKernel()
{
    // setup shader parameters
    CShader &estimator = m_ShaderManager->GetShader(RADIANCE_ESTIMATOR);

    // bind shader
    estimator.Bind();
    estimator.Enable();

    // execute shader
    Workspace();

    estimator.Disable();
}

void PhotonMapperGPU::RenderImage()
{
    RaytraceFrame(*m_pScene);
}



Ray PhotonMapperGPU::CalcEyeRay(int pixelRow, int pixelColumn)
{
	return Ray();
}

void PhotonMapperGPU::ResetPhotonMaps()
{

}


/////  GPU Ray Tracing Code /////////////////////////////////////


void PhotonMapperGPU::InitGridData()
{  
 /*
  DWORD t1 = GetTickCount();
  //UniformGrid ug = *accStructure;
  //ug.setSize(Gridsize);
	//ug.build(scene);
	
	int numtris = ug.getNumRepTris();
	int numcell = ug.getNumCells();
	int tw = numcell*numcell*numcell;
    std::cout << "grid size per dimension: " << numcell << std::endl;
    std::cout << "total grid needed: " << tw << std::endl;
    std::cout << "total triangles: " << ug.num_Rep_Tris << std::endl;
    std::cout << "oringla triangles count: " << ug.num_Diff_Tris << std::endl;
	int th = tw*INVTL+1;
	tw = tw > TEXTURELIMIT?TEXTURELIMIT:tw;
	// padding to avoid writing invalid memory location into texture
	
    std::cout << "grid texture size: ";
    std::cout << tw << " " << th;
    std::cout << " using: " << GetTexMemUsage(tw,th) << " MB\n";
	glGenTextures(1, &ctex);   
	GET_GLERROR(0);
//	for(int i = 0; i < tw; i++)
//		cout << ug.GPUcellInfo[i*3] << " " << ug.GPUcellInfo[i*3+1] << " " << ug.GPUcellInfo[i*3+2] << endl;
//	cout << ply_loader.m_iTriangles << endl;
	glBindTexture(texTarget, ctex);	
	glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	// int array is NONO!!!, please make sure all array are float when doing this
	glTexImage2D(texTarget, 0, texInternalFormat, tw
		, th, 0, GL_RGB, GL_FLOAT, ug.GPUcellInfo);
	GET_GLERROR(0);
	
	tw = numtris>TEXTURELIMIT?TEXTURELIMIT:numtris;
	th = numtris*INVTL+1;
    std::cout << "vert texture " << tw << " " << th;
    std::cout << " using : " << GetTexMemUsage(tw,th)*3 << " MB ( all 3)\n";
	glGenTextures(3, vtex);
	GET_GLERROR(0);
	
	glBindTexture(texTarget, vtex[0]);	
	glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(texTarget, 0, texInternalFormat, tw, th, 0, GL_RGB, GL_FLOAT, ug.verts0);

	glBindTexture(texTarget, vtex[1]);	
	glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(texTarget, 0, texInternalFormat, tw, th, 0, GL_RGB, GL_FLOAT, ug.verts1);

	glBindTexture(texTarget, vtex[2]);	
	glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(texTarget, 0, GL_RGB32F_ARB, tw, th, 0, GL_RGB, GL_FLOAT, ug.verts2);

	glGenTextures(3, ntex);
	GET_GLERROR(0);
	
	glBindTexture(texTarget, ntex[0]);	
	glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(texTarget, 0, texInternalFormat, tw, th, 0, GL_RGB, GL_FLOAT, ug.norms0);

	glBindTexture(texTarget, ntex[1]);	
	glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(texTarget, 0, texInternalFormat, tw, th, 0, GL_RGB, GL_FLOAT, ug.norms1);

	glBindTexture(texTarget, ntex[2]);	
	glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(texTarget, 0, texInternalFormat, tw, th, 0, GL_RGB, GL_FLOAT, ug.norms2);

  printf("time used to built+load grid texture: %f\n", (float)(GetTickCount()-t1));
    */

}


void PhotonMapperGPU::CleanupRayTracer() 
{	
   
  // destroy objects
  glDeleteTextures(10, tex);
  glDeleteTextures(3, vtex);
  glDeleteTextures(3, ntex);
  glDeleteTextures(1, &ctex);
   
}


void PhotonMapperGPU::RaytraceFrame(Scene inScn) 
{
    // Setup OpenGL
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_DEPTH_TEST);
	glViewport(0,0, m_pixelRows, m_pixelCols);
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

    // render to texture
    m_FBO.Bind();    

    m_FBO.AttachTexture(ZERO, tex[0]);
    m_FBO.AttachTexture(ONE, tex[1]);
    m_FBO.AttachTexture(TWO, tex[2]);

    glClearColor(0,0,0,0);

    m_FBO.ClearAttachment(ONE);
    m_FBO.ClearAttachment(TWO);
    
    m_FBO.AttachTexture(THREE, tex[4]);

    m_FBO.AddDrawBuffer(THREE); // eye start
    m_FBO.AddDrawBuffer(ZERO);  // eye dir
    m_FBO.DrawBuffers();

    EyeKernel();		

    m_FBO.AttachTexture(THREE, tex[3]);

    m_FBO.AddDrawBuffer(ONE);   // voxel
    m_FBO.AddDrawBuffer(THREE); // tmax
    m_FBO.AddDrawBuffer(TWO);   // hitinfo
    m_FBO.DrawBuffers();

    // return 2 texture: voxel, tmax
    TIKernel();		
    if(!m_Done)
    {
        if(m_PrintTIData)
        {
            printTextureValues(tex[3]);
        }
        m_Done = true;
    }

    m_FBO.AttachTexture(ONE, tex[6]);
    m_FBO.AttachTexture(TWO, tex[7]);

    m_FBO.AddDrawBuffer(THREE); // overwrite tmax to color
    m_FBO.AddDrawBuffer(ONE);   // normal
    m_FBO.AddDrawBuffer(TWO);   // hit position
    m_FBO.DrawBuffers();
    
    ShadeKernel();

    doshadow = false;
    if(doshadow)
    {
        m_FBO.AttachTexture(ZERO, tex[4]);
        m_FBO.AttachTexture(THREE, tex[0]);
        
        m_FBO.AddDrawBuffer(ZERO);  // ray start
        m_FBO.AddDrawBuffer(THREE); // ray dir
        m_FBO.DrawBuffers();

        Sec_RayKernel(SHADOW);
        // shadow start in tex[4] , shadow ray in tex[5]

        m_FBO.AttachTexture(ONE, tex[1]);
        m_FBO.AttachTexture(TWO, tex[2]);
        m_FBO.AttachTexture(THREE, tex[8]);
              
        m_FBO.AddDrawBuffer(ONE);    // voxel
        m_FBO.AddDrawBuffer(THREE);  // tmax
        m_FBO.AddDrawBuffer(TWO);    // hitinfo
        m_FBO.DrawBuffers();

        // return 2 texture: voxel, tmax
        TIKernel();	

        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT3_EXT, texTarget, tex[3], 0);

        m_FBO.DrawBuffer(THREE);

        ShadowKernel();
    }

    // render to screen
    m_FBO.Disable();
    DisplayKernel();

    glFinish();
    glutSwapBuffers();
    glPopAttrib();
}


void PhotonMapperGPU::ShadowKernel()
{
#ifndef STUB_PHOTONMAPPERGPU
    CShader& shadowShader = m_ShaderManager->GetShader(SHADOW);

    CGparameter& hitinfo = shadowShader.GetNamedParameter("hitinfo", true);
    cgGLSetTextureParameter(hitinfo, tex[2]);

    CGparameter& phong = shadowShader.GetNamedParameter("phong", true);
	cgGLSetTextureParameter(phong, tex[3]);

    CGparameter& Ia = shadowShader.GetNamedParameter("ambient");
	cgGLSetParameter4f(Ia, .1,.1,.1,1);

    shadowShader.EnableTextureParameters();
    shadowShader.Bind();
    shadowShader.Enable();

	Workspace();

    shadowShader.Disable();
    shadowShader.DisableTextureParameters();   
#endif // STUB_PHOTONMAPPERGPU
}

void PhotonMapperGPU::Sec_RayKernel(RAYTYPE type)
{
#ifndef STUB_PHOTONMAPPERGPU
    CShader &rayGenerator = m_ShaderManager->GetShader(RAY_GENERATOR);

    CGparameter& raydir = rayGenerator.GetNamedParameter("raydir", true);
	cgGLSetTextureParameter(raydir, tex[0]);
    CGparameter& cgnormal = rayGenerator.GetNamedParameter("normal", true);
	cgGLSetTextureParameter(cgnormal, tex[6]);
    CGparameter& hitpos = rayGenerator.GetNamedParameter("hitp", true);
	cgGLSetTextureParameter(hitpos, tex[7]);
    	
    rayGenerator.EnableTextureParameters();

    Point3f light(0,50,50);

    CGparameter& L =  rayGenerator.GetNamedParameter("lpos");
	cgGLSetParameter4f(L, light.X(), light.Y(), light.Z(), 0);
    CGparameter& raytype = rayGenerator.GetNamedParameter("type");
    cgGLSetParameter1f(raytype, type);
    CGparameter& refracn = rayGenerator.GetNamedParameter("nn");
    cgGLSetParameter1f(refracn, 1);


	rayGenerator.Bind();
    rayGenerator.Enable();
    
    Workspace();
	
    rayGenerator.Disable();
    rayGenerator.DisableTextureParameters();
#endif // STUB_PHOTONMAPPERGPU   
}


void PhotonMapperGPU::DisplayKernel()
{
#ifndef STUB_PHOTONMAPPERGPU
    CShader &displayShader = m_ShaderManager->GetShader(DISPLAY);
    
    CGparameter& pictureout = displayShader.GetNamedParameter("data", true);
	// tex[0] = eye ray dir
	// tex[1] = voxel
	// tex[2] = hitinfo
	// tex[3] = tmax -> color
	// tex[4] = s ray start
	// tex[5] = s ray dir
	// tex[6] = normal
	// tex[7] = hitpos
    displaytype = 3;
	switch(displaytype)
	{
		case 1:	cgGLSetTextureParameter(pictureout, tex[1]); break;//glutSetWindowTitle("grid info"); break; // grid
		case 2:	cgGLSetTextureParameter(pictureout, tex[2]); break;//glutSetWindowTitle("hit time"); break; // hittime
		case 3: cgGLSetTextureParameter(pictureout, tex[3]); break;//glutSetWindowTitle("phong"); break; // phong
		case 4: cgGLSetTextureParameter(pictureout, tex[4]); break;//glutSetWindowTitle("ray start");break; // ray start
		case 0: cgGLSetTextureParameter(pictureout, tex[0]); break;//glutSetWindowTitle("ray dir");break; // ray dir
		case 6: cgGLSetTextureParameter(pictureout, tex[6]); break;//glutSetWindowTitle("normal");break; // normal
		case 7: cgGLSetTextureParameter(pictureout, tex[7]); break;//glutSetWindowTitle("hit position");break; // hitpos
	}

    displayShader.EnableTextureParameters();
    displayShader.Bind();
    displayShader.Enable();
    Workspace();
    displayShader.Disable();
    displayShader.DisableTextureParameters(); 


    int texSize = texWidth * texHeight * 4; 
    GLfloat *texData = new GLfloat[texSize];

    glBindTexture(texTarget, tex[0]);
    glEnable(texTarget);
    glGetTexImage(texTarget, 0, GL_RGBA, GL_FLOAT, texData);
    glDisable(texTarget);

    TextureGLf image(GL_FLOAT, texTarget, texWidth, texHeight, texData);

    CTextureManager::WriteTextureToPNG("display.png", &image);

#endif // STUB_PHOTONMAPPERGPU
}



// kernal that produce ray direction texture
void PhotonMapperGPU::EyeKernel()
{
#ifndef STUB_PHOTONMAPPERGPU
	// figure out necessary eye vectors
	Vector3f nn, vv, uu;
//	nn.set(eyePos.x - look.x, eyePos.y - look.y, eyePos.z - look.z); // make n
//	uu.set(up.cross(nn)); // make u = up X n
//	nn.normalize(); uu.normalize(); // make them unit length
//	vv.set(nn.cross(uu));  // make v =  n X u
 

    nn = m_pCamera->N();
    vv = m_pCamera->V();
    uu = m_pCamera->U();
    Point3f eyePos = m_pCamera->GetEye();
	// firgure out world width and height
    float sH = m_pCamera->GetNearClipPlane() * tan( m_pCamera->GetViewAngle() * (3.1415/360));
    float sW = sH * m_pCamera->GetAspect();

//	cout << eyePos.x << " " << eyePos.y << " " << eyePos.z << endl;

//	cout << sH << " " << sW << endl;

    CShader& ray = m_ShaderManager->GetShader(RAY);

    CGparameter& n = ray.GetNamedParameter("IN2.n");
	cgGLSetParameter3f(n, nn.X(), nn.Y(), nn.Z());

    CGparameter& v = ray.GetNamedParameter("IN2.v");
	cgGLSetParameter3f(v, vv.X(), vv.Y(), vv.Z());

    CGparameter& u = ray.GetNamedParameter("IN2.u");
	cgGLSetParameter3f(u, uu.X(), uu.Y(), uu.Z());

    CGparameter& view = ray.GetNamedParameter("IN2.view");
	cgGLSetParameter2f(view, m_pixelCols, m_pixelRows);

    CGparameter& world = ray.GetNamedParameter("IN2.world");
	cgGLSetParameter2f(world, sW, sH);
    
    CGparameter& nearDist = ray.GetNamedParameter("IN2.nearDist");
    cgGLSetParameter1f(nearDist, m_pCamera->GetNearClipPlane());

    CGparameter& eye = ray.GetNamedParameter("eyePos");
	cgGLSetParameter3f(eye, eyePos.X(), eyePos.Y(), eyePos.Z());
	
	ray.Bind();
    ray.Enable();

	Workspace();
	
    ray.Disable();
#endif // STUB_PHOTONMAPPERGPU
}


void PhotonMapperGPU::TIKernel()
{
    // temporarily commented out for integration -- brandon
	/*
	CShader& traverser = m_ShaderManager->GetShader(NEW_TRAVERSER);

	CGparameter& raydir = traverser.GetNamedParameter("raydir", true);
	cgGLSetTextureParameter(raydir, tex[0]);
	CGparameter& cell_info = traverser.GetNamedParameter("cell_info", true);
	cgGLSetTextureParameter(cell_info, ctex);
	CGparameter& v0 = traverser.GetNamedParameter("v0t", true);
	cgGLSetTextureParameter(v0, vtex[0]);
	CGparameter& v1 = traverser.GetNamedParameter("v1t", true);
	cgGLSetTextureParameter(v1, vtex[1]);
	CGparameter& v2 = traverser.GetNamedParameter("v2t", true);
	cgGLSetTextureParameter(v2, vtex[2]);
	CGparameter& eye = traverser.GetNamedParameter("eye", true);
	cgGLSetTextureParameter(eye, tex[4]);

	traverser.EnableTextureParameters();
	
	CGparameter& len = traverser.GetNamedParameter("len");
	cgGLSetParameter1f(len, ug.cell_size);
	
    GpuTraverseLoop = 1024;
    CGparameter& maxloop = traverser.GetNamedParameter("maxloop");
	cgGLSetParameter1f(maxloop, GpuTraverseLoop);
	
    CGparameter& gridsize = traverser.GetNamedParameter("gridsize");
	cgGLSetParameter1f(gridsize, ug.num_Cells);
	
    CGparameter& gmin = traverser.GetNamedParameter("gmin");
	cgGLSetParameter3f(gmin, ug.minPt.X(), ug.minPt.Y(), ug.minPt.Z());
	
    CGparameter& gmax = traverser.GetNamedParameter("gmax");
	cgGLSetParameter3f(gmax, ug.maxPt.X(), ug.maxPt.Y(), ug.maxPt.Z());
	
	traverser.Bind();
	traverser.Enable();

	Workspace();

	traverser.Disable();
	traverser.DisableTextureParameters();
    */
}



void PhotonMapperGPU::ShadeKernel()
{
#ifndef STUB_PHOTONMAPPERGPU
    CShader &phong = m_ShaderManager->GetShader(PHONG);

    CGparameter& raydir = phong.GetNamedParameter("raydir", true);
	cgGLSetTextureParameter(raydir, tex[0]);

    CGparameter& hitinfo = phong.GetNamedParameter("hitinfo", true);
	cgGLSetTextureParameter(hitinfo, tex[2]);

    CGparameter& n0 = phong.GetNamedParameter("TRI_N.n0", true);
	cgGLSetTextureParameter(n0, ntex[0]);

    CGparameter& n1 = phong.GetNamedParameter("TRI_N.n1", true);
	cgGLSetTextureParameter(n1, ntex[1]);

    CGparameter& n2 = phong.GetNamedParameter("TRI_N.n2", true);
	cgGLSetTextureParameter(n2, ntex[2]);

    CGparameter& eye = phong.GetNamedParameter("eyePos", true);
	cgGLSetTextureParameter(eye, tex[4]);


    phong.EnableTextureParameters();

    CGparameter& Ia = phong.GetNamedParameter("IN2.Ia");
	cgGLSetParameter4f(Ia, .1,.1,.1,1);
    CGparameter& Id = phong.GetNamedParameter("IN2.Id");
	cgGLSetParameter4f(Id, .7,0,.7 ,1);
    CGparameter& Is = phong.GetNamedParameter("IN2.Is");
	cgGLSetParameter4f(Is, .8,.8,.8,1);
    CGparameter& f = phong.GetNamedParameter("IN2.f");
	cgGLSetParameter1f(f, 12);

    Point3f light(0,50,50);
    CGparameter& L = phong.GetNamedParameter("IN2.L");
	cgGLSetParameter4f(L, light.X(), light.Y(), light.Z(), 0);
    CGparameter& Lc = phong.GetNamedParameter("IN2.Lc");
	cgGLSetParameter4f(Lc, 1,1,1,1);

	phong.Bind();
    phong.Enable();

	Workspace();

    phong.Disable();
    phong.DisableTextureParameters();

//	cout << "Shading Complete!!\n";
#endif // STUB_PHOTONMAPPERGPU
}

void PhotonMapperGPU::Workspace()
{
	glBegin(GL_QUADS);
    {
        glTexCoord2f(0, 0);					glVertex2f(-1, -1);
        glTexCoord2f(maxCoordS, 0);			glVertex2f( 1, -1);
        glTexCoord2f(maxCoordS, maxCoordT); glVertex2f( 1,  1);
        glTexCoord2f(0, maxCoordT);			glVertex2f(-1,  1);
    }
    glEnd();
}


double PhotonMapperGPU::GetTexMemUsage(int w, int h)
{
	return w*h*1.52587891e-5;
}


void PhotonMapperGPU::printTextureValues(GLenum texture)
{
    //printf("saving traverse info to something, please wait\n", TraverseDataFile);
    GLint twidth;
    GLint theight;
    GLint level;
    glBindTexture(texTarget, texture);

    std::ofstream myfile;
    myfile.open ("t01.txt");
    // get base mipmap level, texture width and height
   glGetTexParameteriv(texTarget, GL_TEXTURE_BASE_LEVEL, &level);
        glGetTexLevelParameteriv(texTarget, level, GL_TEXTURE_WIDTH, &twidth);
        glGetTexLevelParameteriv(texTarget, level, GL_TEXTURE_HEIGHT, &theight);
       
        // determine how much space we need in memory for the texture
        int size = twidth*theight*4;
        GLfloat* pixels = (GLfloat*) malloc(size*sizeof(GLfloat));
       
        // initialize that space to a value we know
        for (int s=0;s<size;s++)        pixels[s] = -2;

        // get texture information
        glGetTexImage(texTarget, level, GL_RGBA, GL_FLOAT, pixels);
        //GET_GLERROR(0);

        // print it out
        unsigned int x,y,z;
        float w;
        unsigned int ct = 0;
        unsigned int ctt = 0;
        unsigned int ci = 0;
        unsigned int hit = 0;
        for (int i = 0; i<size/4; i++){
          x = (unsigned int) pixels[i*4];
          y = (unsigned int) pixels[i*4+1];
          z = (unsigned int) pixels[i*4+2];
          w = pixels[i*4+3];
          myfile << i%theight << " " << i/twidth << " total traverse " << x+y << " traverse-empty: " 
            << x << " traverse-hit " << y << " intersection test: " << z;
                if(w > 0)
                {
                  hit++;
                  myfile << " hit\n";
                }
                else
                    myfile << std::endl;
          ct += x;
          ctt += y;
          ci += z;
        }
       
        myfile << "TOTAL TRAVERSE: " << ct+ctt << std::endl;
        myfile << "TOTAL TRAVERSE-EMPTY: " << ct << std::endl;
        myfile << "TOTAL TRAVERSE-HIT: " << ctt << std::endl;
        myfile << "TOTAL INTERSECTION: " << ci << std::endl;
        myfile << "TOTAL VALID HIT FOUND: " << hit << std::endl;
        myfile << "my gridsize is " << 20 << std::endl;
        myfile.close();
        // clean up
        delete [] pixels;
        printf("done parsing traverse info\n");       
} 


