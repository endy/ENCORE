#include "IrradianceCache.h"

#include <iostream>


float IrrCacheNode::GetWeight(const Point3& p, const Vector3& n)
{
    // Equation 11.5 from Pg 141 in Jensen's book
    // weight = 1.0 /( (||xi-x|| /Ri) + Sqrt(1.0-dot(Ni, N)));

    float distance = Vector3(p, m_Position).Magnitude();

    float distCalc = (m_R == 0) ? 0 : distance / m_R;

    float sqrtCalc = (n == m_Normal) ? 0 : sqrt(1.0 - Dot(n, m_Normal));
    
    float denom = sqrtCalc;// + ;

    if(denom == 0)
    {
        m_Weight = 0;
    }
    else
    {
        m_Weight = 1/denom;
    }
    return m_Weight;
}

IrradianceCache::IrradianceCache(Point3 minPoint, float xSize, float ySize, float zSize)
{
    m_Tolerance = 0.1;

    int elementsPerNode = 2;
    m_Cache = new TOctree<IrrCacheNode>(elementsPerNode, minPoint, xSize, ySize, zSize);
}

IrradianceCache::~IrradianceCache()
{
    if(m_Cache)
    {
        delete m_Cache;
        m_Cache = NULL;
    }
}

void IrradianceCache::AddIrradianceNode(IrrCacheNode value, Point3 location)
{
    if(value.m_Normal.MagnitudeSquared() == 0)
    {
       m_Cache->AddElement(value, location); 
    }
    m_Cache->AddElement(value, location);
}


bool IrradianceCache::GetIrradiance(Point3 position, Vector3 normal, Color &radiance)
{
    radiance = Color();

    float maxDistance = 25;
    // do irradiance calculation here...
    std::list< TElement<IrrCacheNode> > precalced;

    int maxDepth = 0;
    m_Cache->GetAllElementsNearPoint(position, maxDistance, precalced, maxDepth);
    
    float weightSum = 0;

    std::list< TElement<IrrCacheNode> >::iterator nodeIter = precalced.begin();

    std::cout << 1/m_Tolerance << std::endl;
    while(nodeIter != precalced.end())
    {
        float w = nodeIter->Data().GetWeight(position, normal);
        if(w > 1/m_Tolerance)
        {
            weightSum += w;
        }
        nodeIter++;
    }

    nodeIter = precalced.begin();

    while(nodeIter != precalced.end())
    {
        float w = nodeIter->Data().GetWeight(position, normal);
        if(w > 1/m_Tolerance)
        {
            radiance += nodeIter->Data().m_Color * (w / weightSum);
        }
        nodeIter++;
    }

    if(weightSum == 0)
    {
        return false;
    }


    //radiance /= weightSum;

    return true;
}
