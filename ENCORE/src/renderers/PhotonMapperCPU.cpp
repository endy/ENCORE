/****************************************************************************
Brandon Light
03/11/2006

PhotonMapperCPU.cpp

****************************************************************************/

#include "PhotonMapperCPU.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <sstream>

#include "Common.h"
#include "Material.h"
#include "TextureManager.h"

#include "Tonemap.h"

#include "Sampler.h"

#include "Singleton.h"
#include "Logfile.h"

#ifdef WIN32
#include <GL/glut.h>
#else
#include <GL/freeglut.h>
#endif

#ifndef WIN32
#define GetTickCount() 0
#define DWORD unsigned int
#endif


using std::vector;

std::fstream emissionTest;

PhotonMapperCPU::PhotonMapperCPU(int pixelRows, int pixelCols):
RayTracer(pixelRows, pixelCols)
{
    m_DirectPhotonMap   = NULL;
    m_IndirectPhotonMap = NULL;
    m_causticsPhotonMap = NULL;
    m_photonMapsBuilt = false;

    m_NewIrrCache = NULL;

    //// debug /////
    ShowRays = false;
    ShowPhotons = false;
    ShowTraces = false;
    ShowIrrCache = false;
    Reflections = false;
    RussianRoulette = true;

    m_bCaptureScreenshot = false;

    ////  SETTINGS  ////////////////////

    // Emission
    m_EmitPhotons = true;
    m_bAverageOnBounce = false;
    m_cMaxBounces = 20;
    m_cGlobalPhotons = 1000;
    m_cCausticPhotons = 0;
    m_PhotonScale = 1.0f;

    m_bPrecomputeIrr = true;
    m_PrecompIrrIncrement = 4;

    m_bCacheIrradiance = false;

    m_Nphotons = 100;
    m_MaxDistance = 5.0f;

    m_bDoFinalGather = true;

    // radiance calculations
    m_bDirectOn   = true;
    m_bIndirectOn = false;
    m_bCausticsOn = false;

    m_DirectTime = 0;
    m_IndirectTime = 0;
    m_CausticsTime = 0;

    m_FinalGatherCount = 0;

}

PhotonMapperCPU::~PhotonMapperCPU()
{
    if(m_DirectPhotonMap != NULL)
    {
        delete m_DirectPhotonMap;
        m_DirectPhotonMap = NULL;
    }
    if(m_IndirectPhotonMap != NULL)
    {
        delete m_IndirectPhotonMap;
        m_DirectPhotonMap = NULL;
    }
    if(m_causticsPhotonMap != NULL)
    {
        delete m_causticsPhotonMap;
        m_causticsPhotonMap = NULL;
    }
    if(m_NewIrrCache)
    {
        delete m_NewIrrCache;
        m_NewIrrCache = NULL;
    }
}

void PhotonMapperCPU::configure(Options* pOptions)
{
    inherited::configure(pOptions);

    Singleton<Logfile>::Get().Stream() << "----------- CONFIGURATION: Photon Mapper ----------" << std::endl;

    // Renderer Options
    if ( pOptions->doesOptionExist( "FinalGatherOn" ) ) { m_bDoFinalGather = (pOptions->getOption( "FinalGatherOn" ) == "true"); pOptions->LogOption("FinalGatherOn", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "FGTolerance")) { m_FGTolerance = atof(pOptions->getOption("FGTolerance").c_str()); pOptions->LogOption("FGTolerance", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "FGRays")) { m_cFGSamples = atoi(pOptions->getOption("FGRays").c_str()); pOptions->LogOption("FGRays", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "NGatherPhotons")) { m_Nphotons = atoi(pOptions->getOption("NGatherPhotons").c_str()); pOptions->LogOption("NGatherPhotons", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "MaxDistance" ) ) { m_MaxDistance = atof(pOptions->getOption( "MaxDistance" ).c_str()); pOptions->LogOption("MaxDistance", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "CacheIrr" )){ m_bCacheIrradiance = (pOptions->getOption("CacheIrr") == "true"); pOptions->LogOption("CacheIrr", &Singleton<Logfile>::Get().Stream());}

    // PM Global Options
    if (pOptions->doesOptionExist( "RenderPhotons" )){ ShowPhotons = (pOptions->getOption("RenderPhotons") == "true"); pOptions->LogOption("RenderPhotons", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "DirectOn" ) ) { m_bDirectOn = (pOptions->getOption( "DirectOn" ) == "true"); pOptions->LogOption("DirectOn", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "IndirectOn" ) ) { m_bIndirectOn = (pOptions->getOption( "IndirectOn" ) == "true"); pOptions->LogOption("IndirectOn", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "CausticsOn" ) ) { m_bCausticsOn = (pOptions->getOption( "CausticsOn" ) == "true"); pOptions->LogOption("CausticsOn", &Singleton<Logfile>::Get().Stream());}

    // Photon Scattering Options
    if ( pOptions->doesOptionExist( "EmitPhotons" ) ) { m_EmitPhotons = (pOptions->getOption( "EmitPhotons" ) == "true"); pOptions->LogOption("EmitPhotons", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "MaxBounces" )){ m_cMaxBounces = atoi(pOptions->getOption("MaxBounces").c_str()); pOptions->LogOption("MaxBounces", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "GlobalPhotons" ) ) { m_cGlobalPhotons = atoi(pOptions->getOption( "GlobalPhotons" ).c_str()); pOptions->LogOption("GlobalPhotons", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "CausticPhotons" ) ) { m_cCausticPhotons = atoi(pOptions->getOption( "CausticPhotons" ).c_str()); pOptions->LogOption("CausticPhotons", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "DirectPhotons" ) ) { m_cDirectPhotons = atoi(pOptions->getOption("DirectPhotons").c_str()); pOptions->LogOption("DirectPhotons", &Singleton<Logfile>::Get().Stream()); }
    if ( pOptions->doesOptionExist( "PhotonScale" )){ m_PhotonScale = atof(pOptions->getOption("PhotonScale").c_str()); pOptions->LogOption("PhotonScale", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "PrecompIrr" )) { m_bPrecomputeIrr = (pOptions->getOption("PrecompIrr") == "true"); pOptions->LogOption("PrecompIrr", &Singleton<Logfile>::Get().Stream());}
    if ( pOptions->doesOptionExist( "PrecompIrrValue" )){ m_PrecompIrrIncrement = atoi(pOptions->getOption("PrecompIrrValue").c_str()); pOptions->LogOption("PrecompIrrValue", &Singleton<Logfile>::Get().Stream());}

}

void PhotonMapperCPU::init(Scene* scn, AccelerationStructure* accelStruct, Camera* cam)
{
    inherited::init(scn, accelStruct, cam);

    if(m_DirectPhotonMap)
    {
        delete m_DirectPhotonMap;
        m_DirectPhotonMap = NULL;
    }
    if(m_IndirectPhotonMap)
    {
        delete m_IndirectPhotonMap;
        m_IndirectPhotonMap = NULL;
    }
    if(m_causticsPhotonMap)
    {
        delete m_causticsPhotonMap;
        m_causticsPhotonMap = NULL;
    }
    m_photonMapsBuilt = false;

    if(m_NewIrrCache)
    {
        delete m_NewIrrCache;
        ShowIrrCache = false;
    }

    m_NewIrrCache = new IrrCache<Color>(m_FGTolerance, Point3(-75, -75, -75), 150, 150, 150);
}


void PhotonMapperCPU::render()
{
    glDisable(GL_LIGHTING);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_pixelCols, 0, m_pixelRows, -0.1, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 

    if(ShowRays)
    {
        RenderRays();
        return;
    }
    else if(ShowTraces)
    {
        EmitPhotons();
        RenderPhotonTraces();
        return;
    }
    else if(ShowPhotons)
    {
        EmitPhotons();
        // photon trace test
        RenderPhotons();
        return;
    }
    else if(ShowIrrCache)
    {
        RenderIrrCache();
        return;
    }

    EmitPhotons();

    RenderScene();

    CTextureManager::WriteTextureToPNG(m_ImageFilename, m_Image);
}

void PhotonMapperCPU::Refine()
{
    if(!m_bRefineInProgress)
    {
        m_photonMapsBuilt = false;
        if(m_DirectPhotonMap != NULL)
        {
            delete m_DirectPhotonMap;
            m_DirectPhotonMap = NULL;
        }
        if(m_IndirectPhotonMap != NULL)
        {
            delete m_IndirectPhotonMap;
            m_IndirectPhotonMap = NULL;
        }
        if(m_causticsPhotonMap != NULL)
        {
            delete m_causticsPhotonMap;
            m_causticsPhotonMap = NULL;
        } 

        m_ImageMaker->FillRect(0, 0, m_pixelCols, m_pixelRows, Color(0,0,0));

        m_CurrentBucketCol = m_StartBucketCol;
        m_CurrentBucketRow = m_StartBucketRow;

        SimpleProfiler emitProf;
        emitProf.Start();
        EmitPhotons();
        emitProf.Stop();
        emitProf.PrintLastTime("Emission time: ", &Singleton<Logfile>::Get().Stream());

        m_bImageComplete = false; 
    } 

    if(ShowPhotons)
    {
        RenderPhotons();
        m_bImageComplete = true;
        m_bRefineInProgress = false;

        return;
    }

    inherited::Refine();

    if(m_bImageComplete)
    {
        m_bRefineInProgress = false;

        Singleton<Logfile>::Get().Stream() << "Direct Time " << m_DirectTime << std::endl;
        Singleton<Logfile>::Get().Stream() << "Indirect Time " << m_IndirectTime << std::endl;
        Singleton<Logfile>::Get().Stream() << "Caustics Time " << m_CausticsTime << std::endl;
        Singleton<Logfile>::Get().Stream() << "Render Time " << m_RenderTime << std::endl;

        Singleton<Logfile>::Get().Stream() << "Final Gather Count: " << m_FinalGatherCount << std::endl;

        std::cout << "Final Gather Count: " << m_FinalGatherCount << std::endl;
    }
}


/////  PRIVATE MEMBER FUNCTIONS //////////////////////////////

void PhotonMapperCPU::RenderPhotonTraces()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_pCamera->LoadCameraMatrices();

    for(int index = 0; index < (int) photonPaths.size(); index++)
    {
        glColor3f(0.5, 0.5, 0.5);

        glBegin(GL_LINE_STRIP);

        for(int nodeIndex = 0; nodeIndex < (int) photonPaths[index].size(); nodeIndex++)
        {	
            Point3 node = photonPaths[index][nodeIndex];
            glVertex3f(node.X(),node.Y(), node.Z());
        }

        glEnd();

    }
    glutSwapBuffers();
}

void PhotonMapperCPU::RenderIrrCache()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    m_pCamera->LoadCameraMatrices();

    /*
    // render photons
    std::list< TElement<IrrCacheNode> > values;
    int maxDepth = 0;
    m_IrrCache->GetCache()->GetAllElementsNearPoint(Point3(0, 0, 0), 50, values, maxDepth);

    while(!values.empty())
    {
    TElement<IrrCacheNode> e = *values.begin();

    float R = e.Data().m_Color.R();
    float G = e.Data().m_Color.G();
    float B = e.Data().m_Color.B();

    Vector3 power(R, G, B); 
    power.Normalize();

    glColor3f(power.X(), power.Y(), power.Z());

    glBegin(GL_LINES);
    glVertex3f(e.Location().X() + 0.5, e.Location().Y() + 0.5, e.Location().Z() + 0.5);
    glVertex3f(e.Location().X(), e.Location().Y(), e.Location().Z());
    glEnd();

    values.pop_front();
    }

    */
    glutSwapBuffers();
}

void PhotonMapperCPU::RenderRays()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    m_pCamera->LoadCameraMatrices();

    glBegin(GL_LINES);

    //     float H = m_pCamera->GetNearClipPlane() * tan( m_pCamera->GetViewAngle() * (PI/360.0));
    //     float W = H * m_pCamera->GetAspect();
    // 
    //     Sampler s(m_pixelCols / m_BlockSize, m_pixelRows / m_BlockSize, 1);
    // 
    //     Vector3 eyeToLook = Vector3(m_pCamera->GetEye(), m_pCamera->GetLook());
    //     eyeToLook.Normalize();
    // 
    //     Point3 center = m_pCamera->GetEye() + (eyeToLook * m_pCamera->GetNearClipPlane()).ToPoint();
    // 
    //     vector<Point3> points = s.GetSamplePointsForRect(center, eyeToLook, W, H);
    //
    //     for(int sampleIndex = 0; sampleIndex < (int) points.size(); ++sampleIndex)
    //     {
    //         Point3 p = points[sampleIndex];
    //         glVertex3f(p.X(), p.Y(), p.Z());
    //         glVertex3f(p.X() + 0.1, p.Y() + 0.1, p.Z() + 0.1);
    //     }

    srand(0);
    Sampler t(10,20, 1);

    std::vector<Ray> emissionRays;
    Vector3 n(0,1,0);
    n.Normalize();
    emissionRays = t.GetSampleRaysForHemisphere(Point3(0,0,0), n);

    for(int sample = 0; sample < (int) emissionRays.size(); ++sample)
    {
        Point3 p1 = emissionRays[sample].Origin();
        Point3 p2 = emissionRays[sample].Origin() + emissionRays[sample].Direction().ToPoint() * 25;
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(p1.X(), p1.Y(), p1.Z());
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(p2.X(), p2.Y(), p2.Z());
    }
    //std::vector<Point3> points = t.GetSamplePointsForRect(Point3(0,50,0), Vector3(0,-1,0), 50,50);
    //for(int sample = 0; sample < (int) points.size(); ++sample)
    //{
    //    Point3 p1 = points[sample];
    //    glVertex3f(p1.X(), p1.Y(), p1.Z());
    //    glVertex3f(p1.X()+ 1, p1.Y()+ 1, p1.Z() + 1);
    //}
    glEnd();

    glutSwapBuffers();

}


void PhotonMapperCPU::RenderPhotons()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_pCamera->LoadCameraMatrices();

#if 0

    fstream file;
    file.open("pointsX.csv", ios_base::out);

    int pointsGenerated = 5000;
    srand(0);
    glColor3f(1.0, 1.0, 1.0);
    for(int i = 0; i < pointsGenerated; ++i)
    {
        Vector3 normal(1, 0, 0);
        normal.Normalize();
        Vector3 v = randDirectionN(normal);

        Point3 p = (v * 20).ToPoint();
        glPointSize(1.0);

        glBegin(GL_POINTS);
        glVertex3f(p.X(), p.Y(), p.Z());
        //file << Dot(v, normal) << "\n";
        glEnd();

    }
    glColor3f(1, 0, 1);
    glutSolidCube(1.0f);

    file.close();
#elif 0

    fstream file;
    file.open("randPoints.csv", ios_base::out);

    srand(0);
    vector<IEmissive*> lights = m_pScene->GetLightVector();
    for(int count = 0; count < 100; ++count)
    {
        IEmissive *l = lights[0];

        Ray path = l->EmitPhoton();

        glPointSize(1.0);
        glBegin(GL_POINTS);
        glVertex3f(path.Origin().X(), path.Origin().Y(), path.Origin().Z());
        glEnd();

        file << path.Origin().X() << "," << path.Origin().Y() << "," << path.Origin().Z() << "\n";
    }

    file.close();

#else
    vector<IEmissive*> lights = m_pScene->GetLightVector();
    // render lights
    for(size_t index = 0; index < lights.size(); index++)
    {
        IEmissive* l = lights[index];

        glColor3f(l->GetEmissiveColor().R(), l->GetEmissiveColor().G(), l->GetEmissiveColor().B());

        glPushMatrix();
        glTranslatef(l->GetPointOfEmission().X(), l->GetPointOfEmission().Y(), l->GetPointOfEmission().Z());
        glutSolidCube(0.1f);
        glPopMatrix();
    }

    Point3 loc = Point3(0, 0, 0);

    // render photons
    vector<PhotonDistPair> photons = m_causticsPhotonMap->GetNearestNPhotons(100000, 7000, loc, Vector3(-1, 0, 0));
    for(int index = 0; index < photons.size(); ++index)
    {
        const Photon* p = photons[index].m_pPhoton;

        float R = p->Power().R();
        float G = p->Power().G();
        float B = p->Power().B();

        Vector3 power(R, G, B); 
        power.Normalize();

        if(p->IsShadowPhoton())
        {
            glColor3f(0,0,1);
        }
        else
        {
            glColor3f(1,1,1);
        }
        glColor3f(power.X(), power.Y(), power.Z());

        glPointSize(1.0);
        glBegin(GL_POINTS);
        glVertex3f(p->Position().X(), p->Position().Y(), p->Position().Z());
        glEnd();
    }
#endif

    glutSwapBuffers();
}

void PhotonMapperCPU::EmitPhotons()
{
    SimpleProfiler prof;
    //emissionTest.open("emission.csv", ios_base::out);

    if(!m_EmitPhotons){ return; }

    if(m_photonMapsBuilt == true)
    {
        std::cout << "Photon map built!" << std::endl;	
        return;
    }

    if(m_DirectPhotonMap == NULL)
    {
        m_DirectPhotonMap = new PhotonMap(m_cDirectPhotons);
    }
    if(m_IndirectPhotonMap == NULL)
    {
        m_IndirectPhotonMap = new PhotonMap(m_cGlobalPhotons);
    }
    if(m_causticsPhotonMap == NULL)
    {
        m_causticsPhotonMap = new PhotonMap(m_cCausticPhotons);
        m_causticsPhotonMap->SetCausticsMap(true);
    }	

    // get all light sources
    vector<IEmissive*> lights = m_pScene->GetLightVector();

    // percentage of photons in scene form a lights
    //  each element corresponds to a light of the same index in lights
    vector<float> photonsPerLight(lights.size());

    // while maximum number of photons has not been reached
    while(m_IndirectPhotonMap->Full() != true)
    {
        // TODO: select random light--biased by power of the light
        int lightIndex = rand() % (int)lights.size();

        IEmissive* l = lights[lightIndex];

        Color power = l->GetEmissiveColor();

        // get photon from light source (light generates photon origin)		
        Ray photonRay = l->EmitPhoton();

        Photon p(photonRay.Origin(), power, photonRay.Direction());

        // shoot photon in a random direction!
        TracePhoton(p, photonRay.Direction());
    }

    while(m_causticsPhotonMap->Full() != true)
    {
        // TODO: select random light--biased by power of the light
        int lightIndex = rand() % (int)lights.size();

        IEmissive* l = lights[lightIndex];

        Color power = l->GetEmissiveColor();

        // get photon from light source (light generates photon origin)		
        Ray photonRay = l->EmitPhoton();

        Photon p(photonRay.Origin(), power, photonRay.Direction());

        // shoot photon in a random direction!
        TracePhoton(p, photonRay.Direction());
    }

    // scale photons

    m_IndirectPhotonMap->ScalePhotons(m_PhotonScale);

    float causticsScale = 0.00005;
    m_causticsPhotonMap->ScalePhotons(causticsScale);

    float directPhotonScale = 0.05;
    m_DirectPhotonMap->ScalePhotons(directPhotonScale);

    // build photon maps
    m_DirectPhotonMap->BalanceAndConstruct();

    prof.Start();
    m_IndirectPhotonMap->BalanceAndConstruct();
    prof.Stop();
    prof.PrintLastTime("Indirect Balance Time (" + intToString(m_IndirectPhotonMap->GetCountStored()) + " photons)", &Singleton<Logfile>::Get().Stream());


    prof.Start();
    m_causticsPhotonMap->BalanceAndConstruct();
    prof.Stop();
    prof.PrintLastTime("Caustic Balance Time (" + intToString(m_causticsPhotonMap->GetCountStored()) + " photons)", &Singleton<Logfile>::Get().Stream());


    if(m_bPrecomputeIrr)
    {
        SimpleProfiler precompProf;
        precompProf.Start();
        PhotonMap *precompPhotonMap = m_IndirectPhotonMap->PrecomputeIrradiance(m_PrecompIrrIncrement, m_Nphotons, m_MaxDistance);

        delete m_IndirectPhotonMap;
        m_IndirectPhotonMap = precompPhotonMap;
        precompProf.Stop();
        precompProf.PrintLastTime("Time to precompute irradiance: ", &Singleton<Logfile>::Get().Stream());

    }
    m_photonMapsBuilt = true;

    emissionTest.close();
}

void PhotonMapperCPU::TracePhoton(Photon p, Vector3 initialDir)
{
    int bounces = 0;

    Ray photonPath = Ray(p.Position() + (initialDir * 0.01f).ToPoint(), initialDir);
    while(bounces <= m_cMaxBounces)
    {
        // trace the photon along the current direction

        HitInfo hit;
        if (m_pAccelStruct->intersect(photonPath, &hit))
        {
            Material *mat = hit.hitObject->getMaterial();

            float pS = 0;//mat->GetAvgSpecularReflectance();
            float pD = mat->GetAvgDiffuseReflectance();
            float pT = mat->GetTransmittance();
            float pTotal = pS + pD + pT;

            // HACK: total must be < 1.0f (otherwise no absorbtion)
            // TODO: enforce the above constraint in some better fashion...
            if(pTotal <= 1.0f)
            {
                float E = randNumber(0.0f, 1.0f);

                if(E < pS)
                {
                    ReflectPhotonSpecularly(&p, bounces, photonPath, hit);
                }
                else if(E < (pS + pD))
                { 

                    ReflectPhotonDiffusely(&p, bounces, photonPath, hit);

                    emissionTest << Dot(hit.hitNormal, photonPath.Direction()) << endl;

                }
                else if( E < pTotal)
                {
                    // transmit photon...
                    TransmitPhoton(&p, bounces, photonPath, hit);
                }
                else
                {
                    // absorb photon and terminate loop
                    ReflectPhotonDiffusely(&p, bounces, photonPath, hit);
                    break;
                }
            }
        }
        else
        {
            // photon didn't hit anything--break out of loop (no more bouncing!)
            break;
        }

        bounces++;
    }
}

void PhotonMapperCPU::ReflectPhotonSpecularly(Photon *p, int bounces, Ray &photonPath, HitInfo surfaceInfo)
{
    // update photon due to surface interaction
    //p->Power(p->Power() * surfaceInfo.hitObject->getMaterial()->GetSpecular());  // not changing power...
    p->SurfNormal(surfaceInfo.hitNormal);
    p->Position(surfaceInfo.hitPoint);

    if(m_IndirectPhotonMap->Full() == false && surfaceInfo.hitObject->getMaterial()->GetAvgDiffuseReflectance() > 0)
    {
        // absorb photon (only if material has diffuse component)
        m_IndirectPhotonMap->AddPhoton(*p);
    }

    p->LastReflection(SPECULAR);

    // find specular reflection path
    float normScale = 2 * Dot(photonPath.Direction(), surfaceInfo.hitNormal);

    Vector3 rDir = photonPath.Direction() - (surfaceInfo.hitNormal * normScale);
    rDir.Normalize();

    photonPath.Direction() = rDir;
    photonPath.Origin() = surfaceInfo.hitPoint + (photonPath.Direction() * 0.001).ToPoint();
}

void PhotonMapperCPU::ReflectPhotonDiffusely(Photon *p, int bounces, Ray &photonPath, HitInfo surfaceInfo)
{
    // update photon due to surface interaction

    Material *mat = surfaceInfo.hitObject->getMaterial();

    Color diffuse = mat->GetDiffuse();

    Color power = p->Power();
    //Color scale = diffuse / mat->GetAvgDiffuseReflectance();
    //power *= scale;

    p->Power(power);
    p->SurfNormal(surfaceInfo.hitNormal);
    p->Position(surfaceInfo.hitPoint);

    // absorb photon in proper photon map
    if(p->LastReflection() == SPECULAR)
    {
        if(m_causticsPhotonMap->Full() == false)
        {
            // store photon in caustics map...
            m_causticsPhotonMap->AddPhoton(*p);
        }
    }
    else if(bounces > 0 && m_IndirectPhotonMap->Full() == false)
    {
        m_IndirectPhotonMap->AddPhoton(*p);
    }
    else if(bounces == 0 && m_DirectPhotonMap->Full() == false)
    {
        m_DirectPhotonMap->AddPhoton(*p);

        Ray shadowPhotonPath = photonPath;
        shadowPhotonPath.Origin() = surfaceInfo.hitPoint + (photonPath.Direction() * 2).ToPoint();

        EmitShadowPhoton(*p, shadowPhotonPath);
    }

    p->LastReflection(DIFFUSE);

    // find diffuse reflection path
    photonPath.Direction() = randDirectionSuperN(surfaceInfo.hitNormal);
    photonPath.Origin() = surfaceInfo.hitPoint + (photonPath.Direction() * 0.001f).ToPoint();
}

void PhotonMapperCPU::TransmitPhoton(Photon *p, int bounces, Ray &photonPath, HitInfo surfaceInfo)
{
    Ray transmittedPath;

    if(TransmitRay(photonPath, surfaceInfo, transmittedPath) == false)
    {
        // surface does not transmit photons, return
        return;
    }

    HitInfo transmittedHit;
    if (m_pAccelStruct->intersect(transmittedPath, &transmittedHit))
    {
        if(transmittedHit.isEntering == false)
        {
            p->SurfNormal(transmittedHit.hitNormal);
            p->Position(transmittedHit.hitPoint);

            TransmitRay(transmittedPath, transmittedHit, photonPath);
        }
        else
        {
            //assert(!"NOT EXITING OBJECT AFTER TRANSMISSION. CORRECT THE SCENE!");
        }
    }

    p->LastReflection(SPECULAR);
}

void PhotonMapperCPU::EmitShadowPhoton(
    Photon p, 
    Ray shadowPhotonPath)
{
    static int shadows = 0;

    HitInfo firstHit;
    while (m_pAccelStruct->intersect(shadowPhotonPath, &firstHit) && !firstHit.isEntering)
    {
        shadowPhotonPath.Origin() = firstHit.hitPoint + (shadowPhotonPath.Direction() * 0.01).ToPoint();
        p.Position() = firstHit.hitPoint;
    }

    if (firstHit.bHasInfo)
    {
        Photon shadowPhoton(firstHit.hitPoint, -p.Power(), shadowPhotonPath.Direction());
        shadowPhoton.SetShadowPhoton(true);
        shadowPhoton.SurfNormal(firstHit.hitNormal);
        m_DirectPhotonMap->AddPhoton(shadowPhoton);

        shadows++;
    }
}

bool PhotonMapperCPU::TransmitRay(Ray initial, HitInfo incidentHit, Ray &transmittedRay)
{
    ////  CHECK FOR TERMINATION  /////
    Material* mat = incidentHit.hitObject->getMaterial();
    if(mat->GetTransparency() < 0.6)
    {
        // material does not transmit...
        transmittedRay = initial;
        return false;
    }

    ////  CHECK MEDIUM  /////
    float c1, c2;
    float airIndex = 0.99;
    if(incidentHit.isEntering)
    {
        // use air as c1
        c1 = airIndex;
        c2 = mat->GetRefractionIndex();
    }
    else
    {
        // use air as c2
        c2 = airIndex;
        c1 = mat->GetRefractionIndex();   
    }

    //// CALC T  /////
    Vector3 N = incidentHit.hitNormal;

    float cRatio = (c1 == 0) ? (c2 / 0.0001) : (c2 / c1);

    // calc theta2
    float nDotDir = Dot(N, initial.Direction());
    float theta2 = sqrt(1 - (cRatio * cRatio) * (1 - (nDotDir * nDotDir)));


    // calc T
    Vector3 T = (initial.Direction() * cRatio) + (N * (cRatio*nDotDir - theta2));
    T.Normalize();

    //// SAVE TRANSMISSION RAY ////

    transmittedRay.Direction() = T;
    transmittedRay.Origin() = incidentHit.hitPoint + (T * 0.01).ToPoint();

    return true;   
}



Color PhotonMapperCPU::CalculateRadiance(Ray eyeRay, HitInfo hit, int recurseLevel /* = 1 */)
{
    Color radiance;

    if(recurseLevel <= 0)
    {
        return radiance;
    }

    m_DirectProf.Start();
    if(m_bDirectOn)
    {
        // calculate direct illumination
        radiance += CalculateDirectIllumination(eyeRay, hit, recurseLevel);
    }
    m_DirectProf.Stop();

    m_IndirectProf.Start();
    if(m_bIndirectOn)
    {
        // calculate indirect illumination
        radiance += CalculateIndirectIllumination(eyeRay, hit);
    }
    m_IndirectProf.Stop();

    m_CausticsProf.Start();
    if(m_bCausticsOn)
    {
        // calculate illumination due to caustics
        radiance += CalculateCausticRadiance(eyeRay, hit);
    }
    m_CausticsProf.Stop();



    return radiance;
}

Color PhotonMapperCPU::CalculateDirectIllumination(Ray eyeRay, HitInfo hit, int recurseLevel)
{
    DWORD startTime = GetTickCount();

    // DO SHADOW TEST WITH PHOTONS
    vector<PhotonDistPair> photons;

    if(m_DirectPhotonMap)
    {
        photons = m_DirectPhotonMap->GetNearestNPhotons(m_Nphotons, 7.0f, hit.hitPoint, hit.hitNormal);
    }

    bool doShadowTest = false;
    int shadowPhotonCount = 0;
    int normalPhotonCount = 0;
    float toleratedShadow = 0.01;

    if(photons.size() > 0)
    {
        for(int index = 0; index < photons.size(); ++index)
        {
            if(photons[index].m_pPhoton->IsShadowPhoton())
            {
                shadowPhotonCount++;
            }
            else
            {

                normalPhotonCount++;
            }  
        }
        float shadowDensity = shadowPhotonCount / (float) photons.size();
        if(toleratedShadow < shadowDensity && shadowDensity < 1)
        {
            doShadowTest = true;
        }
    } 

    Material *objMaterial = hit.hitObject->getMaterial();
    Color diffuse, specular, ambient, emissive;


    Color Ia = Color::One(); // acts as the sum of the contribution of all lights

    ambient = Ia * objMaterial->GetAmbient();

    // calculate diffuse & specular 
    if(shadowPhotonCount == 0 || normalPhotonCount > 0)
    {
        // AREA LIGHT CALCULATION
        vector<IEmissive*> lights = m_pScene->GetLightVector();
        for(size_t index = 0; index < lights.size(); index++)
        {
            IEmissive* l = lights[index];

            std::vector<Point3> samplePoints;
            if(doShadowTest)
            {
                samplePoints = l->GetSamplePoints(1,1,1);
            }
            else
            {
                samplePoints = l->GetSamplePoints(1,1,1);
            }

            for(int sampleIndex = 0; sampleIndex < (int)samplePoints.size(); ++sampleIndex)
            {
                bool lit = (doShadowTest) ? !IsInShadow(hit.hitPoint, samplePoints[sampleIndex]) : true;
                if(lit)
                {
                    // Shade
                    Vector3 dirToLight(hit.hitPoint, samplePoints[sampleIndex]);
                    dirToLight.Normalize();

                    // TODO: direct lighting fix until area lights are refactored a bit

                    // DIFFUSE //
                    float lightContrib = Dot(-dirToLight, Vector3(0,-1, 0));
                    Vector3 normal = (hit.isEntering) ? hit.hitNormal :  Vector3(0, 0, 0) - hit.hitNormal ; 
                    normal.Normalize();

                    float contrib = max(Dot(normal, dirToLight), 0.0f);
                    diffuse += objMaterial->GetDiffuse() * l->GetEmissiveColor() * lightContrib * contrib;  

                    // SPECULAR //
                    if(objMaterial->GetAvgSpecularReflectance() > 0)
                    {
                        Vector3 V(hit.hitPoint, eyeRay.Origin());
                        V.Normalize();

                        Vector3 H(dirToLight + V);
                        H.Normalize();

                        float NdotH = Dot(H, hit.hitNormal);
                        if ( NdotH > 0 )
                        {
                            float Ps = pow(NdotH, objMaterial->GetSpecularExponent());
                            specular += Color(l->GetEmissiveColor() * objMaterial->GetSpecular() * Ps);
                        }
                    }
                }                    
            }

            diffuse /= (float) samplePoints.size();
            specular /= (float) samplePoints.size();
        }
    }

    // calculate reflection 
    if(objMaterial->GetReflectivity() >  0)
    {
        eyeRay.Direction().Normalize();
        hit.hitNormal.Normalize();

        float normScale = 2 * Dot(eyeRay.Direction(), hit.hitNormal);

        Vector3 rDir = eyeRay.Direction() - (hit.hitNormal * normScale);
        rDir.Normalize();

        Ray reflectionRay;
        reflectionRay.Direction() = rDir;
        reflectionRay.Origin() = hit.hitPoint + (rDir * 0.001).ToPoint();

        HitInfo reflectionInfo;
        if (m_pAccelStruct->intersect(reflectionRay, &reflectionInfo))
        {
            DWORD deltaStart = GetTickCount();
            specular += CalculateRadiance(reflectionRay, reflectionInfo, recurseLevel - 1);// * objMaterial.GetReflectivity();
            DWORD deltaStop = GetTickCount();

            DWORD delta = deltaStop - deltaStart; // ignore the time to do this radiance calc--its already included
            startTime += delta; // ignore it by adding it to the start time (mathematically this will subtract it from the total time this direct calculation takes)
        }
    }

    // calculate refraction
    if(objMaterial->GetTransparency() > 0.6)
    {
        HitInfo finalHit;
        Ray finalRay;

        TransmitTillTermination(eyeRay, hit, finalRay, finalHit);

        if(finalHit.bHasInfo)
        {
            specular += CalculateRadiance(finalRay, finalHit, recurseLevel - 1);
        }
    }

    // emissive
    if(hit.hitObject->getMaterial()->IsEmissive())
    {
        emissive = hit.hitObject->getMaterial()->GetDiffuse();
    }

    m_DirectTime += GetTickCount()-startTime;

    return diffuse + specular + emissive + ambient;
}

Color PhotonMapperCPU::CalculateIndirectIllumination(const Ray& eyeRay, const HitInfo& hit)
{
    if(!(hit.hitObject->getMaterial()->GetAvgDiffuseReflectance() > 0))
    {
        return Color(0,0,0);
    }

    DWORD startTime = GetTickCount();

    Color illumination;

    if(!m_bDoFinalGather)
    {
        if(m_bPrecomputeIrr)
        {
            vector<PhotonDistPair> photons = m_IndirectPhotonMap->GetNearestNPhotons(1, m_MaxDistance, hit.hitPoint, hit.hitNormal);
            if(photons.size() == 0)
            {
                illumination = Color();
            }
            else
            {
                illumination = photons[0].m_pPhoton->Power();
                illumination *= hit.hitObject->getMaterial()->GetDiffuse();
            } 
        }
        else
        {
            illumination = m_IndirectPhotonMap->GetRadianceEstimate(m_Nphotons, m_MaxDistance, hit.hitPoint, hit.hitNormal);
            illumination *= hit.hitObject->getMaterial()->GetDiffuse();
        }

        // add exec time for indirect
        m_IndirectTime += GetTickCount() - startTime;

        return illumination;
    }
    float fgScaleHack = 0.35;
    if(m_bCacheIrradiance)
    {
        std::list< CacheValueWeightPair<Color> > cachedValues;
        float weightSum = m_NewIrrCache->GetIrrValuesNearPoint(hit.hitPoint, hit.hitNormal, cachedValues);

        if(weightSum)
        {
            std::list< CacheValueWeightPair<Color> >::iterator iter = cachedValues.begin();
            while(iter != cachedValues.end())
            {
                illumination += iter->m_Value.Data() * iter->m_Weight;
                ++iter;
            }

            illumination /= weightSum;

            // add exec time for indirect
            m_IndirectTime += GetTickCount() - startTime;

            return illumination * hit.hitObject->getMaterial()->GetDiffuse() * fgScaleHack;
        }
    }

    Sampler s(m_cFGSamples, m_cFGSamples,1);
    s.SetJitterPercent(0.5f);
    vector<Ray> sampleRays = s.GetSampleRaysForHemisphere(hit.hitPoint, hit.hitNormal);

    int sampleHits = 0;

    m_FinalGatherCount++;
    
    float Ri = 0; // harmonic mean
    HitInfo sampleHit;
    for(int sampleIndex = 0; sampleIndex < (int)sampleRays.size(); ++sampleIndex)
    {   
        // generate ray
        sampleRays[sampleIndex].Origin() = hit.hitPoint;// + (sampleRays[sampleIndex].Direction() * 0.0000001f).ToPoint();

        // intersect
        if(m_pAccelStruct->intersect(sampleRays[sampleIndex], &sampleHit))
        {
            Color irrValue;
            if(m_bPrecomputeIrr)
            {
                vector<PhotonDistPair> photons = m_IndirectPhotonMap->GetNearestNPhotons(1, 10, sampleHit.hitPoint, sampleHit.hitNormal);

                if(photons.size() == 0){
                    irrValue = Color(0.f, 0.f, 0.f); // black
                }
                else{
                    irrValue = photons[0].m_pPhoton->Power() * sampleHit.hitObject->getMaterial()->GetDiffuse();
                }
                irrValue += m_DirectPhotonMap->GetRadianceEstimate(100, 2, sampleHit.hitPoint, sampleHit.hitNormal) *
                    sampleHit.hitObject->getMaterial()->GetDiffuse();  
            }
            else
            {
                irrValue = m_IndirectPhotonMap->GetRadianceEstimate(m_Nphotons, m_MaxDistance, sampleHit.hitPoint, sampleHit.hitNormal) *
                    sampleHit.hitObject->getMaterial()->GetDiffuse();

                irrValue += m_DirectPhotonMap->GetRadianceEstimate(100, 2, sampleHit.hitPoint, sampleHit.hitNormal) *
                    sampleHit.hitObject->getMaterial()->GetDiffuse();               
            } 


            Ri += 1 / (Vector3(sampleHit.hitPoint, hit.hitPoint)).Magnitude();
            illumination += irrValue; 

            sampleHits++;
        }
    }

    illumination *= PI / sampleRays.size();

    if(Ri != 0)
    {
        Ri = sampleHits / Ri;
    } 
    else
    {
        Ri = 0;
    }
    
    if(m_bCacheIrradiance)
    {
        //Color newIllumination(eyeRay.m_dbgX / 100.0f, 0.0f, eyeRay.m_dbgY / 100.0f);
        m_NewIrrCache->AddElement(TCacheElement<Color>(illumination, hit.hitPoint, hit.hitNormal, Ri));
    }

    // add exec time for indirect
    m_IndirectTime += GetTickCount() - startTime;

    return illumination * hit.hitObject->getMaterial()->GetDiffuse() * fgScaleHack;

}

Color PhotonMapperCPU::CalculateCausticRadiance(Ray eyeRay, HitInfo hit)
{
    DWORD startTime = GetTickCount();
    Color caustics = m_causticsPhotonMap->GetRadianceEstimate(500, 5, hit.hitPoint, hit.hitNormal);
    m_CausticsTime += GetTickCount() - startTime;

    return caustics;
}

bool PhotonMapperCPU::IsInShadow(Point3 surfacePoint, Point3 lightPoint)
{
    Vector3 shadowRayDir = Vector3(surfacePoint, lightPoint);

    float distToLight = shadowRayDir.Magnitude();

    shadowRayDir.Normalize();

    Point3 shadowRayOrigin = surfacePoint + (shadowRayDir * 0.01).ToPoint();
    Ray shadowRay(shadowRayOrigin, shadowRayDir);
    HitInfo shadowHit;

    bool inShadow = (m_pAccelStruct->intersect(shadowRay, &shadowHit) && shadowHit.hitTime < distToLight);

    return inShadow;
}


void PhotonMapperCPU::TransmitTillTermination(Ray incoming, HitInfo incidentHit, Ray &finalRay, HitInfo &finalHit)
{
    ////  CHECK FOR TERMINATION  /////
    Material* mat = incidentHit.hitObject->getMaterial();
    if(mat->GetTransparency() < 0.6)
    {
        // found termination point!
        finalRay = incoming;
        finalHit = incidentHit;
        return;
    }

    ////  CHECK MEDIUM  /////
    float c1, c2;
    float airIndex = 0.99;
    if(incidentHit.isEntering)
    {
        // use air as c1
        c1 = airIndex;
        c2 = mat->GetRefractionIndex();
    }
    else
    {
        // use air as c2
        c2 = airIndex;
        c1 = mat->GetRefractionIndex();   
    }

    //// CALC T  /////
    Vector3 N = incidentHit.hitNormal;

    float cRatio = (c1 == 0) ? (c2 / 0.0001) : (c2 / c1);

    // calc theta2
    float nDotDir = Dot(N, incoming.Direction());
    float theta2 = sqrt(1 - (cRatio * cRatio) * (1 - (nDotDir * nDotDir)));


    // calc T
    Vector3 T = (incoming.Direction() * cRatio) + (N * (cRatio*nDotDir - theta2));
    T.Normalize();

    //// TRANSMIT AGAIN ////
    Ray transRay;
    transRay.Direction() = T;
    transRay.Origin() = incidentHit.hitPoint + (T * 0.01).ToPoint();

    HitInfo transHit;
    if(m_pAccelStruct->intersect(transRay, &transHit))
    {
        TransmitTillTermination(transRay, transHit, finalRay, finalHit);
    }
    else
    {
        // no hit, so must be blackness here!
        finalHit = transHit;
        finalRay = transRay;
        return;
    }

}



void PhotonMapperCPU::ResetPhotonMaps()
{
    if(m_DirectPhotonMap != NULL)
    {
        delete m_DirectPhotonMap;
        m_DirectPhotonMap = NULL;
    }
    if(m_IndirectPhotonMap != NULL)
    {
        delete m_IndirectPhotonMap;
        m_IndirectPhotonMap = NULL;
    }
    if(m_causticsPhotonMap != NULL)
    {
        delete m_causticsPhotonMap;
        m_causticsPhotonMap = NULL;
    }

    m_photonMapsBuilt = false;
}

