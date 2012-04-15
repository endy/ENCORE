//////////////////////////////////////////////////////////////////////////
// ENCORE PROJECT
// 
// Project description and license goes here.
// 
// Developed By
//      Brandon Light:   brandon@alum.wpi.edu
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "UniformGrid.h"
#include "BasicAS.h"
#include "RayTracer.h"
#include "PhotonMap.h"
#include "Photon.h"
#include "Color.h"
#include "IrrCacheOctree.h"

using encore::Color;

/// PhotonMapperCPU class
///
/// Rendering class that uses the Photon-mapping algorithm on the CPU
/// to generate images
///
class PhotonMapperCPU : public RayTracer
{
private: typedef RayTracer inherited;
public:
    PhotonMapperCPU(int in_pixelRows, int in_pixelCols);
	virtual ~PhotonMapperCPU();

public: // Renderer Interface

    /// Configures the renderer's properties
    virtual void configure( Options* pOptions );
    /// Initialize the renderer.  Must be called before calling render()
    virtual void init(Scene* scn, AccelerationStructure* accelStruct, Camera* cam);
    /// Render a single frame
    virtual void render();

    virtual void Refine();

public:  // Rendering Properties
	
    // TODO: make these properties flags that use a properties function w/ enumeration
    
    bool    ShowRays;
	bool	ShowTraces;
	bool	ShowPhotons;
    bool    ShowIrrCache;
	bool	Reflections;
	size_t	MaxBlockIndex;
	bool	RussianRoulette;

public:
    void SetImageFilename(std::string imageFilename){ m_ImageFilename = imageFilename; }

private: // methods
		
	// Shoots photons from light sources in the scene
	void EmitPhotons();	
	
	//// DEBUG RENDERING ALGORITHMS ////

	///  Display photon paths
	void RenderPhotonTraces();	
	//  Display photons
	void RenderPhotons();
    /// Display rays
    void RenderRays();
    /// Visualizes the irradiance cache, renders one frame beforehand
    void RenderIrrCache();

	//// PRIVATE HELPERS METHODS /////
	
	// shoot photon through scene...Russian roulette code goes here
	void TracePhoton(Photon p, Vector3 initialDir);

	void ReflectPhotonSpecularly(Photon *p, int bounces, Ray &photonPath, HitInfo surfaceInfo);
	void ReflectPhotonDiffusely(Photon *p, int bounces, Ray &photonPath, HitInfo surfaceInfo);
    void TransmitPhoton(Photon *p, int bounces, Ray &photonPath, HitInfo surfaceInfo);

    void EmitShadowPhoton(Photon p, Ray shadowPhotonPath);

    bool IsInShadow(Point3f surfacePoint, Point3f lightPoint);

    bool TransmitRay(Ray initial, HitInfo hit, Ray &transmitted);

	/// Calculate the radiance at this location
    virtual Color CalculateRadiance(Ray eyeRay, HitInfo hit, int recurseLevel = 1);
	
	/// Uses standard Monte Carlo ray tracing to calculate direct illumination in the scene
    Color CalculateDirectIllumination(Ray eyeRay, HitInfo hit, int recurseLevel);

	///  Uses the photon map (and final gather?) to calculate the indirect illumination in the scene
    Color CalculateIndirectIllumination(Ray eyeRay, HitInfo hit);

	/// Calculate radiance due to photons forming caustics
    Color CalculateCausticRadiance(Ray eyeRay, HitInfo hit);

	/// clear photon map
	void ResetPhotonMaps();
 
    /// find the terminating point of a ray transmitted through a transparent material
    void TransmitTillTermination(Ray incoming, HitInfo incidentHit, Ray &finalRay, HitInfo &finalHit);
   
	
private:  // member data

 	/// Stores the direct photons
    PhotonMap* m_DirectPhotonMap;
    /// Stores diffusely reflected photons
	PhotonMap* m_IndirectPhotonMap;
    /// Stores specularly reflected photons
	PhotonMap* m_causticsPhotonMap;

	bool m_photonMapsBuilt;

    /// Number of global photons to store
	int m_cGlobalPhotons;
    /// Number of caustic photons to store
	int m_cCausticPhotons;

    int m_cDirectPhotons;

    IrrCache<Color> *m_NewIrrCache;

	// misc rendering data
	vector< vector<Point3f> > photonPaths;

    ////  SETTINGS  ////////////////////

    // profiling
    unsigned long int m_DirectTime;
    unsigned long int m_IndirectTime;
    unsigned long int m_CausticsTime;

    int m_FinalGatherCount;
 
    // misc
    bool m_bCaptureScreenshot;
    std::string m_ImageFilename;

    // Emission
    bool m_EmitPhotons;
    bool m_bAverageOnBounce;
    int  m_cMaxBounces;
    float m_PhotonScale;

    int m_Nphotons;
    float m_MaxDistance;

    bool    m_bDoFinalGather;
    float   m_FGTolerance;
    int     m_cFGSamples;

    bool m_bPrecomputeIrr;
    int  m_PrecompIrrIncrement;

    bool m_bCacheIrradiance;

    bool m_bDirectOn;
    bool m_bIndirectOn;
    bool m_bCausticsOn;

    //// Profiling ////
    SimpleProfiler m_DirectProf;
    SimpleProfiler m_IndirectProf;
    SimpleProfiler m_CausticsProf;

};
