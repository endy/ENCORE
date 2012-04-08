#pragma once

#include "TVector3.h"

using encore::Point3f;
using encore::Vector3f;

// forward declarations
class IPrimitive;

/************
* HitInfo
************/
class HitInfo
{
public:
    HitInfo(void);
    ~HitInfo(void) {}

    double hitTime;
    IPrimitive * hitObject;
    bool isEntering;
    int surface;
    Point3f hitPoint;
    Vector3f hitNormal;
    Point3f hitVoxel;
    bool bHasInfo;
};
