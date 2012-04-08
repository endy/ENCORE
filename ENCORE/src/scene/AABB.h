#pragma once

#include "TPoint3.h"
#include "Ray.h"

using encore::Point3f;
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
    AABB( Point3f minCorner, Point3f maxCorner )
    {
        m_Position = minCorner; m_Size = maxCorner-minCorner; 
    }
    ~AABB( void ) {}
    Point3f getPos( void ) const { return m_Position; }
    Point3f getSize( void ) const { return m_Size; }
    bool IntersectBox( AABB aBox ) { return Intersect( aBox ); }
    bool Intersect( AABB b2 );
    bool Contains(Point3f aPoint );

    // returns AABB that contains AABB and point
    AABB operator+( const Point3f & p); 
    AABB & operator+=( const Point3f & p);

    // returns AABB that contains both AABBs
    AABB operator+( const AABB & b); 
    AABB & operator+=( const AABB & b);

    // returns AABB that is the intersected volume of two AABBs
    AABB Intersection( AABB & b);
    bool AABB::lessThan(float med, int axis) const { return m_Position[axis] < med; }

    // from bbox class
    void Expand( float delta );
    float Volume( void ) const { return m_Size.X() * m_Size.Y() * m_Size.Z(); }
    float Area( void ) const { return m_Size.X()*m_Size.Y() + m_Size.Y()*m_Size.Z() + m_Size.Z()*m_Size.X(); }
    //bool Overlaps(const Triangle &t) const { }
    bool Intersect( Ray &r, float &hit0, float &hit1);

private:
    Point3f m_Position;    // xyz coords of the lowest corner of the AABB
    Point3f m_Size;       // width,height,depth of the AABB
};
