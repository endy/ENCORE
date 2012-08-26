#include "Tonemap.h"

#include <cmath>
#include "TextureMaker.h"

#include <iostream>

const float SCALE_683 = 683.0f; // this is taken from Zack's code sample, also appears in PBR

Tonemap::~Tonemap()
{
    if(m_Lum)
    {
        delete m_Lum;
    }
}

void Tonemap::ApplyToneMap(TextureMaker *pTexture)
{
    if(m_Lum){ delete [] m_Lum; }

    int cPixels = pTexture->GetImageWidth() * pTexture->GetImageHeight();

    m_Lum = new float[cPixels];

    m_MaxDisplay = 100.0f;
   
    // calculate luminance
    for(int row = 0; row < pTexture->GetImageHeight(); ++row)
    {
        for(int col = 0; col < pTexture->GetImageWidth(); ++col)
        {
            GLfloat* pColor = pTexture->GetPixel(row, col);
            int lumIndex = row * pTexture->GetImageWidth() + col;

            // calculate luminance
            m_Lum[lumIndex] = SCALE_683 * ( 0.212671f*pColor[0] + 0.715160f*pColor[1] + 0.072169f*pColor[2] );

            m_MaxLum = (m_Lum[lumIndex] > m_MaxLum) ? m_Lum[lumIndex] : m_MaxLum;
        }
    }

    Map(cPixels);

    float displayTo0_1 = SCALE_683 / m_MaxDisplay;

    for(int pixelIndex = 0; pixelIndex < cPixels; ++pixelIndex)
    {
        pTexture->GetDataValue(pixelIndex * 4) *= m_Lum[pixelIndex] * displayTo0_1;
        pTexture->GetDataValue(pixelIndex * 4 + 1) *= m_Lum[pixelIndex] * displayTo0_1;
        pTexture->GetDataValue(pixelIndex * 4 + 2) *= m_Lum[pixelIndex] * displayTo0_1;
    }

}

void TonemapMaxToWhite::Map(int cPixels)
{
    float s = m_MaxDisplay / m_MaxLum;

    for(int pixelIndex = 0; pixelIndex < cPixels; ++pixelIndex)
    {
        m_Lum[pixelIndex] *= s;
    }
}

void TonemapReinhard::Map(int cPixels)
{
    float invY2;
    if(m_MaxLum <= 0.0f)
    {
        float Ywa = 0.0f;

        for(int pixelIndex = 0; pixelIndex < cPixels; ++pixelIndex)
        {
            Ywa += (m_Lum[pixelIndex] > 0) ? logf(m_Lum[pixelIndex]) : 0;        
        }
        
        Ywa = expf(Ywa / cPixels);
        Ywa /= SCALE_683;
        invY2 = 1.0f / (Ywa * Ywa);
    }
    else
    {
        invY2 = 1.0f / (m_MaxLum * m_MaxLum);
    }

    for(int pixelIndex = 0; pixelIndex < cPixels; ++pixelIndex)
    {
        float ys = m_Lum[pixelIndex] / SCALE_683;

        m_Lum[pixelIndex] = m_MaxDisplay / SCALE_683 * (1.0f + ys * invY2) / (1.0f + ys);
    }
}

