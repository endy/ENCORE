#pragma once

#include "RayTracer.h"
#include "Tonemap.h"

#ifdef WIN32
#include <Windows.h>
#include <GL\glut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "TextureManager.h"
#include "Primitive.h"

#ifndef WIN32
#define GetTickCount() 0
#define DWORD unsigned int
#endif

#include "Singleton.h"
#include "Logfile.h"

RayTracer::RayTracer(int pixelRows, int pixelCols)
{
    m_BucketRows = 1;
    m_BucketCols = 1;
    m_StartBucketRow = 0;
    m_EndBucketRow = m_BucketRows;
    m_StartBucketCol = 0;
    m_EndBucketCol = m_BucketCols;

    m_pixelRows = pixelRows; 
    m_pixelCols = pixelCols;

    m_RaysPerPixel = 1;
    m_BlockSize = 1;
    m_RecurseLevel = 3;

    m_ImageMaker = new TextureMaker(m_pixelCols, m_pixelRows);

    m_Image = NULL;

    m_RenderTime = 0;

    
    m_view = PHONG; ///@todo remove
}

RayTracer::~RayTracer()
{
    if(m_Image != NULL)
    {
        delete m_Image;
        m_Image = NULL;
    }

    if(m_ImageMaker != NULL)
    {
        delete m_ImageMaker;
    }
}

/////  Renderer Interface  ////////////////////////////////////////////

void RayTracer::usage()
{
    std::cout << "No Usage Defined for RayTracer" << std::endl;
}

void RayTracer::configure( Options* pOptions )
{
    Singleton<Logfile>::Get().Stream() << "----------- CONFIGURATION: Ray Tracer -------------" << std::endl;

    if ( pOptions->doesOptionExist( "BucketRows" ) ) 
    { 
        m_BucketRows = atoi(pOptions->getOption("BucketRows" ).c_str()); 
        pOptions->LogOption("BucketRows", &Singleton<Logfile>::Get().Stream()); 
    }

    if ( pOptions->doesOptionExist( "BucketCols" ) ) 
    { 
        m_BucketCols = atoi(pOptions->getOption("BucketCols" ).c_str()); 
        pOptions->LogOption("BucketCols", &Singleton<Logfile>::Get().Stream()); 
    }

    if ( pOptions->doesOptionExist( "StartBucketRow" ) ) 
    { 
        m_StartBucketRow = atoi(pOptions->getOption("StartBucketRow" ).c_str()); 
        pOptions->LogOption("StartBucketRow", &Singleton<Logfile>::Get().Stream()); 
    }

    if ( pOptions->doesOptionExist( "EndBucketRow" ) ) 
    { 
        m_EndBucketRow = atoi(pOptions->getOption("EndBucketRow" ).c_str()); 
        pOptions->LogOption("EndBucketRow", &Singleton<Logfile>::Get().Stream()); 
    }

    if ( pOptions->doesOptionExist( "StartBucketCol" ) ) 
    { 
        m_StartBucketCol = atoi(pOptions->getOption("StartBucketCol" ).c_str()); 
        pOptions->LogOption("StartBucketCol", &Singleton<Logfile>::Get().Stream()); 
    }

    if ( pOptions->doesOptionExist( "EndBucketCol" ) ) 
    { 
        m_EndBucketCol = atoi(pOptions->getOption("EndBucketCol" ).c_str()); 
        pOptions->LogOption("EndBucketCol", &Singleton<Logfile>::Get().Stream()); 
    }

    if ( pOptions->doesOptionExist( "RaysPerPixel" ) ) 
    { 
        m_RaysPerPixel = atoi(pOptions->getOption( "RaysPerPixel" ).c_str()); 
        pOptions->LogOption("RaysPerPixel", &Singleton<Logfile>::Get().Stream());
    }

    if ( pOptions->doesOptionExist( "Blocksize" ) ) 
    { 
        m_BlockSize = atoi(pOptions->getOption( "Blocksize" ).c_str()); 
        pOptions->LogOption("Blocksize", &Singleton<Logfile>::Get().Stream());
    }
    if ( pOptions->doesOptionExist( "RecurseLevel" ) ) 
    { 
        m_RecurseLevel = atoi(pOptions->getOption( "RecurseLevel" ).c_str()); 
        pOptions->LogOption("RecurseLevel", &Singleton<Logfile>::Get().Stream());
    }
}

void RayTracer::init(Scene* scn, AccelerationStructure* accelStruct, Camera* cam)
{
    m_pScene = scn;
    m_pCamera = cam;
    m_pAccelStruct = accelStruct;

    std::list<IModel*> models = m_pScene->getModelList();
    m_pAccelStruct->build(models);


    m_bImageComplete = false;
    m_bRefineInProgress = false;
}

void RayTracer::render()
{
    ///@todo Refactor in or out of codebase

    assert(0); // expected that this path is not exercised

    glDisable(GL_LIGHTING);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_pixelCols, 0, m_pixelRows, -0.1, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 

    RenderScene();

    CTextureManager::WriteTextureToPNG(m_ImageFilename, m_Image);
}

void RayTracer::deinit()
{

}

void RayTracer::keyboard( unsigned char key )
{

}

void RayTracer::Refine()
{
    if(!m_bRefineInProgress)
    {
        // first time Refine() was called, clear the buffers
        glDisable(GL_LIGHTING);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, m_pixelCols, 0, m_pixelRows, -0.1, 10.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity(); 

        m_CurrentBucketCol = m_StartBucketCol;
        m_CurrentBucketRow = m_StartBucketRow;

        m_bRefineInProgress = true;
    }

    if(m_CurrentBucketCol >= m_EndBucketCol)
    {
        m_CurrentBucketCol = m_StartBucketCol;
        m_CurrentBucketRow--;

        if(m_CurrentBucketRow < 0)
        {
            if(m_BlockSize == 1)
            {
                m_bImageComplete = true;
                m_bRefineInProgress = false;

                CTextureManager::WriteTextureToPNG(m_ImageFilename, m_Image);

                return;
            }
            else
            {
                m_BlockSize >>= 2;
                m_CurrentBucketCol = m_StartBucketCol;
                m_CurrentBucketRow = m_EndBucketRow - 1;
            }
        }
    }

    int pixelsPerBucketRow = m_pixelRows / m_BucketRows;
    int startRow = m_CurrentBucketRow * pixelsPerBucketRow;
    int endRow = startRow + pixelsPerBucketRow;

    int pixelsPerBucketCol = m_pixelCols / m_BucketCols;
    int startCol = m_CurrentBucketCol * pixelsPerBucketCol;
    int endCol = startCol + pixelsPerBucketCol;

    unsigned int startTick = GetTickCount();

    // for each pixel in bucket
    for(int row = startRow; row < endRow; row+= m_BlockSize)
    {
        for(int col = startCol; col < endCol; col+= m_BlockSize)
        {
            Color pixelColor(0.0f, 0.0f, 0.0f);

            // get sample rays
            float jitter = (m_RaysPerPixel == 1) ? 0 : 1.0;
            std::vector<Ray> sampleEyeRays = CalcEyeRaySamples(m_RaysPerPixel, jitter, row, col);

            unsigned int samples = 0;
            HitInfo hit;
            while(samples < m_RaysPerPixel)
            {
                Ray r = sampleEyeRays[samples];

                // find first object hit if it exists
                if (m_pAccelStruct->intersect(r, &hit))
                {
                    pixelColor += CalculateRadiance(r, hit, m_RecurseLevel);
                }

                ++samples;
            }

            // write pixel to texture buffer
            GLfloat pixel[4] = { pixelColor.R()/samples, pixelColor.G()/samples, pixelColor.B()/samples, 1.0f };
            m_ImageMaker->FillRect(col, row, m_BlockSize, m_BlockSize, pixel, TOP_LEFT);


            if((GetTickCount() - startTick) > 5000)
            {
                DWORD startRenderTime = GetTickCount();
                // TODO: add option to turn this off for power tests!
                if(m_Image)
                {
                    delete m_Image;
                    m_Image = NULL;
                }  
                m_Image = m_ImageMaker->MakeTexture();
                RenderSceneImage();

                DWORD stopRenderTime = GetTickCount();
                m_RenderTime += stopRenderTime - startRenderTime;
                startTick = stopRenderTime;
            }

        }
    }

    ++m_CurrentBucketCol;

    if(m_Image)
    {
        delete m_Image;
        m_Image = NULL;
    }  
    m_Image = m_ImageMaker->MakeTexture();
    RenderSceneImage();
}


void RayTracer::RenderScene()
{

    TonemapReinhard tmR;
    TonemapMaxToWhite tmWhite;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    unsigned int startTick = GetTickCount();

    int pixelsPerBucketRow = m_pixelRows / m_BucketRows;
    int pixelsPerBucketCol = m_pixelCols / m_BucketCols;

    for(int bucketRow = m_StartBucketRow; bucketRow < m_EndBucketRow; bucketRow++)
    {
        int startRow = bucketRow * pixelsPerBucketRow;
        int endRow = startRow + pixelsPerBucketRow;
        for(int bucketCol = m_StartBucketCol; bucketCol < m_EndBucketCol; bucketCol++)
        {
            int startCol = bucketCol * pixelsPerBucketCol;
            int endCol = startCol + pixelsPerBucketCol;

            // for each pixel in bucket
            for(int row = startRow; row < endRow; row+= m_BlockSize)
            {
                for(int col = startCol; col < endCol; col+= m_BlockSize)
                {
                    Color pixelColor(0.0f, 0.0f, 0.0f);

                    // get sample rays
                    float jitter = (m_RaysPerPixel == 1) ? 0.3 : 1.0;
                    std::vector<Ray> sampleEyeRays = CalcEyeRaySamples(1, jitter, row, col);

                    unsigned int samples = 0;
                    while(samples < m_RaysPerPixel)
                    {
                        Ray r = sampleEyeRays[samples];
                        HitInfo hit;

                        // find first object hit if it exists
                        if(m_pAccelStruct->intersect(r, &hit))
                        {
                            pixelColor += CalculateRadiance(r, hit, m_RecurseLevel);
                        }

                        ++samples;
                    }

                    // write pixel to texture buffer
                    GLfloat pixel[4] = { pixelColor.R(), pixelColor.G(), pixelColor.B(), 1.0f };
                    m_ImageMaker->FillRect(col, row, m_BlockSize, m_BlockSize, pixel, TOP_LEFT);
                }

                if((GetTickCount() - startTick) > 5000)
                {
                    // TODO: add option to turn this off for power tests!
                    if(m_Image)
                    {
                        delete m_Image;
                        m_Image = NULL;
                    }  
                    m_Image = m_ImageMaker->MakeTexture();
                    RenderSceneImage();

                    startTick = GetTickCount();
                } 
            } 
        }
    }

    // TODO: add option to turn this off for power tests!
    if(m_Image)
    {
        delete m_Image;
        m_Image = NULL;
    }
    //tmR.ApplyToneMap(&texMaker);
    m_Image = m_ImageMaker->MakeTexture();
    RenderSceneImage();
}


Color RayTracer::phongShade(HitInfo bestHit, Ray* l_Ray)
{
    Color backgroundColor = m_pScene->getBackgroundColor();
    Color color;

    if (bestHit.bHasInfo == false)
    {
        color = Color( backgroundColor );
    }
    else
    {
        Material* newMat = bestHit.hitObject->getMaterial();

        color = Color( newMat->GetAmbient() );

/*
        Ray feeler;
        feeler.Origin() = Point3( bestHit->hitPoint.X() + (l_Ray->Direction().X() - EPSILON),
                                   bestHit->hitPoint.Y() + (l_Ray->Direction().Y() - EPSILON),
                                   bestHit->hitPoint.Z() + (l_Ray->Direction().Z() - EPSILON) );
//        feeler.m_nRecurseLevel = 1;
*/
        std::list< IEmissive* >::iterator alight;
        std::list< IEmissive* >* lightList = m_pScene->getLightList();

        for (alight = lightList->begin(); alight != lightList->end(); alight++)
        {
            Color lightColor = (*alight)->GetEmissiveColor();
            const Point3 lightPos = (*alight)->GetPointOfEmission();

            Color contribution;
/*
            feeler.Direction() = lightPos - feeler.Origin();
            if ( IsInShadow( feeler ) )
            {
                // don't calculate light contribution for this light
                continue;
            }
*/
            // Compute the diffuse term

            Vector3 L(bestHit.hitPoint, lightPos);
            L.Normalize();

            float NdotL = Dot(L ,bestHit.hitNormal);

            if (NdotL > 0.0) // is normal within 90 degrees of light?
            {
                Color diffuse( lightColor * newMat->GetDiffuse() * NdotL );
                contribution += diffuse;
            }

            // Compute the specular term
            Vector3 V(bestHit.hitPoint, l_Ray->Origin());
            V.Normalize();

            Vector3 H(L + V);
            H.Normalize();

            float NdotH = Dot(H, bestHit.hitNormal);
            if ( NdotH > 0 )
            {
                float phong = pow( NdotH, newMat->GetSpecularExponent() );
                Color specular( lightColor * newMat->GetSpecular() * phong );
                contribution += specular;
            }

            color += contribution; // this should probably be multiply
        }
    }

    return color;
}

Color RayTracer::CalculateRadiance(Ray eyeRay, HitInfo hit, int recurseLevel)
{
    Color c;
    if ( m_view == PHONG )
    {
        c = phongShade( hit, &eyeRay );
    }
    else if ( m_view == NORMAL )
    {
        c = Color( hit.hitNormal.X(), hit.hitNormal.Y(), hit.hitNormal.Z(), 0 );
    }

    return c;
}


void RayTracer::RenderSceneImage()
{
    if(m_Image == NULL)
    {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    m_Image->Bind();

    glPolygonMode(GL_BACK, GL_FILL);
    glColor3f(1.0, 1.0, 1.0);

    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0);  glVertex3i(0, 0, -1);
    glTexCoord2f(0.0, 1.0);  glVertex3i(0, m_pixelRows, -1);
    glTexCoord2f(1.0, 1.0);  glVertex3i(m_pixelCols, m_pixelRows,-1);
    glTexCoord2f(1.0, 0.0);  glVertex3i(m_pixelCols, 0, -1);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers(); 

    Singleton<Logfile>::Get().Stream().flush();
}


std::vector<Ray> RayTracer::CalcEyeRaySamples(int sampleCount, float percentJitter, int pixelRow, int pixelCol)
{
    std::vector<Ray> sampleRays;

    if(sampleCount <= 0){ return sampleRays; }

    float H = m_pCamera->GetNearClipPlane() * tan( m_pCamera->GetViewAngle() * (PI/360.0));
    float W = H * m_pCamera->GetAspect();

    int samples = 0;
    while(samples < sampleCount)
    {
        float x = (percentJitter == 0.0f) ? (pixelCol + (m_BlockSize / 2.0f)) : randNumber(pixelCol, pixelCol + m_BlockSize);
        float y = (percentJitter == 0.0f) ? (pixelRow + (m_BlockSize / 2.0f)) : randNumber(pixelRow, pixelRow + m_BlockSize);

        // (Uc, Uv) is the point in space on the near-plane of the view volume
        float Uc = -W + (W * (2 * x) /(float) m_pixelCols);
        float Vr = -H + (H * (2 * y)    /(float) m_pixelRows);

        float N = m_pCamera->GetNearClipPlane();

        Vector3 n = m_pCamera->N() * -N;
        Vector3 u = m_pCamera->U() * Uc; 
        Vector3 v = m_pCamera->V() * Vr; 

        Vector3 direction = n + u + v; 

        direction.Normalize();

        sampleRays.push_back(Ray(m_pCamera->GetEye(), direction, pixelCol, pixelRow));

        ++samples;
    }

    return sampleRays;

}
