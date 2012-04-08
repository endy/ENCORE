#pragma once

#include "AccelerationStructure.h"

#include "TrianglePrim.h"
#include <list>
#include <fstream>

#ifdef WIN32
#include <windows.h>
#endif

//#define NOTRIBOX
#define NEW
//forward declarations
class IModel;
class IPrimitive;

class UniformGrid : public AccelerationStructure
{
public:
    UniformGrid(void);
    virtual ~UniformGrid(void);

    // must implement to be an Acceleration Structure
    void usage( void );
    void configure( Options* l_pOptions );
    void keyboard( unsigned char key ) {}

    virtual void build(std::list<IModel*> &modelList);

#ifdef WIN32
    void buildGPU(std::list<IModel*> &modelList, std::list<Triangle*> &triangleList, GPUAccelerationStructureData& l_pASD );
    void setGPUParameters( CShader& l_Shader, GPUAccelerationStructureData& l_ASD );
#endif

    HitInfo intersect( Ray& l_pRay );
    virtual void update(std::list<IModel*> &modelList) { build(modelList); }

private:
//    int getNumRepPrims( void ){ return num_Rep_Tris; }
//    int getNumCells( void ){ return m_gridDimension; }
    void addpadding(int);
    void getVoxel(Ray *aRay) const;
    void stepgrid(Ray *l_pRay, Point3f l_step, Point3f *l_tMax) const;
    bool rayBoxIntersect(Ray *aRay, Point3f bmin, Point3f bmax, float* timeIn) const;
    Point3f sign(Point3f value) const;
    Point3f clamp(Point3f value, Point3f minimum, Point3f maximum) const;
    void buildGrid(std::list<IModel*> &modelList);

    IPrimitive* m_tempPrim;

    Point3f m_MinPt;     // smallest x,y,z within the grid
    Point3f m_MaxPt;     // largest x,y,z within the grid
    int m_gridDimension;// number of cells along one dimension in the grid
    float m_cellSize;   // size of the square cells in the grid

    bool sizeset;
    void setGridDimension(int l_gridDimension){ m_gridDimension = abs(l_gridDimension); sizeset = true; }
    void setOptimalGridDimension( void );
    void setModels(std::list<IModel*> &modelList);
    
//    void extractDataFromGrid();
    
    // information stored in texture-like data arrays
    /*
    float* verts0;      // holds all vertex0 information for the scene
    float* verts1;      // holds all vertex1 information for the scene
    float* verts2;      // holds all vertex2 information for the scene
    float* norms0;      // holds all normal0 information for the scene
    float* norms1;      // holds all normal1 information for the scene
    float* norms2;      // holds all normal2 information for the scene
    int* CPUcellInfo;   // has Primitive count and first index of each cell
    float* GPUcellInfo; // GPU version of cell information (different indices)
    */
    int m_num_Models; // number of models in this uniform grid
    int m_num_Diff_Prims;  // number of distinct primitives in the uniform grid
    int m_num_Rep_Prims;   // number of total primitives in the uniform grid
    IModel* m_models;      // 1D array of models that are in the uniform grid
    
    bool m_bHaveModels;        // have the models been given to the uniform grid yet?
#ifdef NEW
    IPrimitive** gdata;
    unsigned int*** m_Grid;
    unsigned short *** num_Grid;
#else
    std::list< IPrimitive* >**** m_Grid;  // 3 dimensional Grid of IPrimitives
#endif
};
