#pragma once
#include "AccelerationStructure.h"
#include "Options.h"

#include <string>
#include <list>

class IPrimitive;

class BasicAS :
    public AccelerationStructure
{
public:
    BasicAS( void );
    virtual ~BasicAS( void );

    virtual void usage( void ) {}
    virtual void configure( Options *l_pConfigOptions ) {}
    virtual void keyboard( unsigned char key ) {}

    virtual void build(std::list<IModel*> &modelList);

#ifdef WIN32
    virtual void buildGPU(std::list<IModel*> &modelList, std::list<Triangle*> &triangleList, GPUAccelerationStructureData& l_pASD );
    virtual void setGPUParameters( CShader& l_Shader, GPUAccelerationStructureData& l_ASD );
#endif

    virtual bool intersect(Ray& l_pRay, HitInfo* pHitInfo);

private:
    std::list< IPrimitive* > m_lpAllPrimitives;
};
