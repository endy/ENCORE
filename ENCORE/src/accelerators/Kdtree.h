#pragma once

#include "AccelerationStructure.h"

#ifdef WIN32
#include <windows.h>
#endif

#include "TrianglePrim.h"
#include <vector>
#include <list>
#include <fstream>
#include <deque>
#include "Common.h"
#include <iostream>

//forward declarations
class Model;
class IPrimitive;

//#define SAH

// true kdtree node
// base on physical rendering book
struct BSPNode
{
	union 
    {
		unsigned int flags;
		float split;
		unsigned int num;
	};
	union 
    {
		unsigned int leftChild;
        IPrimitive ** m_pPrimList;
        //std::list<IPrimitive*> *m_pPrimList;
	};

    //void initLeaf(int np, std::list<unsigned int> *a)
    void initLeaf(int np, IPrimitive **a)
	{
		num = np << 2;
		flags |= 3;
		m_pPrimList = a;
	}
	void initNode(int axis, float s)
	{
		split = s;
		flags &= ~3;
		flags |= axis;
	}
	float SplitPos() const { return split; }
	int nTriangle() const { return num >> 2; }
	int splitAxis() const { return flags & 3; }
	bool isLeaf() const { return (flags & 3) == 3; }
    void deleteIndex() { if(isLeaf()) { if(m_pPrimList) { delete [] m_pPrimList; m_pPrimList = 0;} } }
};

class boundedge {
public:

	float splitpos;
	enum {start = 1, end = 0} type;
//    unsigned int index;
//    boundedge(float _p = 0, unsigned int _i = 0, bool _s = start) : splitpos(_p), index(_i), type(start) 
    IPrimitive* prim;
    boundedge() {};
    //
    boundedge(float splitPosition, IPrimitive* pPrim, bool bStart )
    { 
        splitpos = splitPosition;
        prim = pPrim;
        if(!bStart)
            type = end;
        else
            type = start;
    }

    void set(float splitPosition, IPrimitive* pPrim, bool bStart )
    { 
        splitpos = splitPosition;
        prim = pPrim;
        if(!bStart)
            type = end;
        else
            type = start;
    }
    //
	bool operator<(const boundedge &e) const 
    {
		if(splitpos == e.splitpos)
			return (int)type < (int)e.type;
		else
			return splitpos < e.splitpos;
	};	
};

struct TODO {
	BSPNode * node;
	float tmin;
	float tmax;
};

struct AssistIndex
{
    unsigned int id;
    unsigned int offset;
};

class Kdtree : public AccelerationStructure
{
public:
    Kdtree();
    virtual ~Kdtree();

    // must implement to be an Acceleration Structure
    void usage( void ) {}
    void configure( Options* l_pOptions ) { } //this->setMaxDepth( 1, 1); }
    void keyboard( unsigned char key ) {}

    virtual void build(std::list<IModel*> &modelList);
		
#ifdef WIN32
    void buildGPU(std::list<IModel*> &modelList, std::list<Triangle*> &triangleList, GPUAccelerationStructureData& accelStructData );
    void setGPUParameters( CShader& shader, GPUAccelerationStructureData& accelStructData );
#endif
		
		bool intersect(Ray& aRay, HitInfo* pHitInfo);

    void setCost(float ic, float it, float eb) { m_IntersectCost = ic, m_TraversalCost = it; m_EmptyBonus = eb; }
    void useSAH(bool s) { if(s) m_SplitMode = SPLIT_SAH; else m_SplitMode = SPLIT_NAIVE; }

//    float3 intersect(const ray &_ray, unsigned int &model, unsigned int &index) const;
//    bool intersectb(const ray&_ray) const ;
    void setMaxDepth(unsigned int d, unsigned int item = 16);
    unsigned int getTriangleCount() const { return m_TotalPrimCount; }
    virtual void update(std::list<IModel*> &modelList) { build(modelList); }

private:

void ExtractCellData( BSPNode* aNode, int& parentNodeIndex, int& rightNodeIndex,
                             float* pg0, float* pm0, 
                             float* pv0, float* pv1, float* pv2, 
                             float* pn0, float* pn1, float* pn2 );

    void initTree();
    void deleteTree();
    void construct(unsigned int fn, AABB& box, std::list<IPrimitive*>& triList, AABB * bounds, unsigned int depth);
    void sahSplit(int &splitaxis, float& splitpoint, float& splitcost, unsigned int size, const AABB& box, boundedge * edge[3]) const;
    void construct(unsigned int fn, AABB& box, unsigned int size, unsigned int depth, bool isleft, unsigned int rIndex, 
        unsigned int pre_size, bool dsize, int tries); // if the size did not change, how many times?    
    unsigned int m_MaxDepth;
    unsigned int m_MaxItemPerNode; 

    enum Split_Mode_Enum { SPLIT_NAIVE, SPLIT_SAH } m_SplitMode; // decide split critiera

    std::list<IPrimitive*> m_PrimList;
    std::list<IModel*>* m_pModelList;
//    const Scene * myScene; // pointer to scene object
    bool m_bBuilt;
    unsigned int m_AbsMax;
    AssistIndex * m_pMultiIndex; // index to more than one model
    unsigned int m_TotalTreeTri; // total triangle in the Kdtree, repeated tri are counted
    unsigned int m_TotalPrimCount; // total primitives in this Kdtree
	AABB m_Bounds;       // Outer bounds of the Kdtree
    BSPNode *m_pTree; // root of kdtree
    std::deque<BSPNode> vTree;
    unsigned int m_TotalNode; // internal node count
    unsigned int m_MaxNode; // max space to allocate at start up
    unsigned int m_NodeUsed;
    float m_IntersectCost;
    float m_TraversalCost;
    float m_EmptyBonus;
    unsigned int totalLeaf;

    boundedge* edge[3];
    IPrimitive** leftBOX;
    unsigned int rSize;
    std::vector<IPrimitive*> rightBOX;
    unsigned int maxReserve;
};
