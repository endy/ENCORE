#pragma once

#include "ImageMetric.h"


class CMetricRMS :
    public CImageMetric
{
public:
    CMetricRMS();
    virtual ~CMetricRMS();

public:
    // compares a test image to an ideal image, returns the 
    CTexture* Analyze(CTexture *idealImage, CTexture *testImage);

    double GetError(){ return m_Error; }
    double GetErrorR(){ return m_ErrorR; }
    double GetErrorG(){ return m_ErrorG; }
    double GetErrorB(){ return m_ErrorB; }

private:
    double m_Error;

    double m_ErrorR;
    double m_ErrorG;
    double m_ErrorB;

};