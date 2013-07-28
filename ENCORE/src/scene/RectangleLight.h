#pragma once

#include "Primitive.h"
#include "Emissive.h"

#include "Vertex.h"

#include <vector>
#include <cassert>

class TrianglePrim;

using encore::IEmissive;

class RectangleLight :
    public IPrimitive,
    public IEmissive
{
public:
    RectangleLight();
    RectangleLight(std::vector<Vertex> vertices);
    virtual ~RectangleLight();

    //// IPrimitive Interface ////
    virtual bool intersect(Ray& l_Ray, HitInfo* pHitInfo);
    virtual bool intersectAABB(AABB box);
    virtual AABB getAABB() const;
    virtual std::list<Triangle*>* getNewTesselation(void);

    //// IEmissive Interface ////
    // return a ray that corresponds to the direction of an emitted photon
    virtual Ray EmitPhoton();
    // get the point, or a point of a photon emission
    virtual Point3 GetPointOfEmission();
    // get sample points for soft shadows
    virtual std::vector<Point3> GetSamplePoints(int xDivisions, int yDivisions, int samplesPerDivision);

private:
    void Init();

private:
    std::vector<Vertex> m_Vertices;
    TrianglePrim *m_Prim1;
    TrianglePrim *m_Prim2;

    Point3 m_Center;
    Vector3 m_Normal;

    float m_Length;
    float m_Width;
};
