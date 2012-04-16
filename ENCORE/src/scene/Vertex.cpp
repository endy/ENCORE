#include "Vertex.h"

/*************
* Vertex
*************/
Vertex::Vertex( float l_xCoord, float l_yCoord, float l_zCoord, float l_xNormal, float l_yNormal, float l_zNormal )
{
    set( l_xCoord, l_yCoord, l_zCoord, l_xNormal, l_yNormal, l_zNormal );
}

//Vertex::Vertex( unsigned int l_nIndex, const float* l_pVertexValuesArray, const float* l_pNormalValuesArray)
//{
//    set( l_nIndex, l_pVertexValuesArray, l_pNormalValuesArray );
//}

void Vertex::set( float l_xCoord, float l_yCoord, float l_zCoord, float l_xNormal = 0, float l_yNormal = 0, float l_zNormal = 0)
{
    m_Coordinates = Point3( l_xCoord, l_yCoord, l_zCoord );
    m_Normal = Vector3( l_xNormal, l_yNormal, l_zNormal );
}

//void Vertex::set(unsigned int l_nIndex, const float* l_pVertexValuesArray, const float* l_pNormalValuesArray)
//{
//    m_Coordinates.set( l_pVertexValuesArray[ l_nIndex ], l_pVertexValuesArray[ l_nIndex+1 ], l_pVertexValuesArray[ l_nIndex+2 ] );
//    m_Normal.set( l_pNormalValuesArray[ l_nIndex ], l_pNormalValuesArray[ l_nIndex+1 ], l_pNormalValuesArray[ l_nIndex+2 ] );
//}
