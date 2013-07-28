#include "SpherePrim.h"
#include "Triangle.h"

#include <iostream>

SpherePrim::SpherePrim()
{
    // unit sphere
    m_Center = Point3(0.0f, 0.0f, 0.0f);
    m_Radius = 1.0f;
}

SpherePrim::SpherePrim(Point3 center, float radius)
{
    m_Center = center;
    m_Radius = radius;
}

SpherePrim::~SpherePrim()
{

}

bool SpherePrim::intersect(Ray& r, HitInfo* pHitInfo)
{
    /////////   NOTE   //////////////////////////////////////////
    //       
    //  This algorithm is taken from Real-Time Rendering, 2nd Ed.
    //  pg. 571.   Refer to that for details
    //
    // l is a vector from the ray's origin to the sphere's center
    Vector3 rayToCenter(r.Origin(), m_Center);

    float s             = Dot(rayToCenter, r.Direction());
    float rTCSquared    = Dot(rayToCenter, rayToCenter);
    float radiusSquared = m_Radius * m_Radius;

    if(s < 0 && rTCSquared > radiusSquared)
    {
        // sphere is behind the ray's origin
        return false;
    }

    float mSquared = rTCSquared - (s * s);

    if(mSquared > radiusSquared)
    {
        // ray is to the side of the sphere, but won't intersect
        return false;
    }

    //// RAY INTERSECTS THE SPHERE! ////

    // calc distance q 
    float q = sqrt(radiusSquared - mSquared);

    if(rTCSquared > radiusSquared)
    {
        // ray's origin is outside sphere, calc first intersection
        pHitInfo->hitTime = s - q;	
        pHitInfo->isEntering = true;
    }
    else
    {
        // ray's origin is inside sphere, first AND ONLY intersection is at 
        //   t = s + q
        pHitInfo->hitTime = s + q;
        pHitInfo->isEntering = false;
    }

    // calc hit location using hit time and ray
    pHitInfo->hitPoint  = Point3( r.GetPositionAtTime((float) pHitInfo->hitTime));

    pHitInfo->hitNormal = Vector3(m_Center, pHitInfo->hitPoint);
    pHitInfo->hitNormal.Normalize();

    pHitInfo->hitObject = this;
    pHitInfo->bHasInfo  = true;

    return true;
}

bool SpherePrim::intersectAABB(AABB box)
{
    /////////   NOTE   //////////////////////////////////////////
    //       
    //  This algorithm is taken from Real-Time Rendering, 2nd Ed.
    //  pg. 599.   Refer to that for details
    //

    float distance = 0;

    Point3 minPoint = box.getPos();

    distance += (m_Center.X() < minPoint.X()) ?  (m_Center.X() - minPoint.X()) : (minPoint.X() - m_Center.X());
    distance += (m_Center.Y() < minPoint.Y()) ?  (m_Center.Y() - minPoint.Y()) : (minPoint.Y() - m_Center.Y());
    distance += (m_Center.Z() < minPoint.Z()) ?  (m_Center.Z() - minPoint.Z()) : (minPoint.Z() - m_Center.Z());
   
    float rSquared = m_Radius * m_Radius;
    
    if(distance > rSquared)
    {
        // box & sphere are disjoint
        return false;
    }
    else
    {
        // box & sphere overlap
        return true;
    }
}

AABB SpherePrim::getAABB() const
{
    Point3 minPoint;
    minPoint.X() = m_Center.X() - m_Radius;
    minPoint.Y() = m_Center.Y() - m_Radius;
    minPoint.Z() = m_Center.Z() - m_Radius;

    Point3 maxPoint;
    maxPoint.X() = m_Center.X() + m_Radius;
    maxPoint.Y() = m_Center.Y() + m_Radius;
    maxPoint.Z() = m_Center.Z() + m_Radius;

    return AABB(minPoint, maxPoint);
}

std::list<Triangle*>* SpherePrim::getNewTesselation()
{
    // TODO: create tesselation for sphere
    
    std::list<Triangle*> *triList = new std::list<Triangle*>();

    Triangle *newTri = new Triangle();
    
    //newTri->setVertex0(Vertex(m_Center.X(), m_Center.Y(), m_Center.Z(), 0, 0, 1));
    //newTri->setVertex1(Vertex(m_Center.X()+2, m_Center.Y()+2, m_Center.Z(), 0, 0, 1));
    //newTri->setVertex2(Vertex(m_Center.X(), m_Center.Y(), m_Center.Z(), 0, 0, 1));

    newTri->setMaterial(m_Material);
    
    triList->push_back(newTri);

    return triList;
}

