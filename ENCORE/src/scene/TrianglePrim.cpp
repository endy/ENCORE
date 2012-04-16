#include "TrianglePrim.h"

/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/* Function: int triBoxOverlap(float boxcenter[3],      */
/*          float boxhalfsize[3],float triverts[3][3]); */
/* History:                                             */
/*   2001-03-05: released the code in its first version */
/*   2001-06-18: changed the order of the tests, faster */
/*                                                      */
/* Acknowledgement: Many thanks to Pierre Terdiman for  */
/* suggestions and discussions on how to optimize code. */
/* Thanks to David Hunt for finding a ">="-bug!         */
/********************************************************/
/* Some changes made to the code to fit with data       */
/* structures chosen for the AABB and Uniform Grid      */
/* Function triBoxOverlap() renamed AABB::IntersectTri()*/
/* Changes were made by: Peter Lohrmann                 */
/********************************************************/
/* some minor change on calculation                     */
/* add new build method for Uniform Grid                */
/* Chang made by : Chen hao chang                       */
/*                                                      */

//#define SMOOTH_EDGES

bool planeBoxOverlap(Vector3 normal, float d, Vector3 maxbox)
{
    Vector3 vmin,vmax;
    // x axis
    if(normal.X()>0.0f){
        vmin.X()=-maxbox.X();
        vmax.X()=maxbox.X();
    }else{
        vmin.X()=maxbox.X();
        vmax.X()=-maxbox.X();
    }
    // y axis
    if(normal.Y()>0.0f){
        vmin.Y()=-maxbox.Y();
        vmax.Y()=maxbox.Y();
    }else{
        vmin.Y()=maxbox.Y();
        vmax.Y()=-maxbox.Y();
    }
    // z axis
    if(normal.Z()>0.0f){
        vmin.Z()=-maxbox.Z();
        vmax.Z()=maxbox.Z();
    }else{
        vmin.Z()=maxbox.Z();
        vmax.Z()=-maxbox.Z();
    }

    if(d+(Dot(normal, vmin))>0.0f) return false;
    if(d+(Dot(normal, vmax))>=0.0f) return true;

    return false;
}

/***************
* TrianglePrim Class
***************/
TrianglePrim::TrianglePrim( Triangle& l_Triangle )
{
    m_pTriangle = &l_Triangle;
}

TrianglePrim::TrianglePrim( Vertex& l_Vertex0, Vertex& l_Vertex1, Vertex& l_Vertex2 )
{
    m_pTriangle = new Triangle();
    m_pTriangle->setVertex0( l_Vertex0 );
    m_pTriangle->setVertex1( l_Vertex1 );
    m_pTriangle->setVertex2( l_Vertex2 );
}

TrianglePrim::~TrianglePrim( void )
{
}

std::list<Triangle*>* TrianglePrim::getNewTesselation ( void )
{
    std::list<Triangle*>* triList = new std::list<Triangle*>();
    Triangle* t = new Triangle();
    *t = *m_pTriangle;

    t->setMaterial(m_Material);
    
    triList->push_back( t );
    return triList;
}

/**************
* intersectAABB
**************/
bool TrianglePrim::intersectAABB(AABB l_Box )
{
    /*    use separating axis theorem to test overlap between triangle and box */
    /*    need to test for overlap in these directions: */
    /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
    /*       we do not even need to test these) */
    /*    2) normal of the triangle */
    /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
    /*       this gives 3x3=9 more tests */

    // POS is not center, it is the min point in this AABB
    Vector3 boxhalfsize = Vector3(Point3(), l_Box.getSize()) * 0.5f;
    Point3 center = l_Box.getPos() + boxhalfsize.ToPoint();

    Point3 vp0 = m_pTriangle->getVertex0()->getCoordinates();
    Point3 vp1 = m_pTriangle->getVertex1()->getCoordinates();
    Point3 vp2 = m_pTriangle->getVertex2()->getCoordinates();

    Vector3 normal;
    Vector3 e0, e1, e2;
    float minCoord, maxCoord, p0, p1, p2, rad, fex, fey, fez;
    Vector3 v0(center, vp0);
    Vector3 v1(center, vp1);
    Vector3 v2(center, vp2);

    e0 = (v1 - v0); e1 = (v2 - v1); e2 = (v0 - v2);

    /* Bullet 3:  */
    /*  test the 9 tests first (this was faster) */
    fex = fabsf( e0.X() );
    fey = fabsf( e0.Y() );
    fez = fabsf( e0.Z() );
    AXISTEST_X01( e0.Z(), e0.Y(), fez, fey );
    AXISTEST_Y02( e0.Z(), e0.X(), fez, fex );
    AXISTEST_Z12( e0.Y(), e0.X(), fey, fex );

    fex = fabsf( e1.X() );
    fey = fabsf( e1.Y() );
    fez = fabsf( e1.Z() );
    AXISTEST_X01( e1.Z(), e1.Y(), fez, fey );
    AXISTEST_Y02( e1.Z(), e1.X(), fez, fex );
    AXISTEST_Z0 ( e1.Y(), e1.X(), fey, fex );

    fex = fabsf( e2.X() );
    fey = fabsf( e2.Y() );
    fez = fabsf( e2.Z() );
    AXISTEST_X2 ( e2.Z(), e2.Y(), fez, fey );
    AXISTEST_Y1 ( e2.Z(), e2.X(), fez, fex );
    AXISTEST_Z12( e2.Y(), e2.X(), fey, fex );

    /* Bullet 1: */
    /*  first test overlap in the {x,y,z}-directions */
    /*  find min, max of the triangle each direction, and test for overlap in */
    /*  that direction -- this is equivalent to testing a minimal AABB around */
    /*  the triangle against the AABB */

    FINDMINMAX( v0.X(), v1.X(), v2.X(), minCoord, maxCoord );
    if (minCoord > boxhalfsize.X() || maxCoord < -boxhalfsize.X()){
        return false;
    }
    FINDMINMAX( v0.Y(), v1.Y(), v2.Y(), minCoord, maxCoord );
    if (minCoord > boxhalfsize.Y() || maxCoord < -boxhalfsize.Y()){
        return false;
    }
    FINDMINMAX( v0.Z(), v1.Z(), v2.Z(), minCoord, maxCoord );
    if (minCoord > boxhalfsize.Z() || maxCoord < -boxhalfsize.Z()){
        return false;
    }

    normal = Cross(e0,e1);

    /* Bullet 2: */
    /*  test if the box intersects the plane of the triangle */
    /*  compute plane equation of triangle: normal*x+d=0 */
    float d = -Dot(normal, v0);  /* plane eq: normal.X()+d=0 */
    if ( !planeBoxOverlap( normal, d, boxhalfsize ) )
    {
        return false;
    }

    return true;
}

/******************
* returns an AABB that is just large enough
* to hold the TrianglePrim
******************/
AABB TrianglePrim::getAABB( void ) const
{
    Point3 mins;
    Point3 maxs;
    Point3 v0 = m_pTriangle->getVertex0()->getCoordinates();
    Point3 v1 = m_pTriangle->getVertex1()->getCoordinates();
    Point3 v2 = m_pTriangle->getVertex2()->getCoordinates();

    mins.X() = dmin(dmin(v0.X(), v1.X()), v2.X());
    mins.Y() = dmin(dmin(v0.Y(), v1.Y()), v2.Y());
    mins.Z() = dmin(dmin(v0.Z(), v1.Z()), v2.Z());

    maxs.X() = dmax(dmax(v0.X(), v1.X()), v2.X());
    maxs.Y() = dmax(dmax(v0.Y(), v1.Y()), v2.Y());
    maxs.Z() = dmax(dmax(v0.Z(), v1.Z()), v2.Z());

    AABB boundingBox( mins, maxs );
    return boundingBox;
}

HitInfo TrianglePrim::intersect(Ray l_Ray)
{
    Point3 v0( m_pTriangle->getVertex0()->getCoordinates() );
    Point3 v1( m_pTriangle->getVertex1()->getCoordinates() );
    Point3 v2( m_pTriangle->getVertex2()->getCoordinates() );

    Vector3 edge1(v0, v1);
    Vector3 edge2(v0, v2);

    Vector3 pvec = Cross(l_Ray.Direction(), edge2);

    float det = Dot(edge1, pvec);
    float invdet = 1.0f/det;

    Vector3 svec(v0, l_Ray.Origin());
    float alpha = Dot(svec, pvec) * invdet;

    Vector3 qvec = Cross(svec, edge1);
    float beta = Dot(l_Ray.Direction(), qvec) * invdet;

    float time = Dot(edge2, qvec) * invdet;

    HitInfo info;
    if ( (alpha >= 0.0) && (beta >= 0.0) && (alpha + beta <= 1)
          && ( time > 0.0 ) )
    {
        info.hitTime = time;
#ifdef SMOOTH_EDGES
        // calculate interpolated normal
        Vector3 n0( m_pTriangle->getVertex0()->getNormal() );
        Vector3 n1( m_pTriangle->getVertex1()->getNormal() );
        Vector3 n2( m_pTriangle->getVertex2()->getNormal() );

        Vector3 tempNormal = n0 + (n1-n0)*alpha + (n2-n0)*beta;
        info.hitNormal = tempNormal;   
#else
       info.hitNormal = Cross(edge1, edge2);
#endif
        info.hitNormal.Normalize();
        info.hitPoint = v0 + (v1-v0)*alpha + (v2-v0)*beta;
        info.isEntering = (Dot(l_Ray.Direction(),(info.hitNormal)) < 0);
        info.hitObject = this;
        info.bHasInfo = true;
    }

    return info;

}
