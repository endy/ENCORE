#include "MetricRMS.h"

#include <cmath>

CMetricRMS::CMetricRMS()
{

}

CMetricRMS::~CMetricRMS()
{

}

CTexture* CMetricRMS::Analyze(CTexture *idealImage, CTexture *testImage)
{
    if(idealImage->GetWidth() != testImage->GetWidth() ||
       idealImage->GetHeight() != testImage->GetHeight())
    {
        return NULL;
    }

    CTexture *diff = new CTexture(idealImage->GetWidth(), idealImage->GetHeight(), RGBA);

    double squaredSumR = 0.0;
    double squaredSumG = 0.0;
    double squaredSumB = 0.0;
    
    for(int y = 0; y < idealImage->GetHeight(); ++y)
    for(int x = 0; x < idealImage->GetWidth(); ++x)
    {
        CTexel iTexel = idealImage->GetPixel(x, y);
        CTexel tTexel = testImage->GetPixel(x, y);

        float r = iTexel.R()*255 - tTexel.R()*255;
        r = (r > 0) ? r : -r;
        float g = iTexel.G()*255 - tTexel.G()*255;
        g = (g > 0) ? g : -g;
        float b = iTexel.B()*255 - tTexel.B()*255;
        b = (b > 0) ? b : -b;
        float a = iTexel.A()*255 - tTexel.A()*255;
        a = (a > 0) ? a : -a;

        diff->SetPixel(x, y, CTexel(r, g, b, 1));

        // actual RMS calculation
        squaredSumR += r*r;
        squaredSumG += g*g;
        squaredSumB += b*b;
        
    }

    double pixels = idealImage->GetWidth() * idealImage->GetHeight();

    m_ErrorR = std::sqrt(squaredSumR/pixels);
    m_ErrorG = std::sqrt(squaredSumG/pixels);
    m_ErrorB = std::sqrt(squaredSumB/pixels);

    m_Error = (m_ErrorR + m_ErrorG + m_ErrorB) / 3.0;

    return diff;
}