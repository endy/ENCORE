#pragma once

#include "Vector3.h"

using encore::Point3;
using encore::Vector3;

/*************
* Vertex
*************/
class Vertex
{
public:
    Vertex( void ) {}
    Vertex( float l_xCoord, float l_yCoord, float l_zCoord, float l_xNormal, float l_yNormal, float l_zNormal );
    ~Vertex( void ) {}

    void set( float l_xCoord, float l_yCoord, float l_zCoord, float l_xNormal, float l_yNormal, float l_zNormal );

    void setCoordinates( const Point3& l_Coordinates ) { m_Coordinates = l_Coordinates; }
    void setCoordinates(float x, float y, float z) { m_Coordinates = Point3(x,y,z); }
    const Point3& getCoordinates( void ) const { return m_Coordinates; }

    void setNormal( const Vector3& l_Normal ) { m_Normal = l_Normal; }
    //void setNormal(float x, float y, float z) { m_Normal = Vector3(x,y,z); }
    const Vector3& getNormal( void ) const { return m_Normal; }

    void setTexCoord( float l_xCoord, float l_yCoord, float l_zCoord = 0 ) { m_TexCoord = Point3( l_xCoord, l_yCoord, l_zCoord ); }
    const Point3& getTexCoord( void ) const { return m_TexCoord; };

    bool equals( Vertex& l_Vertex ) const { return this == &l_Vertex; }

private:
    Point3 m_Coordinates;
    Vector3 m_Normal;
    Point3 m_TexCoord;
};
