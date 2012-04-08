#pragma once

#include "TVector3.h"
#include <vector>
#include <list>

#ifdef WIN32
#include <windows.h>
#include <GL/glut.h>
#endif

using encore::Point3f;
using encore::Vector3f;

template <class T> class TElement
{
public:
    TElement<T>(T data, Point3f location){ m_Data = data; m_Location = location; }

    T& Data(){ return m_Data; }
    Point3f& Location(){ return m_Location; }

private:
    T m_Data;
    Point3f m_Location;
};

template <class T> class TOctree
{
public:
    TOctree<T>(int elementsPerNode, Point3f minPoint, float xSize, float ySize, float zSize);
    ~TOctree<T>();

    // add an element to the tree at location
    void AddElement(T element, Point3f location);

    // clear all elements from this tree
    void ClearAll();

    // fills a list with all of the elements within distance of point p
    void GetAllElementsNearPoint(Point3f p, float distance, std::list< TElement<T> > &nearElements, int &maxDepth, int depth = 0);

    void ExhaustiveGetAll(Point3f p, float distance, std::list< TElement<T> > &nearElements);

private:
    TOctree<T>(TOctree<T> *parent, int elementsPerNode, Point3f minPoint, float xSize, float ySize, float zSize);

    // adds the element 
    void AddElement(TElement<T> element);
    // gets the child index for the point
    int FindChildIndex(Point3f p);
    // splits this node into 8 subdivisions that become children
    void SplitNode();
    // determines if the point is in the node
    bool IsInNode(Point3f p);

    // determines whether to search this node for points near p
    bool SearchNode(Point3f p, float distance);

private: // DATA
    TOctree<T> *m_Parent;
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
    std::vector< TOctree<T>* > m_Children;
    
    std::list< TElement<T> > m_Elements;  // elements in this node
    const int m_ELEMENTS_PER_NODE;
    
    Point3f m_MinPoint;
    Point3f m_Center;
    Point3f m_Size;  // the width,length,depth of the octree stored as a point

};

template <class T> TOctree<T>::TOctree(int elementsPerNode, Point3f minPoint, float xSize, float ySize, float zSize):
    m_ELEMENTS_PER_NODE(elementsPerNode)
{
    m_MinPoint = minPoint;
    m_Size = Point3f(xSize, ySize, zSize);

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

template <class T> TOctree<T>::TOctree(TOctree<T> *parent, int elementsPerNode, Point3f minPoint, float xSize, float ySize, float zSize):
    m_ELEMENTS_PER_NODE(elementsPerNode)
{
    m_Parent = parent;

    m_MinPoint = minPoint;
    m_Size = Point3f(xSize, ySize, zSize);

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
template <class T> TOctree<T>::~TOctree<T>()
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


template <class T> void TOctree<T>::AddElement(T element, Point3f location)
{
    Point3f maxPoint = m_MinPoint + m_Size;
    if(location.X() < m_MinPoint.X() || maxPoint.X() < location.X() ||
       location.Y() < m_MinPoint.Y() || maxPoint.Y() < location.Y() ||
       location.Z() < m_MinPoint.Z() || maxPoint.Z() < location.Z())
    {
        // location not within bounds of the octree
        return;
    }
    
    AddElement(TElement<T>(element, location));
}

template <class T> void TOctree<T>::ClearAll()
{
    assert(!"THIS METHOD NOT IMPLEMENTED");
}

template <class T> void TOctree<T>::GetAllElementsNearPoint(Point3f p, float distance, std::list< TElement<T> > &nearElements,  int &maxDepth, int depth /* = 0 */)
{
    depth++;

    maxDepth = (maxDepth < depth) ? depth : maxDepth;

    if(m_IsLeafNode)
    {
        typename std::list< TElement<T> >::iterator iter = m_Elements.begin();
        while(iter != m_Elements.end())
        {
            Vector3f v(p, iter->Location());
            if(v.Magnitude() < distance)
            {
                nearElements.push_back(*iter);
            }
            ++iter;
        }
    }
    else
    {
        // find and search all octants that may contain points near p, but do not contain p
        for(int index = 0; index < (int)m_Children.size(); ++index)
        {
            if(SearchNode(p, distance))
            {
                m_Children[index]->GetAllElementsNearPoint(p, distance, nearElements, maxDepth, depth);
            }
        }
    }

#ifdef VIZ_OCTREE
    float scale = (depth / (float) maxDepth);

    glPushMatrix();
    glColor3f(scale * abs(m_Center.X() / 100),scale * abs(m_Center.Y() / 100),scale * abs(m_Center.Z() / 100));
    glTranslatef(m_Center.X(), m_Center.Y(), m_Center.Z());
    glutWireCube(m_Size.X());
    glPopMatrix();
#endif
}

template <class T> void TOctree<T>::ExhaustiveGetAll(Point3f p, float distance, std::list< TElement<T> > &nearElements)
{
    if(m_IsLeafNode)
    {
        typename std::list< TElement<T> >::iterator iter = m_Elements.begin();
        while(iter != m_Elements.end())
        {
            Vector3f v(p, iter->Location());
            if(v.Magnitude() < distance)
            {
                nearElements.push_back(*iter);
            }
            ++iter;
        }
    }
    else
    {
        // find and search all octants that may contain points near p, but do not contain p
        for(int index = 0; index < (int)m_Children.size(); ++index)
        {
            m_Children[index]->ExhaustiveGetAll(p, distance, nearElements);
        }
    }
}

template <class T> void TOctree<T>::AddElement(TElement<T> element)
{
    if(m_IsLeafNode)
    {
        if(m_cElements + 1 < m_ELEMENTS_PER_NODE)
        {
            // room at this node, just add the element to the list
            m_Elements.push_back(element);
        }
        else
        {
            SplitNode();
            int index = FindChildIndex(element.Location());
            m_Children[index]->AddElement(element);
        }
    }
    else
    {
        // element is in a child
        int index = FindChildIndex(element.Location());
        m_Children[index]->AddElement(element);
    }

    m_cElements++;
}

template <class T> int TOctree<T>::FindChildIndex(Point3f p)
{
    Vector3f dir(m_Center, p);
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

template <class T> void TOctree<T>::SplitNode()
{
    float xSize = m_Size.X() / 2.0f;
    float ySize = m_Size.Y() / 2.0f;
    float zSize = m_Size.Z() / 2.0f;

    // create children
    for(int index = 0; index < (int) m_Children.size(); ++index)
    {
        Point3f nodeMinPoint = m_MinPoint;
        nodeMinPoint.X() += ((index % 2) < 1) ? xSize : 0; 
        nodeMinPoint.Y() += ((index % 4) < 2) ? ySize : 0;
        nodeMinPoint.Z() += ((index % 8) < 4) ? zSize : 0;

        m_Children[index] = new TOctree(this, m_ELEMENTS_PER_NODE, nodeMinPoint, xSize, ySize, zSize);
    }

    // add current elements to children
    while(m_Elements.empty() != false)
    {
        TElement<T> e = *m_Elements.begin();
        
        int index = FindChildIndex(e.Location());
        m_Children[index]->AddElement(e);

        m_Elements.pop_front();
    }

    m_IsLeafNode = false;
}

template <class T> bool TOctree<T>::IsInNode(Point3f p)
{
    Point3f maxPoint = m_MinPoint + m_Size;
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

template <class T> bool TOctree<T>::SearchNode(Point3f p, float distance)
{ 
    if(IsInNode(p))
    {
        // simple test if point is in the box
        return true;
    }

    // point is not in the box, test sphere w/ r=distance against this box
    // if sphere and box overlap, search this node!

    Point3f maxPoint = m_MinPoint + m_Size;

    float d = 0;
    for(int i = 0; i < 3; ++i)
    {
        if(p[i] < m_MinPoint[i])
        {
            d += (p[i] - m_MinPoint[i]) * (p[i] - m_MinPoint[i]);
        }
        else if(p[i] > maxPoint[i])
        {
            d += (p[i] - maxPoint[i]) * (p[i] - maxPoint[i]);
        }
    }

    if(d > (distance * distance))
    {
        return false;
    }
    else
    {
        return true;
    }
}
