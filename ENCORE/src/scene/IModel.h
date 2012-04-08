#pragma once

#include <list>
#include "TPoint3.h"
#include "Material.h"

class IPrimitive;
class Triangle;

using encore::Point3f;
using encore::Material;

using namespace std;

class IModel
{
public:
    IModel() { m_Material = NULL; }
    virtual ~IModel(){ if(m_Material){ delete m_Material; } }

    // TODO: refactor this.  temp hack to add sphere to prim list
    virtual void AddPrimitive(IPrimitive *prim);
    // TODO: temp functions -- brandon
    void DrawOpenGL();

    std::list< IPrimitive* >& getPrimitiveList( void ){ return m_PrimitiveList; }
    std::list< Triangle* >* getNewTesselation ( void );

    unsigned int GetPrimitiveCount( void ) const { return (unsigned int)m_PrimitiveList.size(); }

    void SetMaterial(Material mat);
    Material GetMaterial(){ return *m_Material; }

    Point3f& MaxPoint( void ) { return m_maxPoint; }
    Point3f& MinPoint( void ) { return m_minPoint; }

protected:
    Material *m_Material;

    Point3f m_maxPoint;
    Point3f m_minPoint;

    std::list< IPrimitive* > m_PrimitiveList;
};
