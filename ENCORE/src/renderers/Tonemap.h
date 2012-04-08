#pragma once

class TextureMaker;

#include "Common.h"

class Tonemap
{
public:
    Tonemap(void): m_MaxLum(0.0f), m_Lum(NULL) {}
    Tonemap(float maxLum): m_MaxLum(maxLum), m_Lum(NULL){}
    virtual ~Tonemap();

    void ApplyToneMap(TextureMaker *pTexture);

private:
    virtual void Map(int cPixels) = 0;

protected:
    //
    float m_MaxLum;

    float m_MaxDisplay;
    
    // pointer to an array of luminance values per pixel
    float *m_Lum;
};

class TonemapMaxToWhite : public Tonemap
{
public:
    TonemapMaxToWhite(void){}
    TonemapMaxToWhite(float maxLum): Tonemap(maxLum){}
    virtual ~TonemapMaxToWhite(){}

private:
    virtual void Map(int cPixels);
};

class TonemapReinhard : public Tonemap
{
public:
    TonemapReinhard(void){}
    TonemapReinhard(float maxLum): Tonemap(maxLum){ }
    virtual ~TonemapReinhard(){}

private:
    virtual void Map(int cPixels);
     
};

