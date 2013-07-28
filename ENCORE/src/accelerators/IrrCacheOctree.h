//////////////////////////////////////////////////////////////////////////
// ENCORE PROJECT
// 
// Project description and license goes here.
// 
// Developed By
//      Brandon Light:   brandon@alum.wpi.edu
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "Vector3.h"
#include <vector>
#include <list>

#include "Color.h"

using encore::Point3;
using encore::Vector3;

template <class T> class TCacheElement
{
public:
    TCacheElement<T>(){}
    TCacheElement<T>(T data, Point3 location, Vector3 normal, float Ri)
    {
        m_Data = data; 
        m_Location = location; 
        m_Normal = normal; 
        m_Ri = Ri; 
    }

    T& Data(){ return m_Data; }
    Point3& Location(){ return m_Location; }
    Vector3& Normal(){ return m_Normal; }    
    float& Ri(){ return m_Ri; }

    float GetWeight(const Point3& p, const Vector3& n);
    
    double GetD(Point3 p, Vector3 n);

private:
    T m_Data;
    Point3 m_Location;
    Vector3 m_Normal;

    float m_Ri;
};

template <class T> float TCacheElement<T>::GetWeight(const Point3& p, const Vector3& n)
{
    // Equation 11.5 from Pg 141 in Jensen's book
    // weight = 1.0 /( (||xi-x|| /Ri) + Sqrt(1.0-dot(Ni, N)));

    float distance = Vector3(p, m_Location).Magnitude();

    float distCalc = (m_Ri == 0) ? 0 : distance / m_Ri;

    float sqrtCalc = (n == m_Normal) ? 0 : sqrt(1.0 - Dot(n, m_Normal));
    
    float denom = sqrtCalc + distCalc;

    float weight;

    if(denom == 0)
    {
        weight = 0;
    }
    else
    {
        weight = 1/denom;
    }
    return weight;
}

template <class T> double TCacheElement<T>::GetD(Point3 p, Vector3 n)
{
    return 1;
}

template <class T> class CacheValueWeightPair
{
public:
    CacheValueWeightPair(){}
    CacheValueWeightPair(TCacheElement<T> value, float weight){ m_Value = value; m_Weight = weight; }
public:
    TCacheElement<T> m_Value;
    float m_Weight;
};

template <class T> class IrrCache
{
public:
    IrrCache<T>(float a, Point3 minPoint, float xSize, float ySize, float zSize);
    ~IrrCache<T>();

    // adds the element to the octree
    void AddElement(TCacheElement<T> element);

    // clear all elements from this tree
    void ClearAll();

    // fills a list with all of the elements within distance of point p
    float GetIrrValuesNearPoint(Point3 p, Vector3 normal, std::list< CacheValueWeightPair<T> > &nearElements);

    float ExGetValues(Point3 p, Vector3 normal, std::list< CacheValueWeightPair<T> > &nearElements);

private:
    IrrCache<T>(IrrCache<T> *parent, float a, Point3 minPoint, float xSize, float ySize, float zSize);

    // creates children, subdivides this voxel into 8 smaller voxels
    void CreateChildren();

    // gets the child index for the point
    int FindChildIndex(Point3 p);
    // determines if the point is in the node
    bool IsInNode(Point3 p);

    // determines whether to search this node for points near p
    bool SearchNode(Point3 p);

private: // DATA
    IrrCache<T> *m_Parent;

    Point3 m_MinPoint;
    Point3 m_Center;
    Point3 m_Size;  // the width,length,depth of the octree stored as a point

    float m_A;  // user configurable value

    bool m_IsLeafNode;

    int m_cElements; // number of elements in tree

    /* indexes by octant
    0 x,y,z
    1 -x,y,z
    2 x,-y,z
    3 -x,-y,z
    4 x,y,-z
    5 -x,y,-z
    6 x,-y,-z
    7 -x,-y,-z
    */
    std::vector< IrrCache<T>* > m_Children;
    
    std::list< TCacheElement<T> > m_Elements;  // elements in this node
};

template <class T> IrrCache<T>::IrrCache(float a, Point3 minPoint, float xSize, float ySize, float zSize)
{
    m_A = a;

    m_MinPoint = minPoint;
    m_Size = Point3(xSize, ySize, zSize);

    m_Center = m_MinPoint;
    m_Center.X() += xSize / 2.0f;
    m_Center.Y() += ySize / 2.0f;
    m_Center.Z() += zSize / 2.0f;

    int numChildren = 8;
    m_Children.resize(numChildren);
    for(int index = 0; index < numChildren; ++index)
    {
        m_Children[index] = NULL;
    }    

    m_IsLeafNode = true;
    m_cElements = 0;
}

template <class T> IrrCache<T>::IrrCache(IrrCache<T> *parent, float a, Point3 minPoint, float xSize, float ySize, float zSize)
{
    m_Parent = parent;

    m_A = a;

    m_MinPoint = minPoint;
    m_Size = Point3(xSize, ySize, zSize);

    m_Center = m_MinPoint;
    m_Center.X() += xSize / 2.0f;
    m_Center.Y() += ySize / 2.0f;
    m_Center.Z() += zSize / 2.0f;

    int numChildren = 8;
    m_Children.resize(numChildren);
    for(int index = 0; index < numChildren; ++index)
    {
        m_Children[index] = NULL;
    }    

    m_IsLeafNode = true;
    m_cElements = 0;
}
template <class T> IrrCache<T>::~IrrCache<T>()
{
    for(int index = 0; index < (int) m_Children.size(); ++index)
    {
        if(m_Children[index])
        {
            delete m_Children[index];
            m_Children[index] = NULL;
        }
    }
}


template <class T> void IrrCache<T>::AddElement(TCacheElement<T> element)
{
    Point3 maxPoint = m_MinPoint + m_Size;
    if(element.Location().X() < m_MinPoint.X() || maxPoint.X() < element.Location().X() ||
       element.Location().Y() < m_MinPoint.Y() || maxPoint.Y() < element.Location().Y() ||
       element.Location().Z() < m_MinPoint.Z() || maxPoint.Z() < element.Location().Z())
    {
        // location not within bounds of the octree
        return;
    }

    float domain = m_A * element.Ri();
    float sideLength = m_Size.X();

    if((2 * domain) <= sideLength)
    {
        // add it to this node!
        m_Elements.push_back(element);
    }
    else
    {
        // add it to child node...
        if(m_IsLeafNode)
        {
            CreateChildren();
        }
    
        // add to appropriate child node...
        int index = FindChildIndex(element.Location());
        m_Children[index]->AddElement(element);
    }
    
    m_cElements++;
}

template <class T> void IrrCache<T>::CreateChildren()
{
    float xSize = m_Size.X() / 2.0f;
    float ySize = m_Size.Y() / 2.0f;
    float zSize = m_Size.Z() / 2.0f;

    // create children
    for(int index = 0; index < (int) m_Children.size(); ++index)
    {
        Point3 nodeMinPoint = m_MinPoint;
        nodeMinPoint.X() += ((index % 2) < 1) ? xSize : 0; 
        nodeMinPoint.Y() += ((index % 4) < 2) ? ySize : 0;
        nodeMinPoint.Z() += ((index % 8) < 4) ? zSize : 0;

        m_Children[index] = new IrrCache(this, m_A, nodeMinPoint, xSize, ySize, zSize);
    }

    m_IsLeafNode = false;
}

template <class T> void IrrCache<T>::ClearAll()
{
    for(int index = 0; index < (int) m_Children.size(); ++index)
    {
        if(m_Children[index])
        {
            delete m_Children[index];
            m_Children[index] = NULL;
        }
    }

    m_Elements.clear();
    m_cElements = 0;
}


inline bool SampleInFront(const Point3& p, const Point3& sampleP, const Vector3& n, const Vector3& sampleN)
{
    Vector3 navg;
    navg.X() = sampleN.X() + n.X();
    navg.Y() = sampleN.Y() + n.Y();
    navg.Z() = sampleN.Z() + n.Z();

    Vector3 pVec(sampleP, p);
    pVec.Normalize();
    return (Dot(pVec, navg) < -0.1f); 
}

template <class T> float IrrCache<T>::GetIrrValuesNearPoint(Point3 p, Vector3 normal, std::list< CacheValueWeightPair<T> > &nearElements)
{
    float weightSum = 0;

    typename std::list< TCacheElement<T> >::iterator iter = m_Elements.begin();

    double tolerance = 1 / m_A;
    while(iter != m_Elements.end())
    {
        float weight = iter->GetWeight(p, normal);
        if(weight > tolerance && !SampleInFront(p, iter->Location(), normal, iter->Normal()))
        {
                nearElements.push_back(CacheValueWeightPair<T>(*iter, weight));

                weightSum += weight;
        }
        ++iter;
    }

    if(!m_IsLeafNode)
    {
        // find and search all octants that may contain points near p, but do not contain p
        for(int index = 0; index < (int)m_Children.size(); ++index)
        {
            if(m_Children[index]->SearchNode(p))
            {
                weightSum += m_Children[index]->GetIrrValuesNearPoint(p, normal, nearElements);
            }
        }
    }

    return weightSum;
}

template <class T> float IrrCache<T>::ExGetValues(Point3 p, Vector3 normal, std::list< CacheValueWeightPair<T> > &nearElements)
{
    float weightSum = 0;

    typename std::list< TCacheElement<T> >::iterator iter = m_Elements.begin();

    double tolerance = 1 / m_A;
    while(iter != m_Elements.end())
    {
        float weight = iter->GetWeight(p, normal);
        if(weight > tolerance && iter->GetD(p, normal) >= 0)
        {
                nearElements.push_back(CacheValueWeightPair<T>(*iter, weight));

                weightSum += weight;
        }
        ++iter;
    }

    if(!m_IsLeafNode)
    {
        // find and search all octants that may contain points near p, but do not contain p
        for(int index = 0; index < (int)m_Children.size(); ++index)
        {
            weightSum += m_Children[index]->GetIrrValuesNearPoint(p, normal, nearElements);
        }
    }

    return weightSum;
}


template <class T> int IrrCache<T>::FindChildIndex(Point3 p)
{
    Vector3 dir(m_Center, p);
    dir.Normalize();
    /* indexes by octant
    0 x,y,z
    1 -x,y,z
    2 x,-y,z
    3 -x,-y,z
    4 x,y,-z
    5 -x,y,-z
    6 x,-y,-z
    7 -x,-y,-z
    */
    int index = 0;
    index += (dir.X() < 0) ? 1 : 0;
    index += (dir.Y() < 0) ? 2 : 0;
    index += (dir.Z() < 0) ? 4 : 0;
    return index;
}

template <class T> bool IrrCache<T>::IsInNode(Point3 p)
{
    Point3 maxPoint = m_MinPoint + m_Size;
    if(p.X() < m_MinPoint.X() || maxPoint.X() < p.X())
    {
        return false;
    }
    else if(p.Y() < m_MinPoint.Y() || maxPoint.Y() < p.Y())
    {
        return false;
    }
    else if(p.Z() < m_MinPoint.Z() || maxPoint.Z() < p.Z())
    {
        return false;
    }
    else
    {
        return true; // point is within the bounds of this node
    }
}

template <class T> bool IrrCache<T>::SearchNode(Point3 p)
{ 
    float halfSize = m_Size.X() / 2;
    Point3 maxPoint = m_MinPoint + m_Size;

    if(p.X() < m_MinPoint.X() - halfSize || maxPoint.X() + halfSize < p.X())
    {
        return false;
    }
    else if(p.Y() < m_MinPoint.Y() - halfSize || maxPoint.Y() + halfSize < p.Y())
    {
        return false;
    }
    else if(p.Z() < m_MinPoint.Z() - halfSize || maxPoint.Z() + halfSize < p.Z())
    {
        return false;
    }
    else
    {
        return true;
    }
}

