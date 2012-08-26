#pragma once

#include "Vertex.h"
#include "Material.h"

using encore::Material;

/***********
* Triangle
***********/
class Triangle
{
public:
    Triangle( void );
    Triangle( Vertex& l_v0, Vertex& l_v1, Vertex& l_v2 );
    ~Triangle( void ) {}

    void print( void );

    void setVertex0( Vertex& l_vertex ) { m_pV0 = &l_vertex; }
    Vertex* getVertex0( void ) const { return m_pV0; }

    void setVertex1( Vertex& l_vertex ) { m_pV1 = &l_vertex; }
    Vertex* getVertex1( void ) const { return m_pV1; }

    void setVertex2( Vertex& l_vertex ) { m_pV2 = &l_vertex; }
    Vertex* getVertex2( void ) const { return m_pV2; }

    void setMaterial( Material* l_material ) { m_pMaterial = l_material; }
    Material* getMaterial( void ) const { return m_pMaterial; }

private:
    Vertex* m_pV0;
    Vertex* m_pV1;
    Vertex* m_pV2;

    Material* m_pMaterial;
};
