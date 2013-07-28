#include "BasicAS.h"

#include <cassert>
#include "IModel.h"
#include "Primitive.h"
#include "TrianglePrim.h"
#include "Common.h"

#define STUB 1

BasicAS::BasicAS(void)
{
}

BasicAS::~BasicAS(void)
{
    m_lpAllPrimitives.clear();
}

/*******************
* void BasicAS::build( Scene* l_pScene )
*
* builds the BasicAS from the models in the scene.
*******************/
void BasicAS::build( std::list<IModel*> &modelList )
{
    DWORD dwBuildTime = EncoreGetTime();
    m_lpAllPrimitives.clear();

    std::list< IModel* >::iterator aModel;

    for ( aModel = modelList.begin(); aModel != modelList.end(); aModel++ )
    {
        std::list< IPrimitive* >::iterator aPrim;
        std::list< IPrimitive* >& pPrimList = (*aModel)->getPrimitiveList();

        for ( aPrim = pPrimList.begin(); aPrim != pPrimList.end(); aPrim++ )
        {
            m_lpAllPrimitives.push_back( *aPrim );
        }
    }

    dwBuildTime = EncoreGetTime() - dwBuildTime;
    printf("BasicAS: construction: %.3f\n", dwBuildTime/1000.0f);
}


/***********************
* HitInfo BasicAS::buildGPU( Scene& l_pScene, GPUAccelerationStructureData& l_pASD )
*
* builds the scene into a GPU Acceleration Structure based on a Basic structre
***********************/
void BasicAS::buildGPU(std::list<IModel*> &modelList, std::list<Triangle*> &triangleList, GPUAccelerationStructureData& l_pASD )
{
#ifndef STUB
    // set GPUdata Sizes
    if ( triangleList.size() == 0 )
    {
        assert( !"Error in BasicAS:buildGPU - getnewTesselation returned no triangles." );
    }

    l_pASD.setTraversalShaderFilename( "shaders\\BasicAS.cg" );
    l_pASD.setVertexCount( (unsigned int)triangleList.size()*3 ); // each triangle has 3 vertices
    l_pASD.setNormalCount( (unsigned int)triangleList.size()*3 ); // each triangle has 3 normals
    l_pASD.setMaterialCount( (unsigned int)triangleList.size() ); // each triangle has 1 material
    l_pASD.setCellData0Count( 1 ); // x = #of vertices, y = unused, z = unused, w = unused

    // store how many triangles there are
    l_pASD.m_pCellData0[0] = (float)triangleList.size();

    // store the triangle data using pointers into the current lists
    // this is faster than using indices
    float* pv0 = l_pASD.m_pVertex0Array;
    float* pv1 = l_pASD.m_pVertex1Array;
    float* pv2 = l_pASD.m_pVertex2Array;
    float* pn0 = l_pASD.m_pNormal0Array;
    float* pn1 = l_pASD.m_pNormal1Array;
    float* pn2 = l_pASD.m_pNormal2Array;
    float* pm0 = l_pASD.m_pMaterial0Array;

    Vertex v;
    Material m;
    std::list<Triangle*>::iterator triIter;
    for ( triIter = triangleList.begin(); triIter != triangleList.end(); triIter++ )
    {
        // (*var)++ : sets value then increments pointer

        // Material
        m = *((*triIter)->getMaterial());

        *pm0 = m.GetDiffuse().R();            pm0++;
        *pm0 = m.GetDiffuse().G();            pm0++;
        *pm0 = m.GetDiffuse().B();            pm0++;
        *pm0 = m.GetSpecularExponent();     pm0++;

        // vertex 0
        v = *((*triIter)->getVertex0());

        *pv0 = v.getCoordinates().X();    pv0++;
        *pv0 = v.getCoordinates().Y();    pv0++;
        *pv0 = v.getCoordinates().Z();    pv0++;
        *pv0 = 1.0f;                      pv0++;

        *pn0 = v.getNormal().X();     pn0++;
        *pn0 = v.getNormal().Y();     pn0++;
        *pn0 = v.getNormal().Z();     pn0++;
        *pn0 = 1.0f;                  pn0++;

        // vertex 1
        v = *((*triIter)->getVertex1());

        *pv1 = v.getCoordinates().X();    pv1++;
        *pv1 = v.getCoordinates().Y();    pv1++;
        *pv1 = v.getCoordinates().Z();    pv1++;
        *pv1 = 1.0f;                      pv1++;

        *pn1 = v.getNormal().X();     pn1++;
        *pn1 = v.getNormal().Y();     pn1++;
        *pn1 = v.getNormal().Z();     pn1++;
        *pn1 = 1.0f;                  pn1++;

        // vertex 2
        v = *((*triIter)->getVertex2());

        *pv2 = v.getCoordinates().X();    pv2++;
        *pv2 = v.getCoordinates().Y();    pv2++;
        *pv2 = v.getCoordinates().Z();    pv2++;
        *pv2 = 1.0f;                      pv2++;

        *pn2 = v.getNormal().X();     pn2++;
        *pn2 = v.getNormal().Y();     pn2++; 
        *pn2 = v.getNormal().Z();     pn2++;
        *pn2 = 1.0f;                  pn2++;
    }
#endif
}

/**********************
* void BasicAS::setGPUParameters( CShader& l_Shader )
*
* sets shader parameters for the BasicAS
***********************/
void BasicAS::setGPUParameters( CShader& l_Shader, GPUAccelerationStructureData& l_ASD )
{
#ifndef STUB
    CGparameter& cell_info = l_Shader.GetNamedParameter("cellData0", true);
    cgGLSetTextureParameter(cell_info, l_ASD.m_CellTexture[0]);

    CGparameter& v0 = l_Shader.GetNamedParameter("v0t", true);
    cgGLSetTextureParameter(v0, l_ASD.m_VertexTexture[0]);

    CGparameter& v1 = l_Shader.GetNamedParameter("v1t", true);
    cgGLSetTextureParameter(v1, l_ASD.m_VertexTexture[1]);

    CGparameter& v2 = l_Shader.GetNamedParameter("v2t", true);
    cgGLSetTextureParameter(v2, l_ASD.m_VertexTexture[2]);
#endif
}



/***********************
* HitInfo BasicAS::intersect( Ray* l_pRay )
*
* finds the first hit of the ray within the BasicAS
***********************/
bool BasicAS::intersect( Ray& l_pRay, HitInfo* pHitInfo )
{

#ifndef INFINITY
#define INFINITY     FLT_MAX
#endif

    pHitInfo->bHasInfo = false;
    pHitInfo->hitTime = INFINITY;

    // check all the primitives for a hit
    std::list< IPrimitive* >::iterator ilpPrim;
    for( ilpPrim = m_lpAllPrimitives.begin(); ilpPrim != m_lpAllPrimitives.end(); ilpPrim++)
    {
        HitInfo newHit;
        (*ilpPrim)->intersect( l_pRay, &newHit);

        if ( 0 < newHit.hitTime &&
            newHit.hitTime < pHitInfo->hitTime )
        {
            *pHitInfo = newHit;
            pHitInfo->bHasInfo = true;
        }
    }

    // return the best hit
    return pHitInfo->bHasInfo;
}
