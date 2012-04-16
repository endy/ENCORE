#include "PointLightPrim.h"

#include "Common.h"

PointLightPrim::PointLightPrim(void)
{
    m_Position = Point3( 0, 0, 0 );
}

PointLightPrim::PointLightPrim( const Point3& l_Position, const Color& l_Color )
{
    m_Position = l_Position;
    SetEmissiveColor( l_Color );
}

PointLightPrim::PointLightPrim( float x, float y, float z, float r, float g, float b)
{
    m_Position = Point3( x, y, z );
    m_EmissiveColor = Color( r, g, b, 0 );
}

PointLightPrim::~PointLightPrim(void)
{

}

Ray PointLightPrim::EmitPhoton()
{
    // point light source: photon origin @ point, ray is a random direction
    return Ray(m_Position, randDirection());
}

HitInfo PointLightPrim::intersect(Ray l_Ray )
{
    // can't actually intersect with a PointLightPrim
    HitInfo hit;
    hit.bHasInfo = false;
    return hit;
}
