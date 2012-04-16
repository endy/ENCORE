#include "Kdtree.h"
#include <queue>
#include <algorithm>
#include "TPoint3.h"
#include "AABB.h"
#include "IModel.h"
#include <float.h>

using namespace std;

#ifndef INFINITY
#define INFINITY     FLT_MAX
#endif
Kdtree::Kdtree() 
: m_MaxDepth(0), m_MaxItemPerNode(10), m_TotalNode(0), m_MaxNode(32),
  m_bBuilt(false), m_SplitMode(SPLIT_NAIVE), m_NodeUsed(0), m_IntersectCost(80), m_TraversalCost(1),
  m_EmptyBonus(0.15f), m_pTree(NULL), m_AbsMax(65536), totalLeaf(0) 
{
    maxReserve = 0;
    edge[0] = edge[1] = edge[2] = 0;
    leftBOX = 0;
    rSize = 0;
    m_IntersectCost = 10;
}

Kdtree::~Kdtree()
{
    deleteTree();
}

void Kdtree::setMaxDepth(unsigned int d, unsigned int item)
{
    deleteTree();
    m_MaxDepth = d;
    m_MaxItemPerNode = item;
}

void Kdtree::initTree()
{
//    if ( m_pTree )
//        delete [] m_pTree;
//    m_pTree = NULL;
    //m_pTree = new BSPNode[m_MaxNode];
}

void Kdtree::deleteTree()
{
#ifdef SAH
    for(int i = 0; i < 3; i++)
    {
        if(edge[i])
        {
            delete [] edge[i];
            edge[i] = 0;
        }
    }
#endif

    // cautious approach , clear the list before delete the tree
    if(m_pTree)
    {
        for(unsigned int i = 0; i < this->m_MaxNode; i++)
            m_pTree[i].deleteIndex();
        free(m_pTree);
        m_pTree = 0;
    }

    //if(bboxs)
    //    free(bboxs);
    //bboxs = 0;
    if(leftBOX)
        free(leftBOX);
    leftBOX = 0;
    rightBOX.clear();
    rSize = 0;
    m_TotalNode = 0;
    m_TotalTreeTri = 0;
    totalLeaf = 0;
}


void Kdtree::build(std::list<IModel*> &modelList)
{
#ifdef WIN32
    DWORD dwBuildTime = timeGetTime();
#endif
    m_PrimList.clear();
//    if(built) // already built, return
//        return;

    Point3 minPt(INFINITY, INFINITY,INFINITY);
    Point3 maxPt(-INFINITY,-INFINITY,-INFINITY);

    m_pModelList = &modelList;
    // compile all the primitives together
    std::list<IModel*>::iterator m;
    std::list<IPrimitive*>::iterator p;
    for ( m = m_pModelList->begin(); m != m_pModelList->end(); m++ )
    {
        minPt.X() = min((*m)->MinPoint().X(), minPt.X());
        minPt.Y() = min((*m)->MinPoint().Y(), minPt.Y());
        minPt.Z() = min((*m)->MinPoint().Z(), minPt.Z());

        maxPt.X() = max((*m)->MaxPoint().X(), maxPt.X());
        maxPt.Y() = max((*m)->MaxPoint().Y(), maxPt.Y());
        maxPt.Z() = max((*m)->MaxPoint().Z(), maxPt.Z());

        std::list<IPrimitive*> pl = (*m)->getPrimitiveList();
        for ( p = pl.begin(); p != pl.end(); p++ )
        {
            m_PrimList.push_back( (*p) );
        }
    }

    Point3 diff = Point3(0.001f, 0.001f, 0.001f);
    m_Bounds = AABB( minPt-diff, maxPt+diff );
    m_TotalPrimCount = (unsigned int)m_PrimList.size();

    //useSAH(true);
    //printf("total count is %i\n", m_TotalPrimCount);
    // depth base on formula recommanded
    m_MaxDepth = 11+(unsigned int)(1.3*log((float)m_TotalPrimCount));
    m_NodeUsed = 1;
    m_TotalTreeTri = 0;

    if(m_TotalPrimCount <= 5000)
        m_MaxItemPerNode = 2;

    // depth base on formula recommanded
    m_MaxDepth = 11+(unsigned int)(1.3*log((float)m_TotalPrimCount));
    
    //printf("KdTree: building kd tree with max depth : %i\n", m_MaxDepth);

    m_MaxNode = m_TotalPrimCount*2-1;
    // clearing the old data
    deleteTree();

    if(m_MaxNode > maxReserve)
        maxReserve = m_MaxNode;
    // malloc enought space right at the start
    rightBOX.reserve(maxReserve);
    
    m_NodeUsed = 1;

    // allocation edges for sah
#ifdef SAH
    for(int i = 0; i < 3; i++ )
    {
        if(!edge[i])
            edge[i] = new boundedge[2*m_TotalPrimCount];
    }
#endif

    if(!m_pTree)
    {
        m_pTree = (BSPNode*)malloc(sizeof(BSPNode)*(m_MaxNode));
    }
    if(m_pTree == NULL)
    {
        printf("no memory");
        return;
    }
    if(!leftBOX)
    {
        try {
            leftBOX = new IPrimitive*[m_TotalPrimCount]; 
        }
        catch(std::bad_alloc&) {
            printf("no memory"); return; }
    }

    construct(0, m_Bounds, m_TotalPrimCount, 0, 1, 0, 0, 0, 0);
    m_TotalNode += m_NodeUsed;

    maxReserve = (unsigned int)rightBOX.size();

    // not freeing anything
    // they will be freed when next build or kdtree destructor is called
    /*
    if(leftBOX)
    {
        free(leftBOX);
        leftBOX = 0;
    }
    rightBOX.clear();
#ifdef SAH
    for(int i = 0; i < 3; i++)
    {
        delete [] edge[i];
        edge[i] = 0;
    }
#endif
    */
    
	//construct(0, m_Bounds, m_PrimList, 0, 0);
    //printf("KdTree: total leaf node %i\n", totalLeaf);
		
#ifdef WIN32
    dwBuildTime = timeGetTime() - dwBuildTime;
    //printf("KdTree: construction: %.3f\n", dwBuildTime/1000.0f);
#endif
}




// the first 3 value passed in are overwrited to new value
// they are important information on where the best split for the axis is,
// and its cost
// this function return INF cost if a better cost can't be found
void Kdtree::sahSplit(int &splitaxis, float& splitpoint, float& splitcost, unsigned int size, const AABB& box, boundedge * edge[3]) const
{
    int bestaxis = -1;
    float bestsplit = 0;
    float bestcost = INFINITY;
    float oldCost = m_IntersectCost * (float)size;
    float invTotalSA = .5f/box.Area();
    
    int axis = 0; // default to Z axis
    Point3 bsize = box.getSize();
    /*
    if (( bsize.X() > bsize.Y()) && (bsize.X() > bsize.Z()) )
    {
        axis = 0; // X is longest
    }
    else if (( bsize.Y() > bsize.X() ) && ( bsize.Y() > bsize.Z() ) )
    {
        axis = 1; // Y is longest
    }
    */

    bool found = false;
    int nleft = 0, nright = size;
    Point3 d = bsize; // store diagonal

    int otherAxis[3][2] = { {1,2},{0,2},{0,1} };
    
    for(int j = 0; j < 3; j++)
    {
        sort(&edge[axis][0], &edge[axis][2*size]); // sort the axis before perform SAH

        for(unsigned int i = 0; i < 2*size; i++)
        {
            if(edge[axis][i].type == boundedge::end) 
                --nright;
            float edget = edge[axis][i].splitpos;
            Point3 minp = box.getPos();
            Point3 maxp = minp + box.getSize();
            if(edget > minp[axis] && (edget-minp[axis]) > 0.0001 &&edget < maxp[axis])
            {
                
                int otherAxis0 = otherAxis[axis][0];
                int otherAxis1 = otherAxis[axis][1];
                
                float belowSA = 2 * ( d[otherAxis0] * d[otherAxis1] + 
                                      (edget - minp[axis]) *
                                      (d[otherAxis0] + d[otherAxis1]));
                float aboveSA = 2 * ( d[otherAxis0] * d[otherAxis1] + 
                                      (maxp[axis] - edget) *
                                      (d[otherAxis0] + d[otherAxis1]));

                float lcost = belowSA * invTotalSA;
                float rcost = aboveSA * invTotalSA;
                float eb = ( nleft == 0 || nright == 0) ? m_EmptyBonus : 0.f;
                float cost = m_TraversalCost + m_IntersectCost * (1.f - eb) *
                    (lcost * nleft + rcost * nright);

                if(cost < bestcost)
                {
                    bestcost = cost;
                    bestaxis = axis;
                    bestsplit = edget;
                }
            }
            if(edge[axis][i].type == boundedge::start) ++nleft;
        }

        
        if(bestcost < oldCost) // better split found
        {
            found = true;
            //break;
        }
        // go back and retry other axis if best axis is not found
        axis = ++axis;// % 3;
		nleft = 0; 
		nright = size;
    }

    if(!found && bestcost >= oldCost)
        bestcost = INFINITY;

    splitaxis = bestaxis;
    splitpoint = bestsplit;
    splitcost = bestcost;
}

// recursive construction algorithm
//void Kdtree::construct(unsigned int fn, const AABB& box, const std::list<unsigned int>& triList, AABB * bounds, unsigned int depth)
//void Kdtree::construct(unsigned int fn, AABB& box, std::list<IPrimitive*>& primList, AABB * bounds, unsigned int depth)
void Kdtree::construct(unsigned int fn, AABB& box, unsigned int size,
                       unsigned int depth, bool isLeft, unsigned int rIndex, 
                       unsigned int pre_size, bool dsize, int tries)
{
	//BSPNode *node = &m_pTree[fn];
    //printf("max num: %i\n", m_pTree.max_size());
    //m_pTree.assign(BSPNode(), fn);
    BSPNode *node = &m_pTree[fn];

    //unsigned int size = (unsigned int)primList.size();
    //std::list<IPrimitive*>::iterator p;

    //printf(".");
	// if end condition met
    // if(abs(previous size - size) <= 3 ...
    if((int)abs((int)(pre_size-size)) <= 3)
    {
        dsize = true;
        tries++;
    }
    else
    {
        dsize = false;
        tries = 0;
    }
	if(size <= m_MaxItemPerNode || depth >= m_MaxDepth || (dsize && tries == 3))
	{
        IPrimitive** myList = 0;
        if(size > 0)
            myList = new IPrimitive*[size];
        //std::list<IPrimitive*>* myList = new std::list<IPrimitive*>;
        if(depth == 0)
        {
            std::list<IPrimitive*>::iterator tit;
            int i = 0;
            for(tit = m_PrimList.begin(); tit != m_PrimList.end(); tit++, i++)
                myList[i] = (*tit);
        }
        else if(isLeft)
        {
            for(unsigned int i = 0; i < size; i++)
                myList[i] = leftBOX[i];
        }
        else
        {
            for(unsigned int i = 0; i < size; i++)
                myList[i] = rightBOX[rIndex+i];
        }
        node->initLeaf(size, myList);
        
        // increment counts
        m_TotalTreeTri += size;
        totalLeaf++;
		return; // leaf with triangle
	}
    
    // each node must have a split axis and split position
    int axis = 0;
    float splitpos = 0;
    unsigned int i;

#ifdef SAH    

    AABB tbox;
    // build up information of bounding box and edges
    if(depth == 0)
    {
        i = 0;
        std::list<IPrimitive*>::iterator tit;
        for(tit = m_PrimList.begin(); tit != m_PrimList.end(); tit++, i++)
        {
            tbox = (*tit)->getAABB();
            AABB* mybox = &(box.Intersection(tbox)); // get intersection of triangle box + bounding box of this node
            Point3 minp = mybox->getPos();
            Point3 maxp = minp + mybox->getSize();

            // add x position to the list
		    edge[0][i*2].set(minp.X(), (*tit), true); 
		    edge[0][i*2+1].set(maxp.X(), (*tit), false); 			
		    // add y
		    edge[1][i*2].set(minp.Y(), (*tit), true); 
		    edge[1][i*2+1].set(maxp.Y(), (*tit), false);
		    // add z
		    edge[2][i*2].set(minp.Z(), (*tit), true); 
		    edge[2][i*2+1].set(maxp.Z(), (*tit), false);
        }
    }
    else if(isLeft)
    {
        for(i = 0; i < size; i++)
        {
            tbox = leftBOX[i]->getAABB();
            AABB* mybox = &(box.Intersection(tbox)); // get intersection of triangle box + bounding box of this node
            Point3 minp = mybox->getPos();
            Point3 maxp = minp + mybox->getSize();

            // add x position to the list
		    edge[0][i*2].set(minp.X(), 0, true); 
		    edge[0][i*2+1].set(maxp.X(), 0, false); 			
		    // add y
		    edge[1][i*2].set(minp.Y(), 0, true); 
		    edge[1][i*2+1].set(maxp.Y(), 0, false);
		    // add z
		    edge[2][i*2].set(minp.Z(), 0, true); 
		    edge[2][i*2+1].set(maxp.Z(), 0, false);
        }
    }
    else
    {
        for(i = 0; i < size; i++)
        {
            unsigned int tIndex = rIndex + i;
            tbox = rightBOX[tIndex]->getAABB();
            AABB* mybox = &(box.Intersection(tbox)); // get intersection of triangle box + bounding box of this node
            Point3 minp = mybox->getPos();
            Point3 maxp = minp + mybox->getSize();

            // add x position to the list
		    edge[0][i*2].set(minp.X(), 0, true); 
		    edge[0][i*2+1].set(maxp.X(), 0, false); 			
		    // add y
		    edge[1][i*2].set(minp.Y(), 0, true); 
		    edge[1][i*2+1].set(maxp.Y(), 0, false);
		    // add z
		    edge[2][i*2].set(minp.Z(), 0, true); 
		    edge[2][i*2+1].set(maxp.Z(), 0, false);
        }
    }
    
    // calculate SAH and split position
    float cost = INFINITY;
    sahSplit(axis, splitpos, cost, size, box, edge); // figure out best split
    /*
    // old stuff
        if(!bounds)
            bounds = new AABB[size];  // create a container for all triangle bbox
    
        for( i = 0; i < 3; i++ )
        {
            edge[i] = new boundedge[2*size];
        }
        AABB tbox;
        // build up information of bounding box and edges
        for(p = primList.begin(), i = 0; p != primList.end(); p++, i++)
        {
            //AABB* tbox = &(*p)->getAABB();
            if(depth == 0)
            {
                //unsigned int index = (*uit);
                //unsigned int model = getTrueIndex(index);
                //Triangle tt = myScene->getModel(model)->getTriangle(index);
    //            tbox = new AABB(tt);
    //            bounds[i] = (*tbox);

                //IPrimitive p;
                tbox = (*p)->getAABB();
                bounds[i] = tbox;
            }
            else
            {
                tbox = bounds[i];
            }
            
            AABB* mybox = &(box.Intersection(tbox)); // get intersection of triangle box + bounding box of this node
            Point3 minp = mybox->getPos();
            Point3 maxp = minp + mybox->getSize();

            // add x position to the list
		    edge[0][i*2] = boundedge(minp.X(), (*p), true); 
		    edge[0][i*2+1] = boundedge(maxp.X(), (*p), false); 			
		    // add y
		    edge[1][i*2] = boundedge(minp.Y(), (*p), true); 
		    edge[1][i*2+1] = boundedge(maxp.Y(), (*p), false);
		    // add z
		    edge[2][i*2] = boundedge(minp.Z(), (*p), true); 
		    edge[2][i*2+1] = boundedge(maxp.Z(), (*p), false);
        
        }

        // calculate SAH and split position
        float cost = INFINITY;
        sahSplit(axis, splitpos, cost, size, box, edge); // figure out best split
*/
    // no good(better) split position can be found
    if(cost == INFINITY)
	{
        IPrimitive** myList = 0;
        if(size > 0)
            myList = new IPrimitive*[size];
        //std::list<IPrimitive*>* myList = new std::list<IPrimitive*>;
        if(depth == 0)
        {
            std::list<IPrimitive*>::iterator tit;
            int i = 0;
            for(tit = m_PrimList.begin(); tit != m_PrimList.end(); tit++, i++)
                myList[i] = (*tit);
        }
        else if(isLeft)
        {
            for(unsigned int i = 0; i < size; i++)
                myList[i] = leftBOX[i];
        }
        else
        {
            for(unsigned int i = 0; i < size; i++)
                myList[i] = rightBOX[rIndex+i];
        }
        node->initLeaf(size, myList);
        
        // increment counts
        m_TotalTreeTri += size;
        totalLeaf++;
		return; // leaf with triangle
	}
#else
    axis = depth % 3;
    Point3 minp = box.getPos();
    Point3 maxp = minp + box.getSize();
    splitpos = (minp[axis] + maxp[axis])/2;
#endif

    // a split position was calculated,
    // so create left and right nodes
	m_NodeUsed += 2;
    //if(m_NodeUsed == 21)
    //    printf("what");
    //m_pTree.push_back(BSPNode());
    //m_pTree.push_back(BSPNode());

    // if we need more node
    if(m_NodeUsed >= m_MaxNode/* || rSize >= m_MaxNode*3*/)
    {
        try {
            BSPNode *tt = new BSPNode[m_MaxNode*2]; 
            memcpy(tt, m_pTree, m_MaxNode*sizeof(BSPNode));
            free(m_pTree);
            
            m_pTree = tt;
            node = &m_pTree[fn];
            m_MaxNode *= 2;
        }
        catch(std::bad_alloc&) {
            deleteTree();
            printf("no memory\n");
            return;
        }
    }

	// figure out left voxel and right voxel
    Point3 rminp = box.getPos();
    Point3 lmaxp = box.getPos() + box.getSize();

    rminp[axis] = splitpos;
    lmaxp[axis] = splitpos;

	AABB lvoxel( box.getPos(), lmaxp );
    AABB rvoxel( rminp, box.getPos() + box.getSize() );

    int a,b;
    int rcIndex; // right index pass down to child
    bool hit = false;
    a = b = 0;
    if(depth == 0) // special case at depth 0, triangle information are still in the list
    {
        std::list<IPrimitive*>::iterator tit;
        for(tit = m_PrimList.begin(); tit != m_PrimList.end(); tit++)
        {
            if( lvoxel.Intersect( (*tit)->getAABB() ) )
            {
                leftBOX[a] = (*tit);//.push_back(&(*tit));
                a++;
                hit = true;
            }
            if( rvoxel.Intersect( (*tit)->getAABB() ) )
            {
                rightBOX.push_back((*tit));
                //rightBOX[rSize+b] = &(*tit);//.push_back(&(*tit));
                b++;
                hit = true;
            }
            if(!hit)
            {
                leftBOX[a] = (*tit);
                a++;
                rightBOX.push_back((*tit));
                b++;
            }
            hit = false;
        }
        rcIndex = rSize;
        rSize += b;
    }
    else
    {
        if(isLeft) // use left BOX , so data is overwritten
        {
            for(i = 0; i < size; i++)
            {
                if( lvoxel.Intersect( leftBOX[i]->getAABB() ) )
                {
                    leftBOX[a] = leftBOX[i];//.push_back(&(*tit));
                    hit = true;
                    a++;
                }
                if( rvoxel.Intersect( leftBOX[i]->getAABB() ) )
                {
                    rightBOX.push_back(leftBOX[i]);
                    //rightBOX[rSize+b] = primList[i];//.push_back(&(*tit));
                    b++;
                    hit = true;
                }
                if(!hit)
                {
                    leftBOX[a] = leftBOX[i];
                    a++;
                    rightBOX.push_back(leftBOX[i]);
                    b++;
                }
                hit = false;
            }
        }
        else // use right BOX, so data is appended
        {
            unsigned int tSize = rIndex+size;
            for(i = rIndex; i < tSize; i++)
            {
                if( lvoxel.Intersect( rightBOX[i]->getAABB() ) )//primList[i]->getAABB() ) )
                {
                    leftBOX[a] = rightBOX[i];//primList[i];//.push_back(&(*tit));
                    a++;
                    hit = true;
                }
                if( rvoxel.Intersect( rightBOX[i]->getAABB()/*primList[i]->getAABB()*/ ) )
                {
                    rightBOX.push_back(rightBOX[i]);
                    //rightBOX[rSize+b] = rightBOX[i];//primList[i];//.push_back(&(*tit));
                    b++;
                    hit = true;
                }
                if(!hit)
                {
                    leftBOX[a] = rightBOX[i];
                    a++;
                    rightBOX.push_back(rightBOX[i]);
                    b++;
                }
                hit = false;
            }
        }
        rcIndex = rSize;
        rSize += b;
    }

    // old stuff
    /*
	// distribute primitives
    std::list<IPrimitive*> leftList;
    std::list<IPrimitive*> rightList;
    
    for(p = primList.begin(); p != primList.end(); p++)
    {
        //i/f(fn == 20)
        //    printf("why\n");
        if( lvoxel.Intersect( (*p)->getAABB() ) )
        {
            leftList.push_back(*p);
        }
        if( rvoxel.Intersect( (*p)->getAABB() ) )
        {
            rightList.push_back(*p);
        }
    }
    */

	// produce branch
    node->initNode(axis, splitpos);
	node->leftChild = m_NodeUsed-2;
	unsigned int left = node->leftChild;

    construct(left, lvoxel, a, depth+1, 1, 0, size, dsize, tries);    
    construct(left+1, rvoxel, b, depth+1, 0, rcIndex, size, dsize, tries);
}

HitInfo Kdtree::intersect( Ray &r )
{
    HitInfo hit;
    hit.hitTime = INFINITY;
    Point3 invDir(1.f/r.Direction().X(), 1.f/r.Direction().Y(), 1.f/r.Direction().Z());
    float tmin = 0;
    float tmax = 0;

    // intersect bounding box
	if(!m_Bounds.Intersect(r, tmin, tmax))
    {
        return hit;
    }

    bool done = false;	
	list<TODO> todo;
	BSPNode *current = &m_pTree[0];	

	while(!done)
	{
		if(hit.hitTime < tmin) break;
		if(!current->isLeaf())
		{
			int axis = current->splitAxis();
            float split = current->SplitPos();
            split = (split > -EPSILON && split < EPSILON) ? 0 : split;
			float tplane = (split - r.Origin()[axis]) * invDir[axis];

            BSPNode *first, *second;
			// decide which child to check first
			int below = r.Origin()[axis] <= split;
			if(below)
			{
				first = &m_pTree[current->leftChild];
				second = &m_pTree[current->leftChild+1];
			}
			else
			{
				second = &m_pTree[current->leftChild];
				first = &m_pTree[current->leftChild+1];
			}
			
            if(tplane == 0)//(tplane > -EPS && tplane < EPS)
            {
                if(invDir[axis] > 0)
                    current = second;
                else
                    current = first;
            }
            else if(tplane >= tmax || tplane < 0)
				current = first;
			else if(tplane <= tmin)
				current = second;
			else
			{
				TODO temp;
				temp.node = second;
				temp.tmax = tmax;
				temp.tmin = tplane;
				todo.push_front(temp);
				current = first;
				tmax = tplane;
			}
		}
		else
		{
			// check all intersect in this leaf
            std::list<IPrimitive*>::const_iterator it;
	
            if(current->m_pPrimList)
            {
                for(int i = 0; i < current->nTriangle(); i++)
			    {
                    HitInfo thishit = current->m_pPrimList[i]->intersect( r );
                    if( thishit.hitTime < hit.hitTime )
                    {
                        hit = thishit;
                    }
			    } // end checking all Primitives in this leaf
            }
			if(!todo.empty())
			{
				TODO temp = (*todo.begin());
				current = temp.node;
				tmin = temp.tmin;
				tmax = temp.tmax;
				todo.pop_front();
			}
			else
            {
				done = true;
            }

		} // end check leaf
	} // end while loop

	return hit;
}


/*
// an intersect that returns just a bool is a bit faster for
// determining shadows
bool Kdtree::intersectb(const ray &r) const
{
    float3 invDir(1.f/r.dir.X(), 1.f/r.dir.Y(), 1.f/r.dir.Z());
    float tmin = 0;
    float tmax = 0;
    bool found = false;
 
    // intersect bounding box
	if(!myBound.intersect(r, tmin, tmax))
        return found;
    
    bool done = false;	
	list<TODO> todo;
	BSPNode *current = &tree[0];	

	while(!done)
	{
		if(found) break;
		if(!current->isLeaf())
		{
			int axis = current->splitAxis();
			float tplane = (current->SplitPos() - r.orig[axis]) * invDir[axis];
			
            BSPNode *first, *second;
			// decide which child to check first
			int below = r.orig[axis] <= current->SplitPos();
			if(below)
			{
				first = &tree[current->leftChild];
				second = &tree[current->leftChild+1];
			}
			else
			{
				second = &tree[current->leftChild];
				first = &tree[current->leftChild+1];
			}
			if(tplane > tmax || tplane < 0)
				current = first;
			else if(tplane < tmin)
				current = second;
			else
			{
				TODO temp;
				temp.node = second;
				temp.tmax = tmax;
				temp.tmin = tplane;
				todo.push_front(temp);
				current = first;
				tmax = tplane;
			}
		}
		else
		{
			// check all intersect in this leaf
			list<unsigned int>::const_iterator it;
	
			for(it = current->index->begin(); it != current->index->end(); it++)
			{
                unsigned int indexID = (*it);
                unsigned int mid = getTrueIndex(indexID);
                Triangle tt = myScene->getModel(mid)->getTriangle(indexID);
                if(tt.intersect(r))
                {
                    //model = mid; index = indexID;
                    found = true;
                    break;                    
                }
			} // end checking all triangles in ths leaf
			if(!todo.empty() && !found)
			{
				TODO temp = (*todo.begin());
				current = temp.node;
				tmin = temp.tmin;
				tmax = temp.tmax;
				todo.pop_front();
			}
			else
				done = true;

		} // end check leaf
	} // end while loop

	return found;
}
*/

#ifdef WIN32
void Kdtree::buildGPU( std::list<IModel*> &modelList, 
                       std::list<Triangle*> &triangleList, 
                       GPUAccelerationStructureData& accelStructData )
{
    // first build for CPU
    build( modelList );

    // set GPU data sizes
    accelStructData.setTraversalShaderFilename( "shaders/Kdtree_restart.cg" );
    accelStructData.setCellData0Count( m_NodeUsed );
    accelStructData.setVertexCount( m_TotalTreeTri * 3 );
    accelStructData.setNormalCount( m_TotalTreeTri * 3 );
    accelStructData.setMaterialCount( m_TotalTreeTri );

    // store the triangle data using pointers into the current lists
    // this is faster than using indices
    float* pv0 = accelStructData.m_pVertex0Array;
    float* pv1 = accelStructData.m_pVertex1Array;
    float* pv2 = accelStructData.m_pVertex2Array;
    float* pn0 = accelStructData.m_pNormal0Array;
    float* pn1 = accelStructData.m_pNormal1Array;
    float* pn2 = accelStructData.m_pNormal2Array;
    float* pm0 = accelStructData.m_pMaterial0Array;
    float* pg0 = accelStructData.m_pCellData0;
    
    Vertex v;
    Material m;

    // then extract out information
    float currentTriangleIndex = 0;
    for ( unsigned int i = 0; i < m_TotalNode; i++ )
    {
        if ( !m_pTree[i].isLeaf() )
        {
            // this is an Intermediate node
            (*pg0) = (float)m_pTree[i].leftChild;       pg0++;
            (*pg0) = m_pTree[i].SplitPos();             pg0++;
            (*pg0) = 0;                               pg0++;
            (*pg0) = (float)-(m_pTree[i].splitAxis()+2);pg0++;
        }
        else
        {
            // this is a leaf node
            (*pg0) = currentTriangleIndex;          pg0++;
            (*pg0) = 0;                             pg0++;
            (*pg0) = 0;                             pg0++;
            (*pg0) = (float)m_pTree[i].nTriangle();   pg0++;

            //std::list< IPrimitive* >& pPrims = *(m_pTree[i].m_pPrimList);
            //std::list< IPrimitive* >& pPrims = *(m_pTree[i].m_pPrimList);
            //std::list< IPrimitive* >::iterator ip;
            for (int ti = 0; ti < m_pTree[i].nTriangle(); ti++)//(ip = pPrims.begin(); ip != pPrims.end(); ip++ )
            {
                if(i == 4829)
                    printf("why");
                int abc = m_pTree[i].nTriangle();
                std::list< Triangle* >* pTris = m_pTree[i].m_pPrimList[ti]->getNewTesselation();
                std::list< Triangle* >::iterator itp;

                currentTriangleIndex += pTris->size();

                for ( itp = pTris->begin(); itp != pTris->end(); itp++ )
                {
                    // Material
                    m = *((*itp)->getMaterial());

                    *pm0 = m.GetDiffuse().R();          pm0++;
                    *pm0 = m.GetDiffuse().G();          pm0++;
                    *pm0 = m.GetDiffuse().B();          pm0++;
                    *pm0 = m.GetSpecularExponent();     pm0++;

                    // vertex 0
                    v = *((*itp)->getVertex0());

                    *pv0 = v.getCoordinates().X();    pv0++;
                    *pv0 = v.getCoordinates().Y();    pv0++;
                    *pv0 = v.getCoordinates().Z();    pv0++;
                    *pv0 = 0.0f;                      pv0++;

                    *pn0 = v.getNormal().X();     pn0++;
                    *pn0 = v.getNormal().Y();     pn0++;
                    *pn0 = v.getNormal().Z();     pn0++;
                    *pn0 = 0.0f;                  pn0++;
                    
                    // vertex 1
                    v = *((*itp)->getVertex1());

                    *pv1 = v.getCoordinates().X();    pv1++;
                    *pv1 = v.getCoordinates().Y();    pv1++;
                    *pv1 = v.getCoordinates().Z();    pv1++;
                    *pv1 = 0.0f;                      pv1++;

                    *pn1 = v.getNormal().X();     pn1++;
                    *pn1 = v.getNormal().Y();     pn1++;
                    *pn1 = v.getNormal().Z();     pn1++;
                    *pn1 = 0.0f;                  pn1++;

                    // vertex 2
                    v = *((*itp)->getVertex2());

                    *pv2 = v.getCoordinates().X();    pv2++;
                    *pv2 = v.getCoordinates().Y();    pv2++;
                    *pv2 = v.getCoordinates().Z();    pv2++;
                    *pv2 = 0.0f;                      pv2++;

                    *pn2 = v.getNormal().X();     pn2++;
                    *pn2 = v.getNormal().Y();     pn2++; 
                    *pn2 = v.getNormal().Z();     pn2++;
                    *pn2 = 0.0f;                  pn2++;

                    // cleanup New Triangles from getNewTesselation()
                    delete *itp;
                    *itp = NULL;
                } // triangles

                pTris->clear();
                delete pTris;
                pTris = NULL;
            } // prims
        }// end leaf node
    } // end node loop
}

void Kdtree::setGPUParameters( CShader& shader, GPUAccelerationStructureData& accelStructData ) 
{
    CGparameter& cell_info = shader.GetNamedParameter("cellData0", true);
    cgGLSetTextureParameter(cell_info, accelStructData.m_CellTexture[0]);

    //accelStructData.SaveTextureData();

    CGparameter& v0 = shader.GetNamedParameter("v0t", true);
    cgGLSetTextureParameter(v0, accelStructData.m_VertexTexture[0]);

    CGparameter& v1 = shader.GetNamedParameter("v1t", true);
    cgGLSetTextureParameter(v1, accelStructData.m_VertexTexture[1]);

    CGparameter& v2 = shader.GetNamedParameter("v2t", true);
    cgGLSetTextureParameter(v2, accelStructData.m_VertexTexture[2]);

/*
    CGparameter& maxDepth = shader.GetNamedParameter("maxDepth");
    cgGLSetParameter1f( maxDepth, (float) m_MaxDepth );
*/


    Point3 minPt = m_Bounds.getPos();
    Point3 maxPt = minPt + m_Bounds.getSize();

    CGparameter& minPoint = shader.GetNamedParameter("gmin");
    cgGLSetParameter3f( minPoint, minPt.X(), minPt.Y(), minPt.Z() );

    CGparameter& maxPoint = shader.GetNamedParameter("gmax");
    cgGLSetParameter3f( maxPoint, maxPt.X(), maxPt.Y(), maxPt.Z() );

    CGparameter& maxloop = shader.GetNamedParameter("maxloop");
    cgGLSetParameter1f( maxloop, 1000 );
}

#endif
