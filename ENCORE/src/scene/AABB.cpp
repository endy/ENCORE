#include "AABB.h"

#include "Common.h"

#ifndef INFINITY
#define INFINITY     FLT_MAX
#endif

bool AABB::Intersect( AABB b2 )
{
    Point3 v1 = b2.getPos();
    Point3 v2 = b2.getPos() + b2.getSize();
    Point3 v3 = m_Position;
    Point3 v4 = m_Position + m_Size;
    return ((v4.X() > v1.X()) && (v3.X() < v2.X()) && // x-axis overlap
        (v4.Y() > v1.Y()) && (v3.Y() < v2.Y()) && // y-axis overlap
        (v4.Z() > v1.Z()) && (v3.Z() < v2.Z()));   // z-axis overlap
}

bool AABB::Contains( Point3 aPoint )
{
    Point3 v1 = m_Position, v2 = m_Position + m_Size;
    return ((aPoint.X() > (v1.X() - EPSILON)) && (aPoint.X() < (v2.X() + EPSILON)) &&
        (aPoint.Y() > (v1.Y() - EPSILON)) && (aPoint.Y() < (v2.Y() + EPSILON)) &&
        (aPoint.Z() > (v1.Z() - EPSILON)) && (aPoint.Z() < (v2.Z() + EPSILON)));
}

inline void AABB::Expand( float delta )
{
    m_Position = m_Position - Point3( delta, delta, delta );
    m_Size = m_Size + Point3( 2*delta, 2*delta, 2*delta );
}


AABB AABB::operator+(const Point3 & p)
{
    Point3 maxp = m_Position + m_Size;
    AABB temp( Point3( mmin(m_Position.X(), p.X()),
                        mmin(m_Position.Y(), p.Y()),
                        mmin(m_Position.Z(), p.Z()) ),
               Point3( mmax(maxp.X(), p.X()),
                        mmax(maxp.Y(), p.Y()),
                        mmax(maxp.Z(), p.Z()) ) );
    return temp;
}

AABB & AABB::operator+=(const Point3 & p)
{
    Point3 maxp = m_Position + m_Size;

    m_Position.X() = mmin(m_Position.X(), p.X());
    m_Position.Y() = mmin(m_Position.Y(), p.Y());
    m_Position.Z() = mmin(m_Position.Z(), p.Z());
    maxp.X() = mmax(maxp.X(), p.X());
    maxp.Y() = mmax(maxp.Y(), p.Y());
    maxp.Z() = mmax(maxp.Z(), p.Z());

    m_Size = maxp - m_Position;

    return *this;
}

AABB AABB::operator+( const AABB & b)
{
    Point3 maxp = m_Position + m_Size;
    Point3 bmaxp = b.getPos() + b.getSize();

    AABB temp( Point3( mmin(m_Position.X(), b.getPos().X()),
                        mmin(m_Position.Y(), b.getPos().Y()),
                        mmin(m_Position.Z(), b.getPos().Z()) ),
               Point3( mmax(maxp.X(), bmaxp.X()),
                        mmax(maxp.Y(), bmaxp.Y()),
                        mmax(maxp.Z(), bmaxp.Z()) ) );
    return temp;
}

AABB & AABB::operator+=(const AABB & b)
{
    Point3 maxp = m_Position + m_Size;
    Point3 bmaxp = b.getPos() + b.getSize();

    m_Position.X() = mmin(m_Position.X(), b.getPos().X());
    m_Position.Y() = mmin(m_Position.Y(), b.getPos().Y());
    m_Position.Z() = mmin(m_Position.Z(), b.getPos().Z());

    maxp.X() = mmax(maxp.X(), bmaxp.X());
    maxp.Y() = mmax(maxp.Y(), bmaxp.Y());
    maxp.Z() = mmax(maxp.Z(), bmaxp.Z());

    m_Size = maxp - m_Position;

    return *this;
}

AABB AABB::Intersection( AABB & b )
{
    if ( b.Intersect( *this ) )
    {
        Point3 maxp = m_Position + m_Size;
        Point3 bmaxp = b.getPos() + b.getSize();

        AABB temp( Point3( mmax(m_Position.X(), b.getPos().X()),
                            mmax(m_Position.Y(), b.getPos().Y()),
                            mmax(m_Position.Z(), b.getPos().Z()) ),
                   Point3( mmin(maxp.X(), bmaxp.X()),
                            mmin(maxp.Y(), bmaxp.Y()),
                            mmin(maxp.Z(), bmaxp.Z()) ) );
        return temp;
    }
    else
    {
        // return default AABB
        return AABB();
    }
}

bool AABB::Intersect( Ray& r, float& hit0, float& hit1)
{
//    Point3 inv_rayD(1.f/r.Direction().X(), 1.f/r.Direction().Y(), 1.f/r.Direction().Z());
    float t0 = 0, t1 = INFINITY;

    Point3 tIn = (m_Position - r.Origin());//*inv_rayD;
    tIn.X() /= r.Direction().X();
    tIn.Y() /= r.Direction().Y();
    tIn.Z() /= r.Direction().Z();

    Point3 tOut = (m_Position + m_Size - r.Origin());//*inv_rayD;
    tOut.X() /= r.Direction().X();
    tOut.Y() /= r.Direction().Y();
    tOut.Z() /= r.Direction().Z();

    for(int i = 0; i < 3; i++)
    {
        if(tIn[i] > tOut[i]) swap(tIn[i], tOut[i]);
        t0 = tIn[i] > t0 ? tIn[i] : t0;
        t1 = tOut[i] < t1 ? tOut[i] : t1;
        if(t0 > t1) return false;
    }
    hit0 = t0;
    hit1 = t1;

    return true;
}
