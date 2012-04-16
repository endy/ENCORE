#include "Texture.h"

#include <cstring>

CTexel::CTexel(float r, float g, float b, float a)
{
    m_Data[0] = r;
    m_Data[1] = g;
    m_Data[2] = b;
    m_Data[3] = a;
}

CTexel::CTexel(float pixel[], int pixelWidth)
{
    memcpy(m_Data, pixel, pixelWidth*sizeof(float));
}

CTexel::~CTexel()
{
}

CTexture::CTexture(int width, int height, int pixelWidth):
    m_Width(width),
    m_Height(height),
    m_PixelWidth(pixelWidth)
{
    m_Data = new float[m_Width * m_Height * m_PixelWidth];
}

CTexture::~CTexture()
{
}

CTexel CTexture::GetPixel(int x, int y)
{
    int index = (m_Width * m_PixelWidth * y) + (m_PixelWidth * x);
    
    return CTexel((m_Data+index), m_PixelWidth);
}

void CTexture::SetPixel(int x, int y, const CTexel& t)
{
   int index = (m_Width * m_PixelWidth * y) + (m_PixelWidth * x);

   memcpy(m_Data+index, t.m_Data, m_PixelWidth*sizeof(float));
}
