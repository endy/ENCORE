#pragma once

#include "Primitive.h"
#include "Emissive.h"

using encore::Color;
using encore::IEmissive;

class PointLightPrim : 
    public encore::IEmissive, 
    public IPrimitive
{
public:
    PointLightPrim( void );
    PointLightPrim( const Point3& l_Position, const Color& l_Color );
    PointLightPrim( float x, float y, float z, float r, float g, float b );
    ~PointLightPrim( void );

    // IEmissive Interface
    virtual Ray EmitPhoton();
    virtual Point3 GetPointOfEmission( void ) { return m_Position; }

    // IPrimitive Interface
    HitInfo intersect(Ray l_Ray);
    bool intersectAABB(AABB l_Box) { return l_Box.Contains( m_Position ); }
    AABB getAABB( void ) const { return AABB( m_Position, m_Position ); }
    std::list<Triangle*>* getNewTesselation ( void ) { return new std::list<Triangle*>(); } // returns no triangles

    // PointLightPrim Public Methods
    Point3 GetPosition(){ return m_Position; }
    void SetPosition(Point3 position){ m_Position = position; }

private:
    Point3 m_Position;
};
