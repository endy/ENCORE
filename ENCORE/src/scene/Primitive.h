#pragma once


#include <list>

#include "AABB.h"
#include "Ray.h"
#include "HitInfo.h"

#include "Material.h"

// forward declarations
class Triangle;

using encore::Material;
using encore::Ray;

/***************
* IPrimitive Class
***************/
class IPrimitive
{
public:

    IPrimitive( void ) { m_Material = NULL; }
    virtual ~IPrimitive( void ) {}

    // intersect:
    // returns the HitInfo of the Ray intersection with the IPrimitive
    virtual HitInfo intersect(Ray& l_Ray ) = 0;

    // intersectAABB:
    // returns boolean whether or not the IPrimitive intersects with an AABB
    virtual bool intersectAABB(AABB l_pBox) = 0;

    // getAABB:
    // returns an AABB that fits the IPrimitive
    virtual AABB getAABB( void ) const = 0;

    virtual std::list< Triangle* >* getNewTesselation ( void ) = 0;

    // these are inherited by subclasses and shouldn't be overloaded.
    void setMaterial( Material* l_Material ) { m_Material = l_Material; }

    Material* getMaterial( void ) { return m_Material; }

protected:
    Material *m_Material;
};
