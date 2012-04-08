//////////////////////////////////////////////////////////////////////////
// ENCORE PROJECT
// 
// Developed By
//      Brandon Light:   brandon@alum.wpi.edu
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Texture.h"

class CImageMetric
{
public:
    CImageMetric(){};
    virtual ~CImageMetric(){};

public:
    // compares a test image to an ideal image, returns the 
    virtual CTexture* Analyze(CTexture *idealImage, CTexture *testImage) = 0;

};