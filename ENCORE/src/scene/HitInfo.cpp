#include "HitInfo.h"

#include "Common.h"

/************
* HitInfo
************/
HitInfo::HitInfo(void)
{
    bHasInfo = false;
    hitTime = POS_INF;
    hitObject = NULL;
    isEntering = false;
    surface = 0;
    hitVoxel.X() = -1;
    hitVoxel.Y() = -1;
    hitVoxel.Z() = -1;
}
