#pragma once
#include "AccelerationStructure.h"
#include "Common.h"
#include "HitInfo.h"
#include "Primitive.h"
#include "Triangle.h"
#include "Model.h"
#include "AABB.h"
#include <vector>
#include <list>

#define USEUPDATE

// 36 byte node
struct bvhNode
{
    AABB bound;  // 24
    union  // 4
    {
        unsigned int leftChild;
        IPrimitive* pTris;
    };
    unsigned int extra;  // 4
    unsigned int escapeOffset; // 4

    void initLeaf(const AABB& box, IPrimitive* t)
	{
        bound = box;
		pTris = t;
        extra = 1;
	}
	void initNode(const AABB& box, unsigned int s)
	{
		bound = box;
		leftChild = s;
        extra = 0;
	}

};

struct TINFO
{
    unsigned int index;
    float tmin;

    TINFO(unsigned int i, float t)
    {
        index = i;
        t = tmin;
    }
};

class Bvh :
    public AccelerationStructure
{
public:
    Bvh(void);
    ~Bvh(void);

    void usage( void ){}
    void configure( Options* l_pOptions ) {}
    void keyboard( unsigned char key ) {}

    virtual void build(std::list<IModel*> &modelList);
    
#ifdef WIN32
		virtual void buildGPU(std::list<IModel*> &modelList, std::list<Triangle*> &triangleList, GPUAccelerationStructureData& l_pASD );
		virtual void setGPUParameters( CShader& l_Shader, GPUAccelerationStructureData& l_ASD );
#endif
		
		HitInfo intersect( Ray& l_pRay );

    //virtual void update(const list<Primitive*>& primList, list<Triangle>* triangleList);

    unsigned int maxDepth; // not used in anything.. just simple report, so no harm for it to be public

    virtual void update(std::list<IModel*> &modelList);
    // temp function for testing
    //void draw();
private:
    AABB myBound;
    unsigned int myTriSize;
    unsigned int maxSize;
    //list<Triangle>* m_triList;
    IPrimitive** leftBOX;
    unsigned int rSize;
    std::vector<IPrimitive*> rightBOX;

    // mm to work with old encore you need these
    std::list<IPrimitive*> primList;
    //std::list<IModel*>* modelList;
    // update strcutre
    //unsigned int** childLink;
    //unsigned int cIndex;
    //unsigned int* childLinkSize;

    
    bvhNode* bTree; // size is always 2n-1 each leaf only have 1 object
    void deleteTree();
    // mode = 0 , depth == 0 , mode = 1, left , mode = 2..else, right
    void trace(unsigned int node, unsigned int* path, unsigned int depth);
    void split(int mode, float median, int axis, AABB &lAABB, AABB &rAABB, 
           unsigned int &lefts, unsigned int &rights, unsigned int size, unsigned int rIndex);
    void splitinsert(int mode, float median, int axis, unsigned int size, unsigned int rIndex);
    void halfsplitinsert(int mode, AABB &lAABB, AABB &rAABB, 
           unsigned int &lefts, unsigned int &rights, unsigned int size, unsigned int rIndex, unsigned int &rcIndex);
    void construct(unsigned int node, bool isLeft, unsigned int size, const AABB& pAABB,
                    unsigned int rIndex, int depth);
    unsigned int m_NodeUsed;
    unsigned int maxReserve;

};
