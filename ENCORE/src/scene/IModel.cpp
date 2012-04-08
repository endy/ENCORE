#include "IModel.h"
#include "Primitive.h"
#include "Triangle.h"
#include "Vertex.h"

#ifdef WIN32
#include <Windows.h>
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif

std::list< Triangle* >* IModel::getNewTesselation ( void )
{
    std::list<Triangle*>::iterator itri;
    std::list<Triangle*>* ptris;
    std::list<Triangle*>* pTriangles = new std::list<Triangle*>();

    std::list< IPrimitive* >::iterator ip;
    for ( ip = m_PrimitiveList.begin(); ip != m_PrimitiveList.end(); ip++ )
    {
        ptris = (*ip)->getNewTesselation();
        for ( itri = ptris->begin(); itri != ptris->end(); itri++ )
        {
            pTriangles->push_back( *itri );
        }
        ptris->clear();
        delete ptris;
        ptris = NULL;
    }
    return pTriangles;
}

void IModel::AddPrimitive(IPrimitive *prim)
{
    prim->setMaterial(m_Material);
    m_PrimitiveList.push_back(prim);
}

void IModel::DrawOpenGL()
{
    glColor3f(m_Material->GetDiffuse().R(), m_Material->GetDiffuse().G(), m_Material->GetDiffuse().B());

    std::list<Triangle*> *tris = getNewTesselation();
    std::list<Triangle*>::iterator triIter = tris->begin();

    while(triIter != tris->end())
    {
        Triangle *tri = *triIter;
        Point3f v0 = tri->getVertex0()->getCoordinates();
        Point3f v1 = tri->getVertex1()->getCoordinates();
        Point3f v2 = tri->getVertex2()->getCoordinates();

        glBegin(GL_TRIANGLES);
        glVertex3f(v0.X(), v0.Y(), v0.Z());
        glVertex3f(v1.X(), v1.Y(), v1.Z());
        glVertex3f(v2.X(), v2.Y(), v2.Z());
        glEnd();

        ++triIter;
    }
}

void IModel::SetMaterial(Material mat)
{
    if(!m_Material)
    {
        m_Material = new Material();
    }

    *m_Material = mat;
}