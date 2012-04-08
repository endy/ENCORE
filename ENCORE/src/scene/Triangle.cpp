#include "Triangle.h"


/***********
* Triangle
***********/
Triangle::Triangle( void ): m_pV0(NULL), m_pV1(NULL), m_pV2(NULL)//, m_pMaterial(NULL)
{
}

Triangle::Triangle( Vertex& l_v0, Vertex& l_v1, Vertex& l_v2 )
{
    m_pV0 = &l_v0;
    m_pV1 = &l_v1;
    m_pV2 = &l_v2;
}

void Triangle::print( void )
{
    Point3f v0 = m_pV0->getCoordinates();
    Point3f v1 = m_pV1->getCoordinates();
    Point3f v2 = m_pV2->getCoordinates();

    printf("Vertex0: %f, %f, %f\n",   v0.X(), v0.Y(), v0.Z());
    printf("Vertex1: %f, %f, %f\n",   v1.X(), v1.Y(), v1.Z());
    printf("Vertex2: %f, %f, %f\n\n", v2.X(), v2.Y(), v2.Z());
}