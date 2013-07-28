#include "BVH.h"

#ifdef WIN32
#include <GL/gl.h>
#elif __APPLE__
#include <OpenGL/gl.h>
#endif

#include <deque>

#define STUB 1

Bvh::Bvh(void)
: myTriSize(0), m_NodeUsed(0), bTree(NULL), leftBOX(NULL), rSize(0), maxDepth(0)
{
    maxReserve = 32;
}

void Bvh::deleteTree()
{
    /*
    if(childLink)
    {
        delete [] childLinkSize;
        for(unsigned int i = 0; i < myTriSize; i++)
            delete [] childLink[i];
        delete [] childLink;
        childLink = 0;
        cIndex = 0;
    }
    */
    
    maxDepth = 0;
    if(bTree)
        delete [] bTree;
    bTree = 0;
    m_NodeUsed = 0;
    if(leftBOX)
        delete [] leftBOX;
    leftBOX = 0;
    rSize = 0;
    //m_NodeUsed = 0;
    if(!rightBOX.empty())
        rightBOX.clear();
}

Bvh::~Bvh(void)
{
    deleteTree();
}

void Bvh::update(std::list<IModel*> &modelList)
{
    if(!bTree)
        return;

    primList.clear();
    
    Point3 minPt(POS_INF, POS_INF,POS_INF);
    Point3 maxPt(-POS_INF,-POS_INF,-POS_INF);
    myTriSize = 0;

    std::list<IModel*>::iterator model;
    std::list<IPrimitive*>::iterator p;

    for( model = modelList.begin(); model != modelList.end(); model++ )
    {
        myTriSize += (*model)->GetPrimitiveCount();

        // update uniform grids min and max values;
        minPt.X() = min((*model)->MinPoint().X(), minPt.X());
        minPt.Y() = min((*model)->MinPoint().Y(), minPt.Y());
        minPt.Z() = min((*model)->MinPoint().Z(), minPt.Z());

        maxPt.X() = max((*model)->MaxPoint().X(), maxPt.X());
        maxPt.Y() = max((*model)->MaxPoint().Y(), maxPt.Y());
        maxPt.Z() = max((*model)->MaxPoint().Z(), maxPt.Z());

        std::list<IPrimitive*>* pl = &(*model)->getPrimitiveList();
        for ( p = pl->begin(); p != pl->end(); p++ )
        {
            primList.push_back( (*p) );
        }
    }

    myBound = AABB( minPt, maxPt );
    unsigned int *temp = new unsigned int[myTriSize];
    // check as we go
    trace(0, temp, 0);
    delete [] temp;
    bTree[0].bound = myBound;
}

void Bvh::trace(unsigned int node, unsigned int *path, unsigned int depth)
{
    bvhNode* current = &bTree[node];
    
    if(depth > maxDepth)
        maxDepth = depth;
    if(current->extra)
    {
        //if(randNumber(0.f, 3.f) > 1.4f)
        if(true)
        //if(current->bound != current->pTris->getAABB())
        {
            current->bound = current->pTris->getAABB(); // assign new box
            // propergate up
            for(int i = depth-2; i > 0; i--)
            {
                //printf("get here\n");
                bTree[path[i]].bound += current->pTris->getAABB();
            }
            
        }
        return;
    }
    path[depth] = current->leftChild;
    trace(current->leftChild, path, depth+1);
    path[depth] = current->leftChild+1;
    trace(current->leftChild+1, path, depth+1);    
}

void Bvh::build(std::list<IModel*> &modelList)
{
    DWORD dwBuildTime = EncoreGetTime();

    primList.clear();
   
    // destory the old information regardless
    deleteTree();
    rightBOX.reserve(maxReserve);
    
    Point3 minPt(POS_INF, POS_INF,POS_INF);
    Point3 maxPt(-POS_INF,-POS_INF,-POS_INF);
    myTriSize = 0;

    std::list<IModel*>::iterator model;
    std::list<IPrimitive*>::iterator p;

    for( model = modelList.begin(); model != modelList.end(); model++ )
    {
        myTriSize += (*model)->GetPrimitiveCount();

        // update uniform grids min and max values;
        minPt.X() = min((*model)->MinPoint().X(), minPt.X());
        minPt.Y() = min((*model)->MinPoint().Y(), minPt.Y());
        minPt.Z() = min((*model)->MinPoint().Z(), minPt.Z());

        maxPt.X() = max((*model)->MaxPoint().X(), maxPt.X());
        maxPt.Y() = max((*model)->MaxPoint().Y(), maxPt.Y());
        maxPt.Z() = max((*model)->MaxPoint().Z(), maxPt.Z());

        std::list<IPrimitive*>* pl = &(*model)->getPrimitiveList();
        for ( p = pl->begin(); p != pl->end(); p++ )
        {
            primList.push_back( (*p) );
        }
    }

    myBound = AABB( minPt, maxPt );
    
    // bvh size is always 2n-1
    maxSize = 2*myTriSize-1;
    bTree = new bvhNode[maxSize];//(bvhNode*)malloc((maxSize)*sizeof(bvhNode));
    if(bTree == NULL)
    {
        printf("no memory to make bvh\n");
        return;
    }
    try {
        leftBOX = new IPrimitive*[myTriSize]; }
    catch(std::bad_alloc&) {
        printf("no memory"); return; }
        
    m_NodeUsed = 1;
    
    construct(0, 1, myTriSize, myBound, 0, 0);
    
    // clear junk
    //if(leftBOX)
    //    free(leftBOX);   
    //leftBOX = 0;
    //rightBOX.clear();
    maxReserve = rightBOX.size();

    dwBuildTime = EncoreGetTime() - dwBuildTime;
}

void Bvh::construct(unsigned int node, bool isLeft, unsigned int size, const AABB& pAABB,
                    unsigned int rIndex, int depth)
{
    bvhNode* current = &bTree[node];

    current->escapeOffset = 2*size - 1;

    // special case where there is only 2,1,or 0 triangle at depth 0
    if(!depth && size <= 2)
    {
        list<IPrimitive*>::iterator tit = primList.begin();
        if(size == 2)
        {
            m_NodeUsed += 2;
            current->initNode(pAABB, m_NodeUsed-2);
            bTree[current->leftChild].initLeaf((*tit)->getAABB(), (*tit));
            tit++;
            bTree[current->leftChild+1].initLeaf((*tit)->getAABB(), (*tit));
        }
        else if(size)
            current->initLeaf((*tit)->getAABB(), (*tit));
        else
            current->initLeaf(AABB(), 0);

        return;
    }

    // size is one, make leaf
    if(size == 1)
    {
        if(isLeft)
        {
            current->initLeaf(leftBOX[0]->getAABB(), leftBOX[0]);
            return;
        }
        current->initLeaf(rightBOX[rIndex]->getAABB(), rightBOX[rIndex]);
        return;
    }

    // size is two, make 2 leaf and 1 node
    if(size == 2)
    {
        m_NodeUsed += 2;
        current->initNode(pAABB, m_NodeUsed-2);
        if(isLeft)
        {
            bTree[current->leftChild].initLeaf(leftBOX[0]->getAABB(), leftBOX[0]);
            bTree[current->leftChild+1].initLeaf(leftBOX[1]->getAABB(), leftBOX[1]);
        }
        else
        {
            bTree[current->leftChild].initLeaf(rightBOX[rIndex]->getAABB(), rightBOX[rIndex]);
            bTree[current->leftChild+1].initLeaf(rightBOX[rIndex+1]->getAABB(), rightBOX[rIndex+1]);
        }
        return;
    }

    unsigned int a,b;
    // alternate axis
    int axis = depth%3;

    // axis size / 2 + axis lowest position = median
    float median = pAABB.getPos()[axis]+pAABB.getSize()[axis]/2;
    

    AABB lAABB, rAABB;    
    unsigned int rcIndex = 0;
    bool valid = false;
    // no data is assigned to left and right child, first pass
    // this is to find if the split produce non zero result
    int mode;
    if(!depth)
        mode = 2;
    else
        mode = isLeft;

    for(int i = 0; i < 3; i++)
    {
        split(mode, median, axis, lAABB, rAABB, a, b, size, rIndex);
        // if the split produce zero branch
        // try out other axis
        if(!a || !b)
        {
            valid = false;
            axis = (axis+1)%3;
            median = pAABB.getPos()[axis]+pAABB.getSize()[axis]/2;
            lAABB = rAABB = AABB();
            a = b = 0;
        }
        else // valid hit is found break out
        {
            rcIndex = rSize;
            rSize += b;
            valid = true;
            break;
        }
    }

    if(valid)
    {
        splitinsert(mode, median, axis, size, rIndex);
    }
    else
    {
        halfsplitinsert(mode, lAABB, rAABB, a, b, size, rIndex, rcIndex);
    }

    // allocate 2 more space
    m_NodeUsed += 2;
    if(m_NodeUsed >= maxSize)
    {
        // allocate more memory
        printf("...");
    }

    // produce branch
    // total node start at 1
    //unsigned int leftn = m_totalNode-2;
    current->initNode(pAABB, m_NodeUsed-2);
    
    construct(current->leftChild, true, a, lAABB, 0, depth+1);    
    construct(current->leftChild+1, false, b, rAABB, rcIndex, depth+1);

}

void Bvh::halfsplitinsert(int mode, AABB &lAABB, AABB &rAABB, 
           unsigned int &lefts, unsigned int &rights, unsigned int size, unsigned int rIndex, unsigned int &rcIndex)
{
    lefts = rights = 0;
    unsigned int hs = size/2;
    if(mode == 2)
    {
        list<IPrimitive*>::iterator tit;
        for(tit = primList.begin(); tit != primList.end(); tit++)
        {
            if(lefts < hs)
            {
                if(!lefts)
                    lAABB = (*tit)->getAABB();
                else
                    lAABB += (*tit)->getAABB();
                leftBOX[lefts] = (*tit);
                lefts++;
            }
            else
            {
                if(!rights)
                    rAABB = (*tit)->getAABB();
                else
                    rAABB += (*tit)->getAABB();
                rightBOX.push_back((*tit));
                rights++;
            }
        }        
    }
    else if(mode == 1)
    {
        for(unsigned int i = 0; i < size; i++)
        {
            if(lefts < hs)
            {
                if(!lefts)
                    lAABB = leftBOX[i]->getAABB();
                else
                    lAABB += leftBOX[i]->getAABB();
                leftBOX[lefts] = leftBOX[i];
                lefts++;
            }
            else
            {
                if(!rights)
                    rAABB = leftBOX[i]->getAABB();
                else
                    rAABB += leftBOX[i]->getAABB();
                rightBOX.push_back(leftBOX[i]);
                rights++;
            }
        }
    }
    else
    {
        unsigned int tSize = rIndex+size;
        for(unsigned int i = rIndex; i < tSize; i++)
        {
            if(lefts < hs)
            {
                if(!lefts)
                    lAABB = rightBOX[i]->getAABB();
                else
                    lAABB += rightBOX[i]->getAABB();
                leftBOX[lefts] = rightBOX[i];
                lefts++;
            }
            else
            {
                if(!rights)
                    rAABB = rightBOX[i]->getAABB();
                else
                    rAABB += rightBOX[i]->getAABB();
                rightBOX.push_back(rightBOX[i]);
                rights++;
            }
        }            
    }
    rcIndex = rSize;
    rSize += rights;
    // end of split
}

void Bvh::splitinsert(int mode, float median, int axis, unsigned int size, unsigned int rIndex)
{
    int a = 0;
    if(mode == 2)
    {
        list<IPrimitive*>::iterator tit;
        for(tit = primList.begin(); tit != primList.end(); tit++)
        {
            if((*tit)->getAABB().lessThan(median, axis))
            {
                leftBOX[a] = (*tit);
                a++;
            }
            else
                rightBOX.push_back((*tit));//[b] = &(*tit);
        }
    }
    else if(mode == 1)
    {
        for(unsigned int i = 0; i < size; i++)
        {
            if(leftBOX[i]->getAABB().lessThan(median, axis))
            {
                leftBOX[a] = leftBOX[i];
                a++;
            }
            else
                rightBOX.push_back(leftBOX[i]);
        }
    }
    else
    {
        unsigned int tSize = rIndex+size;
        for(unsigned int i = rIndex; i < tSize; i++)
        {
            if(rightBOX[i]->getAABB().lessThan(median, axis))
            {
                leftBOX[a] = rightBOX[i];
                a++;
            }
            else
                rightBOX.push_back(rightBOX[i]);
        }        
    }
    // done
}

void Bvh::split(int mode, float median, int axis, AABB &lAABB, AABB &rAABB, 
           unsigned int &lefts, unsigned int &rights, unsigned int size, unsigned int rIndex)
{
    lefts = rights = 0;
    if(mode == 2)
    {
        list<IPrimitive*>::iterator tit;
        for(tit = primList.begin(); tit != primList.end(); tit++)
        {
            if((*tit)->getAABB().lessThan(median, axis))
            {
                if(!lefts)
                    lAABB = (*tit)->getAABB();
                else
                    lAABB += (*tit)->getAABB();
                //leftBOX[a] = &(*tit);
                lefts++;
            }
            else
            {
                if(!rights)
                    rAABB = (*tit)->getAABB();
                else
                    rAABB += (*tit)->getAABB();
                //rightBOX.push_back(&(*tit));//[b] = &(*tit);
                rights++;
            }
        }
        //rcIndex = rSize;
        //rSize += b;
    }
    else if(mode == 1)
    {
        for(unsigned int i = 0; i < size; i++)
        {
            if(leftBOX[i]->getAABB().lessThan(median, axis))
            {
                if(!lefts)
                    lAABB = leftBOX[i]->getAABB();
                else
                    lAABB += leftBOX[i]->getAABB();
                //leftBOX[a] = leftBOX[i];
                lefts++;
            }
            else
            {
                if(!rights)
                    rAABB = leftBOX[i]->getAABB();
                else
                    rAABB += leftBOX[i]->getAABB();
                //rightBOX.push_back(leftBOX[i]);
                rights++;
            }
        }
        //rcIndex = rSize;
        //rSize += b;
    }
    else
    {
        unsigned int tSize = rIndex+size;
        for(unsigned int i = rIndex; i < tSize; i++)
        {
            if(rightBOX[i]->getAABB().lessThan(median, axis))
            {
                if(!lefts)
                    lAABB = rightBOX[i]->getAABB();
                else
                    lAABB += rightBOX[i]->getAABB();
                //leftBOX[a] = rightBOX[i];
                lefts++;
            }
            else
            {
                if(!rights)
                    rAABB = rightBOX[i]->getAABB();
                else
                    rAABB += rightBOX[i]->getAABB();
                //rightBOX.push_back(leftBOX[i]);
                rights++;
            }
        }
        //rcIndex = rSize;
        //rSize += b;
            
    }
    // end of split
}

void Bvh::buildGPU(std::list<IModel*> &modelList, std::list<Triangle*> &triangleList, GPUAccelerationStructureData& l_pASD )
{
#ifndef STUB
    l_pASD.setTraversalShaderFilename( "shaders\\BVH.cg" );
#ifdef USEUPDATE
    if(bTree)
        update(modelList);
    else
        build(modelList);
#else
    build( modelList );
#endif

    // need enough room for the nodes and the triangles
    l_pASD.setVertexCount( m_NodeUsed * 3 ); 
    l_pASD.setNormalCount( m_NodeUsed * 3 );
    l_pASD.setMaterialCount( m_NodeUsed * 3 );

    float* pV0 = l_pASD.m_pVertex0Array;
    float* pV1 = l_pASD.m_pVertex1Array;
    float* pV2 = l_pASD.m_pVertex2Array;

    float* pN0 = l_pASD.m_pNormal0Array;
    float* pN1 = l_pASD.m_pNormal1Array;
    float* pN2 = l_pASD.m_pNormal2Array;

    float* pM = l_pASD.m_pMaterial0Array;

    std::deque<bvhNode*> todo;
    bvhNode* current = NULL;

    todo.push_back( &bTree[0] );

    int nProcessedNodes = 0;

//    while( nProcessedNodes < ( m_NodeUsed + triangleList.size() ) )
    while ( !todo.empty() ) // while there are nodes to process
    {
        // get the first node to process and then remove it from the queue
        current = todo[0];
        todo.pop_front();

        if( current->extra == 1 ) // is leaf
        {
            std::list< Triangle* >* pTris = current->pTris->getNewTesselation();
            std::list< Triangle* >::iterator t;
            for ( t = pTris->begin(); t != pTris->end(); t++ )
            {
                Triangle* tri = *t;
                Point3 v0 = tri->getVertex0()->getCoordinates();
                Point3 v1 = tri->getVertex1()->getCoordinates();
                Point3 v2 = tri->getVertex2()->getCoordinates();

                Vector3 n0 = tri->getVertex0()->getNormal();
                Vector3 n1 = tri->getVertex1()->getNormal();
                Vector3 n2 = tri->getVertex2()->getNormal();

                // vertices
                *pV0 = v0.X(); ++pV0;
                *pV0 = v0.Y(); ++pV0;
                *pV0 = v0.Z(); ++pV0;
                *pV0 = 0.0f;   ++pV0; // 0 = triangle node

                *pV1 = v1.X();    ++pV1;
                *pV1 = v1.Y();    ++pV1;
                *pV1 = v1.Z();    ++pV1;
                *pV1 = 1.0f; ++pV1;// just go to the next index

                *pV2 = v2.X(); ++pV2;
                *pV2 = v2.Y(); ++pV2;
                *pV2 = v2.Z(); ++pV2;
                *pV2 = 0;      ++pV2; // TODO: Material Pointer

                // normals
                *pN0 = n0.X(); ++pN0;
                *pN0 = n0.Y(); ++pN0;
                *pN0 = n0.Z(); ++pN0;
                *pN0 = 0.0f; ++pN0;

                *pN1 = n1.X(); ++pN1;
                *pN1 = n1.Y(); ++pN1;
                *pN1 = n1.Z(); ++pN1;
                *pN1 = 0.0f; ++pN1;

                *pN2 = n2.X(); ++pN2;
                *pN2 = n2.Y(); ++pN2;
                *pN2 = n2.Z(); ++pN2;
                *pN2 = 0.0f;   ++pN2;

                encore::Color diffuse = tri->getMaterial()->GetDiffuse();
                float spec = tri->getMaterial()->GetSpecularExponent();
                *pM = diffuse.R(); ++pM;
                *pM = diffuse.G(); ++pM;
                *pM = diffuse.B(); ++pM;
                *pM = spec; ++pM;

                
                delete *t;
                *t = NULL;
                // increase node count
                ++nProcessedNodes;
            }
            pTris->clear();
            if(pTris) delete pTris;
            pTris = NULL;
        }
        else // is node
        {
            unsigned int left = current->leftChild;
            unsigned int right = left+1;

            // add child nodes to the queue
//            todo.push_back( &bTree[left] );
//            todo.push_back( &bTree[right] );
            // push the right one first so that we pop the left one first
            todo.push_front( &bTree[right] );
            todo.push_front( &bTree[left] );

            Point3 bMin = current->bound.getPos();
            Point3 bMax = bMin + current->bound.getSize();

            *pV0 = bMin.X(); ++pV0;
            *pV0 = bMin.Y(); ++pV0;
            *pV0 = bMin.Z(); ++pV0;
            *pV0 = 1;        ++pV0; // this is an intermediate node

            *pV1 = bMax.X(); ++pV1;
            *pV1 = bMax.Y(); ++pV1;
            *pV1 = bMax.Z(); ++pV1;
            *pV1 = (float)current->escapeOffset;    ++pV1;//TODO just go to the next node or exit?
                                    // if we miss the bounding box, we might just quit...

            *pV2 = 0; ++pV2; // No data stored here
            *pV2 = 0; ++pV2;
            *pV2 = 0; ++pV2;
            *pV2 = 0; ++pV2;

            // still need to write out fake normals to keep index aligned
            *pN0 = 0.0f; ++pN0;
            *pN0 = 0.0f; ++pN0;
            *pN0 = 0.0f; ++pN0;
            *pN0 = 0.0f; ++pN0;

            *pN1 = 0.0f; ++pN1;
            *pN1 = 0.0f; ++pN1;
            *pN1 = 0.0f; ++pN1;
            *pN1 = 0.0f; ++pN1;

            *pN2 = 0.0f; ++pN2;
            *pN2 = 0.0f; ++pN2;
            *pN2 = 0.0f; ++pN2;
            *pN2 = 0.0f; ++pN2;

            *pM = 0.0f; ++pM;
            *pM = 0.0f; ++pM;
            *pM = 0.0f; ++pM;
            *pM = 0.0f; ++pM;

            // increase node count
            ++nProcessedNodes;
        }
    }
    m_NodeUsed = nProcessedNodes;
#endif
}

void Bvh::setGPUParameters( CShader& l_Shader, GPUAccelerationStructureData& l_ASD )
{
#ifndef STUB
    CGparameter& v0 = l_Shader.GetNamedParameter("v0t", true);
    cgGLSetTextureParameter(v0, l_ASD.m_VertexTexture[0]);

    CGparameter& v1 = l_Shader.GetNamedParameter("v1t", true);
    cgGLSetTextureParameter(v1, l_ASD.m_VertexTexture[1]);

    CGparameter& v2 = l_Shader.GetNamedParameter("v2t", true);
    cgGLSetTextureParameter(v2, l_ASD.m_VertexTexture[2]);

//l_ASD.SaveTextureData( "v0t.txt", l_ASD.m_VertexTexture[0] );
//l_ASD.SaveTextureData( "v1t.txt", l_ASD.m_VertexTexture[1] );
//l_ASD.SaveTextureData( "v2t.txt", l_ASD.m_VertexTexture[2] );

    CGparameter& maxindex = l_Shader.GetNamedParameter("maxIndex");
    cgGLSetParameter1f( maxindex, (float)m_NodeUsed );

    CGparameter& maxloop = l_Shader.GetNamedParameter("looplimit");
    cgGLSetParameter1f( maxloop, 2500.0f );
#endif
}


/*
void Bvh::draw()
{
    bool done = false;
    list<unsigned int> todo;
    bvhNode* current = &bTree[0];
    int leftonly = 0;
    while(!done)
    {
        if(current->extra)
        {
            glBegin(GL_TRIANGLES);
            glNormal3f(current->pTris->getVertex0().getNormal().X(),
                    current->pTris->getVertex0().getNormal().Y(),
                    current->pTris->getVertex0().getNormal().Z());
            glVertex3f(current->pTris->getVertex0().getCoordinates().X(),
                    current->pTris->getVertex0().getCoordinates().Y(),
                    current->pTris->getVertex0().getCoordinates().Z());
            glNormal3f(current->pTris->getVertex1().getNormal().X(),
                    current->pTris->getVertex1().getNormal().Y(),
                    current->pTris->getVertex1().getNormal().Z());
            glVertex3f(current->pTris->getVertex1().getCoordinates().X(),
                    current->pTris->getVertex1().getCoordinates().Y(),
                    current->pTris->getVertex1().getCoordinates().Z());
            glNormal3f(current->pTris->getVertex2().getNormal().X(),
                    current->pTris->getVertex2().getNormal().Y(),
                    current->pTris->getVertex2().getNormal().Z());
            glVertex3f(current->pTris->getVertex2().getCoordinates().X(),
                   current->pTris->getVertex2().getCoordinates().Y(),
                   current->pTris->getVertex2().getCoordinates().Z());
            glEnd();

            if(todo.empty())
                done = true;
            else
            {
                current = &bTree[(*todo.begin())];
                todo.pop_front();
            }
        }
        else
        {
            int left = current->leftChild;
            int right = left+1;
            current = &bTree[left];
            if(!leftonly)
                todo.push_front(right);
            else
                leftonly--;
        }
    }
}
*/

bool Bvh::intersect( Ray& l_pRay, HitInfo* pHitInfo )
{
    bool found = false;
    pHitInfo->hitTime = POS_INF;
    pHitInfo->bHasInfo = false;

    //double maxhit = info.hitTime;
    float tmin, tmax;
    float lhit, rhit,hit1, hit2;
    hit1 = POS_INF;
    
    if(!myBound.Intersect(l_pRay, tmin, tmax))
    {
        return false;
    }

    std::deque<TINFO> todo;
    bool lefthit, righthit;
    lefthit = righthit = false;
    bvhNode* current = &bTree[0];
    while (!found)
    {
        if (current->extra) // is leaf
        {
            if (pHitInfo->hitTime == POS_INF || hit1 < pHitInfo->hitTime)
            {
                HitInfo newinfo;
                current->pTris->intersect(l_pRay, &newinfo);
                if(newinfo.hitTime < pHitInfo->hitTime)
                {
                    *pHitInfo = newinfo;
                    pHitInfo->bHasInfo = true;
                }
            }

            if( !todo.empty())
            {
                current = &bTree[todo[0].index];
                hit1 = todo[0].tmin;
                todo.pop_front();
            }
            else
            {
                found = true;
            }
        }
        else // is node
        {
            lefthit = righthit = false;
            unsigned int left = current->leftChild;
            unsigned int right = left+1;
            if (pHitInfo->hitTime == POS_INF || hit1 < pHitInfo->hitTime)
            {
                if (bTree[left].bound.Intersect(l_pRay, lhit, hit2))
                {
                    lefthit = true;
                }

                if (bTree[right].bound.Intersect(l_pRay, rhit, hit2))
                {
                    righthit = true;
                }
            }
            
            if (lefthit && righthit)
            {
                if (rhit > lhit)
                {
                    current = &bTree[left];
                    todo.push_front(TINFO(right, rhit));
                    hit1 = lhit;
                }
                else
                {
                    current = &bTree[right];
                    todo.push_front(TINFO(left, lhit));
                    hit1 = rhit;
                }
            }
            else if(lefthit)
            {
                current = &bTree[left];
                hit1 = lhit;
            }
            else if(righthit)
            {
                current = &bTree[right];
                hit1 = rhit;
            }
            else
            {
                if(!todo.empty())
                {
                    current = &bTree[todo[0].index];
                    hit1 = todo[0].tmin;
                    todo.pop_front();
                }
                else
                {
                    break; // hit nothing
                }
            }
        }
    }

    return pHitInfo->bHasInfo;
}

