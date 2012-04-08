#include "Emissive.h"
#include "TrianglePrim.h"

using encore::IEmissive;

class TriangleLight : 
    public IEmissive
{
    // return a ray that corresponds to the direction of an emitted photon
    virtual Ray EmitPhoton();;
    // get the point, or a point of a photon emission
    virtual Point3f GetPointOfEmission();
};