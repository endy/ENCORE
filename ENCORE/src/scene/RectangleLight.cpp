#include "RectangleLight.h"

#include "Triangle.h"
#include "TrianglePrim.h"

#include <iostream>

#include "Sampler.h"
#include "Common.h"

#define ENCR_MIN(a,b) (((a) < (b)) ? (a) : (b))
#define ENCR_MAX(a,b) (((a) > (b)) ? (a) : (b))

RectangleLight::RectangleLight()
{
    float yCoord = 49.99999f;
    m_Vertices.resize(4);

    m_Vertices[0].setCoordinates(Point3(15, yCoord, 15));
    m_Vertices[0].setNormal(Vector3(0, -1, 0));

    m_Vertices.push_back(Vertex());
    m_Vertices[1].setCoordinates(Point3(-15, yCoord, 15));
    m_Vertices[1].setNormal(Vector3(0, -1, 0));

    m_Vertices.push_back(Vertex());
    m_Vertices[2].setCoordinates(Point3(-15, yCoord, -15));
    m_Vertices[2].setNormal(Vector3(0, -1, 0));

    m_Vertices.push_back(Vertex());
    m_Vertices[3].setCoordinates(Point3(15, yCoord, -15));
    m_Vertices[3].setNormal(Vector3(0, -1, 0));
   
    m_Prim1 = new TrianglePrim(m_Vertices[0], m_Vertices[1], m_Vertices[2]);
    m_Prim2 = new TrianglePrim(m_Vertices[2], m_Vertices[3], m_Vertices[0]);

    Init();
}

RectangleLight::RectangleLight(std::vector<Vertex> vertices)
{
    assert(vertices.size() == 4);

    m_Vertices = vertices;

    assert(!"Constructor Not Supported");

    Init();
}

void RectangleLight::Init()
{
    m_Center.X() = (m_Vertices[0].getCoordinates().X() +
                    m_Vertices[1].getCoordinates().X() +
                    m_Vertices[2].getCoordinates().X() +
                    m_Vertices[3].getCoordinates().X()) / 4.0f;

    m_Center.Y() = (m_Vertices[0].getCoordinates().Y() +
                    m_Vertices[1].getCoordinates().Y() +
                    m_Vertices[2].getCoordinates().Y() +
                    m_Vertices[3].getCoordinates().Y()) / 4.0f;


    m_Center.Z() = (m_Vertices[0].getCoordinates().Z() +
                    m_Vertices[1].getCoordinates().Z() +
                    m_Vertices[2].getCoordinates().Z() +
                    m_Vertices[3].getCoordinates().Z()) / 4.0f;

    m_Normal = m_Vertices[0].getNormal();

    Point3 p1 = m_Vertices[0].getCoordinates();
    Point3 p2 = m_Vertices[1].getCoordinates();
    Point3 p3 = m_Vertices[3].getCoordinates();

    Vector3 p1p2(p1, p2);
    m_Length = p1p2.Magnitude();

    Vector3 p1p3(p1, p3);
    m_Width = p1p3.Magnitude();
}

RectangleLight::~RectangleLight()
{
    if(m_Prim1)
    {
        delete m_Prim1;
    }
    if(m_Prim2)
    {
        delete m_Prim2;
    }
}

HitInfo RectangleLight::intersect(Ray& r)
{
    HitInfo info = m_Prim1->intersect(r);

    if(info.bHasInfo)
    {
        info.hitObject = this;
        return info;
    }

    info = m_Prim2->intersect(r);
    
    if(info.bHasInfo)
    {
        info.hitObject = this;
        return info;
    }
    else
    {
        return info;
    }
}

bool RectangleLight::intersectAABB(AABB box)
{
    // check all points in the box

    Point3 minPoint = box.getPos();
    Point3 maxPoint = box.getPos() + box.getSize();

    for(int index = 0; index < (int)m_Vertices.size(); ++index)
    {
        Point3 p = m_Vertices[index].getCoordinates();

        if( minPoint.X() < p.X() && p.X() < maxPoint.X() &&
            minPoint.Y() < p.Y() && p.Y() < maxPoint.Y() &&
            minPoint.Z() < p.Z() && p.Z() < maxPoint.Z())
        {
            // at least one vert is in the box
            return true;
        }
    }

    return false;
}

AABB RectangleLight::getAABB() const
{
    Point3 minPoint = m_Vertices[0].getCoordinates();
    Point3 maxPoint = m_Vertices[0].getCoordinates();

    for(int index = 1; index < (int)m_Vertices.size(); ++index)
    {
        Point3 p = m_Vertices[index].getCoordinates();

        minPoint.X() = ENCR_MIN(p.X(), minPoint.X());
        minPoint.Y() = ENCR_MIN(p.Y(), minPoint.Y());
        minPoint.Z() = ENCR_MIN(p.Z(), minPoint.Z());

        maxPoint.X() = ENCR_MAX(p.X(), maxPoint.X());
        maxPoint.Y() = ENCR_MAX(p.Y(), maxPoint.Y());
        maxPoint.Z() = ENCR_MAX(p.Z(), maxPoint.Z());
    }

    return AABB(minPoint, maxPoint);
}

std::list<Triangle*>* RectangleLight::getNewTesselation()
{
    std::list<Triangle*> *tris = new std::list<Triangle*>();

    Triangle *t1 = new Triangle(m_Vertices[0], m_Vertices[1], m_Vertices[2]);
    t1->setMaterial(m_Material);
    Triangle *t2 = new Triangle(m_Vertices[2], m_Vertices[3], m_Vertices[0]);
    t2->setMaterial(m_Material);
    
    tris->push_back(t1);
    tris->push_back(t2);

    return tris;
}

Ray RectangleLight::EmitPhoton()
{
    Sampler s(1, 1, 1);
    s.SetJitterPercent(1.0);

    std::vector<Point3> originList = s.GetSamplePointsForRect(m_Center, m_Normal, m_Length, m_Width);
    Point3 origin = originList[0];

    Vector3 dir;

    dir = randDirectionN(m_Normal);

    return Ray(origin, dir);
}

Point3 RectangleLight::GetPointOfEmission()
{
    return EmitPhoton().Origin();
}

std::vector<Point3> RectangleLight::GetSamplePoints(int xDivisions, int yDivisions, int samplesPerCell)
{
    Sampler s(xDivisions, yDivisions, samplesPerCell);

    if(samplesPerCell != 1)
    {
        s.SetJitterPercent(0.5);
    }

    return s.GetSamplePointsForRect(m_Center, m_Normal, m_Length, m_Width);
}


