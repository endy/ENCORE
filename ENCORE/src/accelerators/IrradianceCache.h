#pragma once

#include "Color.h"
#include "TOctree.h"

using encore::Color;

class IrrCacheNode
{
public:

    double GetWeight(Point3 p, Vector3 n);

public:
    Color       m_Color;
    Point3     m_Position;
    Vector3    m_Normal;
    float       m_R;
    double      m_Weight;
};

class IrradianceCache
{
public:
    IrradianceCache(Point3 minPoint, float xSize, float ySize, float zSize);
    virtual ~IrradianceCache();
    
    void AddIrradianceNode(IrrCacheNode value, Point3 location);

    // returns true whether the value radiance can be used at location
    // false if a new value needs to be calculated
    bool GetIrradiance(Point3 location, Vector3 normal, Color &radiance);

    TOctree<IrrCacheNode>* GetCache(){ return m_Cache; }

    void SetTolerance(float tolerance){ m_Tolerance = tolerance; }

private:
    TOctree<IrrCacheNode> *m_Cache;

    float m_Tolerance;
};
