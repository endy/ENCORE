#pragma once

#include "Vector3.h"

using encore::Point3;
using encore::Vector3;

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
    Point3 hitPoint;
    Vector3 hitNormal;
    Point3 hitVoxel;
    bool bHasInfo;
};
