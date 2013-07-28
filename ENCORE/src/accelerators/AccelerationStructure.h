//////////////////////////////////////////////////////////////////////////
// ENCORE PROJECT
// 
// Project description and license goes here.
// 
// Developed By
//      Chen-Hao Chang:  chocobo7@WPI.EDU
//      Brandon Light:   brandon@alum.wpi.edu
//      Peter Lohrman:   plohrmann@alum.WPI.EDU
//////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef WIN32
#include "GPUAccelerationStructureData.h"
#endif

#include "Options.h"
#include "Shader.h"
#include "Ray.h"
#include "HitInfo.h"

// forward declarations
class Triangle;
class IModel;

using namespace encore;

/// AccelerationStructure class
///
/// Interface for acceleration structures
///
class AccelerationStructure
{
public:
    AccelerationStructure(){}
    virtual ~AccelerationStructure() {}

public:
    /// Prints the usage information for AccelerationStructure to STDOUT
    virtual void usage( void ) = 0;

    /// Configures the acceleration structure's properties
    virtual void configure( Options* l_pOptions ) = 0;

    /// builds the acceleration structure from the Scene
    virtual void build(std::list<IModel*> &modelList) = 0;

    /// update the acceleration structure
    /// used for dynamic scenes
    virtual void update(std::list<IModel*> &modelList) {}

#ifdef WIN32
    /// builds the accelerationStructure into the GPUAccelerationStructureData reference
    virtual void buildGPU(std::list<IModel*> &modelList, std::list<Triangle*> &triangleList, GPUAccelerationStructureData& l_pASD ) = 0;

    /// set the shader parameters that the accel struct needs to pass in
    virtual void setGPUParameters( CShader& l_Shader, GPUAccelerationStructureData& l_ASD ) = 0;
#endif
    
    /// returns the HitInfo of the first successful intersection 
    /// of the ray with the Primitives in the AccelerationStructure
    virtual bool intersect( Ray& l_Ray, HitInfo* pHitInfo ) = 0;

    /// defines how the AcceleratioStructure should react to keyboard input
    virtual void keyboard( unsigned char key ) = 0;
};
