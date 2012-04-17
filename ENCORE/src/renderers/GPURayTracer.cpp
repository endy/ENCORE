#include "GPURayTracer.h"
#include <cassert>

#define STUB 1

#ifndef STUB

#define GLH_EXT_SINGLE_FILE
#include "GLEW/glew.h"

void GPURayTracer::printTextureValues(GLenum texture)
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
    memset( pixels, -2, size );
    // initialize that space to a value we know

    // get texture information
    glGetTexImage(texTarget, level, GL_RGBA, GL_FLOAT, pixels);
    //GET_GLERROR(0);

    // print it out
    GLfloat x,y,z;
    GLfloat w;
    for (int i = 0; i<size/4; i++){
        x = pixels[i*4];
        y = pixels[i*4+1];
        z = pixels[i*4+2];
        w = pixels[i*4+3];
        myfile << i%theight << " " << i/twidth << "\tx " << x << "\ty " 
        << y << "\tz " << z << "\tw " << w;
        myfile << std::endl;
    }
    
    myfile.close();
    // clean up
    delete [] pixels;
//    printf("done printing texture values\n");       
} 


GPURayTracer::GPURayTracer(void):
    m_pShaderManager(NULL)
{
    if ( !glh_init_extensions( "GL_NV_occlusion_query " "GL_ARB_occlusion_query " ) ) 
    {
        printf("Unable to load the following extension(s): %s\n\nExiting...\n", glh_get_unsupported_extensions());
        assert( !"ERROR" );
    }
    glGenQueriesARB(1, &m_glQuery);
}

GPURayTracer::~GPURayTracer(void)
{
    delete triangleList;
    glh_shutdown_extensions();
    glDeleteQueriesARB(1, &m_glQuery);
}

void GPURayTracer::usage( void )
{
}

void GPURayTracer::init( Scene* l_pScene, AccelerationStructure* l_pAccelStruct, Camera* l_pCamera )
{
    triangleList = 0;
    m_pScene = l_pScene;
    m_pAccelStruct = l_pAccelStruct;
    m_pCamera = l_pCamera;

    m_pixelCols = l_pCamera->GetViewportWidth();
    m_pixelRows = l_pCamera->GetViewportHeight();

    //std::list<IModel*> modelList = m_pScene->getModelList();
    triangleList = m_pScene->getNewTesselation();
    // move to render
    m_pAccelStruct->buildGPU(m_pScene->getModelList(), *triangleList, m_ASData );

    // cleanup triangle list memory
    /*
    std::list<Triangle*>::iterator triIter = triangleList->begin();
    while(triIter != triangleList->end())
    {
        if(*triIter != NULL)
        {
            Triangle *tri =  *triIter;
        }
        ++triIter;
    }
    delete triangleList;
    
    */
    m_ASData.generateTextures();
    
    init_FBOs();
    init_shaders();
    
}

void GPURayTracer::configure( Options* l_pOptions )
{
}

void GPURayTracer::render( void )
{
    //m_pAccelStruct->buildGPU(m_pScene->getModelList(), *triangleList, m_ASData );
    //m_ASData.generateTextures();
    // Setup OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glViewport(0,0, m_pixelCols, m_pixelRows);
    glDisable( GL_DEPTH_TEST );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);


    // tex[0] =     Ray Start
    // tex[1] =     Eye Ray Dir -> Shadow Ray
    // tex[2] =     Reflected Ray Dir
    // tex[3] =     Refracted Ray Dir
    // tex[4] =     Hit info
    // tex[5] =     tMax (traverseInfo1)
    // tex[6] =     Voxel(traverseInfo2)
    // tex[7] =     ?

    // render to texture
    m_FBO.Bind();

    // pass 1:
    // in: nothing
    // out: ray start and ray dir

    m_FBO.AttachTexture(ZERO, tex[0]);
    m_FBO.AttachTexture(ONE, tex[1]);

    m_FBO.AddDrawBuffer(ZERO);  // Ray Start
    m_FBO.AddDrawBuffer(ONE);   // Ray Direction
    m_FBO.DrawBuffers();

    EyeKernel();

    // pass 2:
    // in: ray start and ray dir
    // out: hit info, tmax, and voxel

    m_FBO.AttachTexture(ZERO, tex[4]);  // hit info
    glClearColor( 6000000000, 0, 0, -1 ); // seed maxHitTime very high
    m_FBO.ClearAttachment(ZERO);

    m_FBO.AttachTexture(TWO, tex[5]);   // traverseData1
    m_FBO.AttachTexture(THREE, tex[6]); // traverseData2
    glClearColor( 0, 0, 0, 0 );
    m_FBO.ClearAttachment(TWO);
    m_FBO.ClearAttachment(THREE);

//    m_FBO.AttachTexture(THREE, tex[6]); // Voxel

    m_FBO.AddDrawBuffer(ZERO);
    m_FBO.AddDrawBuffer(TWO);
    m_FBO.AddDrawBuffer(THREE);

    m_FBO.DrawBuffers();

    TIKernel();

    // Pass 3
    // in: hit info
    // out: color
    m_FBO.Disable();
    ShadeKernel();

    // render to screen
//    m_FBO.Disable();
//    DisplayKernel();

    glFinish();
    glutSwapBuffers();
    glPopAttrib();
}

void GPURayTracer::deinit( void )
{
    glDeleteTextures(10, tex);

    if (m_pShaderManager != NULL )
    {
        delete m_pShaderManager;
        m_pShaderManager = NULL;
    }
}

void GPURayTracer::keyboard( unsigned char key )
{
}

/*****************************************************
* PRIVATE MEMBER FUNCTIONS
*****************************************************/

/*************
*  initFBOs  *
*************/
void GPURayTracer::init_FBOs()
{
    ////  INIT FRAME BUFFER OBJECT  ////////////////////////////////////////

    glGenTextures(10, tex);

    filterMode = (texTarget == GL_TEXTURE_RECTANGLE_ARB) ? GL_NEAREST : GL_LINEAR;

    maxCoordS = (texTarget == GL_TEXTURE_RECTANGLE_ARB) ? m_pixelCols : 1;
    maxCoordT = (texTarget == GL_TEXTURE_RECTANGLE_ARB) ? m_pixelRows : 1;

    // init texture
    int i;
    for(i=0; i < 10; i++)
    {
        glBindTexture(texTarget, tex[i]);
        glTexImage2D(texTarget, 0, texInternalFormat, m_pixelCols, m_pixelRows, 0, GL_RGBA, GL_FLOAT, 0);
        GET_GLERROR(0);
        glTexParameterf(texTarget, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameterf(texTarget, GL_TEXTURE_MAG_FILTER, filterMode);
        glTexParameterf(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    
    }

    m_FBO.Initalize(GL_FRAMEBUFFER_EXT, texTarget);
}

void GPURayTracer::Workspace(){
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0, 0);                 glVertex2f(-1, -1);
        glTexCoord2f(maxCoordS, 0);         glVertex2f( 1, -1);
        glTexCoord2f(maxCoordS, maxCoordT); glVertex2f( 1,  1);
        glTexCoord2f(0, maxCoordT);         glVertex2f(-1,  1);
    }
    glEnd();
}

void GPURayTracer::init_shaders()
{
    /* Create and load the required Shader
     * then get handle to each parameter so that
     * we can change them within our code
     */

    m_pShaderManager = new CShaderManager();

    m_pShaderManager->LoadVertexShader( VS_EYE_RAY_GENERATOR, "shaders\\pjl\\eyeRayGeneratorVP.cg" );
    m_pShaderManager->LoadFragmentShader( FS_EYE_RAY_GENERATOR, "shaders\\pjl\\eyeRayGeneratorFP.cg" );
    m_pShaderManager->LoadFragmentShader( FS_TRAVERSER, m_ASData.m_strTraversalShaderFilename );
    m_pShaderManager->LoadFragmentShader( FS_PHONG, "shaders\\phong_shader.cg" );
//  m_pShaderManager->LoadFragmentShader( FS_DISPLAY, "shaders\\pjl\\display.cg");

    std::cout << "Compiled all shaders" << std::endl;
}

void GPURayTracer::EyeKernel()
{
    // need to generate ray start and ray direction 
    // using vertex shader
    Vector3f nn, vv, uu;
    uu = m_pCamera->U();
    vv = m_pCamera->V();
    nn = m_pCamera->N();

    CShader* ray = &(m_pShaderManager->GetShader(FS_EYE_RAY_GENERATOR));

    CGparameter& u = ray->GetNamedParameter("u");
    cgGLSetParameter3f(u, uu.X(), uu.Y(), uu.Z());

    CGparameter& v = ray->GetNamedParameter("v");
    cgGLSetParameter3f(v, vv.X(), vv.Y(), vv.Z());

    CGparameter& n = ray->GetNamedParameter("n");
    cgGLSetParameter3f(n, nn.X(), nn.Y(), nn.Z());

    CGparameter& view = ray->GetNamedParameter("viewport");
    cgGLSetParameter2f(view, m_pCamera->GetViewportWidth(),m_pCamera->GetViewportHeight());

    CGparameter& world = ray->GetNamedParameter("worldwindow");
    cgGLSetParameter2f(world, m_pCamera->GetWorldWidth(), m_pCamera->GetWorldHeight());

    CGparameter& nearDist = ray->GetNamedParameter("nearDist");
    cgGLSetParameter1f(nearDist, m_pCamera->GetNearClipPlane());

    CGparameter& eye = ray->GetNamedParameter("eyePosition");
    cgGLSetParameter3f(eye, m_pCamera->GetEye().X(), m_pCamera->GetEye().Y(), m_pCamera->GetEye().Z());

    ray->Bind();
    ray->Enable();

    Workspace();

    ray->Disable();
}

void GPURayTracer::DisplayKernel()
{
    CShader &displayShader = m_pShaderManager->GetShader(FS_DISPLAY);
    
    CGparameter& pictureout = displayShader.GetNamedParameter("data", true);
    // tex[0] = eye ray dir
    // tex[1] = voxel
    // tex[2] = hitinfo
    // tex[3] = tmax -> color
    // tex[4] = s ray start
    // tex[5] = s ray dir
    // tex[6] = normal
    // tex[7] = hitpos
    int displaytype = 1;
    switch(displaytype)
    {
        case 1: cgGLSetTextureParameter(pictureout, tex[1]); break;//glutSetWindowTitle("grid info"); break; // grid
        case 2: cgGLSetTextureParameter(pictureout, tex[2]); break;//glutSetWindowTitle("hit time"); break; // hittime
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
}


void GPURayTracer::TIKernel()
{
    bool bFirstPass = false;
    CShader* ray;

    if ( bFirstPass )
    {
        // need to generate ray start and ray direction 
        // using vertex shader
        Vector3f nn, vv, uu;
        nn = m_pCamera->N();
        vv = m_pCamera->V();
        uu = m_pCamera->U();

        ray = &(m_pShaderManager->GetShader( VS_EYE_RAY_GENERATOR ));

        CGparameter& n = ray->GetNamedParameter("n");
        cgGLSetParameter3f(n, nn.X(), nn.Y(), nn.Z());

        CGparameter& v = ray->GetNamedParameter("v");
        cgGLSetParameter3f(v, vv.X(), vv.Y(), vv.Z());

        CGparameter& u = ray->GetNamedParameter("u");
        cgGLSetParameter3f(u, uu.X(), uu.Y(), uu.Z());

        CGparameter& view = ray->GetNamedParameter("viewport");
        cgGLSetParameter2f(view, m_pCamera->GetViewportWidth(),m_pCamera->GetViewportHeight());

        CGparameter& world = ray->GetNamedParameter("worldwindow");
        cgGLSetParameter2f(world, m_pCamera->GetWorldWidth(), m_pCamera->GetWorldHeight());

        CGparameter& nearDist = ray->GetNamedParameter("nearDist");
        cgGLSetParameter1f(nearDist, m_pCamera->GetNearClipPlane());

        CGparameter& eye = ray->GetNamedParameter("eyePos");
        cgGLSetParameter3f(eye, m_pCamera->GetEye().X(), m_pCamera->GetEye().Y(), m_pCamera->GetEye().Z());

        ray->Bind();
        ray->Enable();
    }

    // now set the pixel shader to traverse
    CShader& traverser = m_pShaderManager->GetShader( FS_TRAVERSER );

    // set AccelStruct GPU parameters
    m_pAccelStruct->setGPUParameters( traverser, m_ASData );

    // set ray-specific parameters
    CGparameter& raystartmap = traverser.GetNamedParameter("rayStartMap", true);
    cgGLSetTextureParameter( raystartmap, tex[0] );

    CGparameter& raydirmap = traverser.GetNamedParameter("rayDirMap", true);
    cgGLSetTextureParameter( raydirmap, tex[1] );

    CGparameter& hitinfomap = traverser.GetNamedParameter("hitInfoMap", true);
    cgGLSetTextureParameter( hitinfomap, tex[4] );

    CGparameter& trav0map = traverser.GetNamedParameter("trav0Map", true);
    cgGLSetTextureParameter( trav0map, tex[5] );

    CGparameter& trav1map = traverser.GetNamedParameter("trav1Map", true);
    cgGLSetTextureParameter( trav1map, tex[6] );

    traverser.EnableTextureParameters();

    traverser.Bind();
    traverser.Enable();

//  THE FOLLOWING CODE CAN BE UNCOMMENTED IF IT IS TAKEN OUT OF THE RENDER FUNCTION
//  THIS WILL AVOID DOING EXTRA TEXTURE LOADS AND SHADER LOADS
//    Workspace();

    unsigned int count = 1;
    const unsigned int uiMaxShaderRuns = 100;
    unsigned int uiShaderRuns = 0;
    while ( count > 0 && uiShaderRuns <= uiMaxShaderRuns )
    {
        // begin occlusion query
        glBeginQueryARB(GL_SAMPLES_PASSED_ARB, m_glQuery);

        Workspace();

        // end occlusion query and get result
        glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        glGetQueryObjectuivARB(m_glQuery, GL_QUERY_RESULT_ARB, &count);
//printf( "Wrote %d pixels\n", count );
        uiShaderRuns++;
    }

//printf("Ran Shader %d times\n", uiShaderRuns );

    traverser.Disable();
    traverser.DisableTextureParameters();

    if ( bFirstPass )
    {
        ray->Disable();
        bFirstPass = false;
    }
}



void GPURayTracer::ShadeKernel()
{
    
    CShader &phong = m_pShaderManager->GetShader(FS_PHONG);

    CGparameter& eye = phong.GetNamedParameter("eyePos", true);
    cgGLSetTextureParameter(eye, tex[0]);

    CGparameter& raydir = phong.GetNamedParameter("raydir", true);
    cgGLSetTextureParameter(raydir, tex[1]);

    CGparameter& hitinfo = phong.GetNamedParameter("hitinfo", true);
    cgGLSetTextureParameter(hitinfo, tex[4]);

    CGparameter& n0 = phong.GetNamedParameter("TRI_N.n0", true);
    cgGLSetTextureParameter(n0, m_ASData.m_NormalTexture[0]);

    CGparameter& n1 = phong.GetNamedParameter("TRI_N.n1", true);
    cgGLSetTextureParameter(n1, m_ASData.m_NormalTexture[1]);

    CGparameter& n2 = phong.GetNamedParameter("TRI_N.n2", true);
    cgGLSetTextureParameter(n2, m_ASData.m_NormalTexture[2]);

    CGparameter& mat = phong.GetNamedParameter("material", true);
    cgGLSetTextureParameter(mat, m_ASData.m_MaterialTexture[0]);

    phong.EnableTextureParameters();

    CGparameter& Ia = phong.GetNamedParameter("IN2.Ia");
    cgGLSetParameter4f(Ia, 0,0,0,0);
    CGparameter& Id = phong.GetNamedParameter("IN2.Id");
    cgGLSetParameter4f(Id, .1,1,.1 ,1);
    CGparameter& Is = phong.GetNamedParameter("IN2.Is");
    cgGLSetParameter4f(Is, 1.0,1.0,1.0,1);
    CGparameter& f = phong.GetNamedParameter("IN2.f");
    cgGLSetParameter1f(f, 32);

    Color backgroundColor = m_pScene->getBackgroundColor();
    CGparameter& bgColor = phong.GetNamedParameter("backgroundColor");
    cgGLSetParameter4f(bgColor, backgroundColor.R(), backgroundColor.G(), backgroundColor.B(), backgroundColor.A() );

    IEmissive* pLight = *(m_pScene->getLightList()->begin());
    Color lightColor = pLight->GetEmissiveColor();
    Point3f lightPos = pLight->GetPointOfEmission();

    CGparameter& L = phong.GetNamedParameter("IN2.L");
    cgGLSetParameter4f( L, lightPos.X(), lightPos.Y(), lightPos.Z(), 1 ); 
    CGparameter& Lc = phong.GetNamedParameter("IN2.Lc");
    cgGLSetParameter4f(Lc, lightColor.R(), lightColor.G(), lightColor.B(), lightColor.A() ); 

    phong.Bind();
    phong.Enable();

    Workspace();

    phong.Disable();
    phong.DisableTextureParameters();

//  cout << "Shading Complete!!\n";
    
}
#endif // STUB
