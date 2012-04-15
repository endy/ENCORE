#pragma once

#include "Point3.h"
#include "Ray.h"

using encore::Point3;
using encore::Ray;

#ifndef mmax
#define mmax(a,b)     (((a) > (b)) ? (a) : (b))
#endif

#ifndef mmin
#define mmin(a,b)     (((a) < (b)) ? (a) : (b))
#endif

inline void swap(float &a, float &b)
{
    float temp;
    temp = a; a = b; b = temp;
}

/*******
* AABB *
********
* Based on aabb class by Jacco Bikkar
* rewritten to fit these data structures
* by: Peter Lohrmann
*******/
class AABB
{
public:
    AABB( void ) { }
    AABB( Point3 minCorner, Point3 maxCorner )
    {
        m_Position = minCorner; m_Size = maxCorner-minCorner; 
    }
    ~AABB( void ) {}
    Point3 getPos( void ) const { return m_Position; }
    Point3 getSize( void ) const { return m_Size; }
    bool IntersectBox( AABB aBox ) { return Intersect( aBox ); }
    bool Intersect( AABB b2 );
    bool Contains(Point3 aPoint );

    // returns AABB that contains AABB and point
    AABB operator+( const Point3 & p); 
    AABB & operator+=( const Point3 & p);

    // returns AABB that contains both AABBs
    AABB operator+( const AABB & b); 
    AABB & operator+=( const AABB & b);

    // returns AABB that is the intersected volume of two AABBs
    AABB Intersection( AABB & b);
    bool lessThan(float med, int axis) const { return (m_Position[axis] < med); }

    // from bbox class
    void Expand( float delta );
    float Volume( void ) const { return m_Size.X() * m_Size.Y() * m_Size.Z(); }
    float Area( void ) const { return m_Size.X()*m_Size.Y() + m_Size.Y()*m_Size.Z() + m_Size.Z()*m_Size.X(); }
    //bool Overlaps(const Triangle &t) const { }
    bool Intersect( Ray &r, float &hit0, float &hit1);

private:
    Point3 m_Position;    // xyz coords of the lowest corner of the AABB
    Point3 m_Size;       // width,height,depth of the AABB
};
