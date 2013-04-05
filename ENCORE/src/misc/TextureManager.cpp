
#include "TextureManager.h"


#include <stdio.h>
#include <fstream>

#include "stb_image_write.h"

#define ENCR_MIN(a,b) (((a) < (b)) ? (a) : (b))
#define ENCR_MAX(a,b) (((a) > (b)) ? (a) : (b))

typedef unsigned char byte;

#define STUB 1

CTexture* CTextureManager::ReadTextureFromPNG(string filename)
{
#ifndef STUB

    png_bytep header = new png_byte[1024];
    png_size_t number = 8;

    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp)
    {
        return (ERROR);
    }
    fread(header, 1, number, fp);

    corona::Image *image = corona::OpenImage(filename.c_str(), corona::PF_R8G8B8A8, corona::FF_PNG);

    if(!image)
    {
        return NULL;
    }
    CTexture *tex = new CTexture(image->getWidth(), image->getHeight(), RGBA);

    byte* pixels = (byte*)image->getPixels();

    for(int row = 0; row < image->getHeight(); ++row)
    {
        for(int col = 0; col < image->getWidth(); ++col)
        {
            float r = (*pixels)/255.f; pixels++;
            float g = (*pixels)/255.f; pixels++;
            float b = (*pixels)/255.f; pixels++;
            float a = (*pixels)/255.f; pixels++;

            int rowrow = image->getHeight() - row - 1;
            tex->SetPixel(col, rowrow, CTexel(r, g, b, a));
        }
    }
    
#else
    CTexture *tex = new CTexture(100, 100, RGBA);
    
    for(int row = 0; row < 100; ++row)
    {
        for(int col = 0; col < 100; ++col)
        {
            int rowrow = 100 - row - 1;
            tex->SetPixel(col, rowrow, CTexel(1,1,1,1));
        }
    }

#endif

    return tex;

}


void CTextureManager::WriteTextureToPNG(string filename, TextureGL<GLfloat> *tex)
{
    int width  = tex->Width();
    int height = tex->Height();
    int comp = 4; // rgba

    GLfloat *texData = tex->Data();

    byte* pImgData = new byte[width*comp*height];

    for(int row = 0; row < height; row++)
    {
        byte* p = pImgData+(((height-1)-row) * (width*comp));
        for (int col = 0; col < width; ++col) 
        {
            // red
            GLfloat v = *texData++;
            (*p++) = (byte)ENCR_MAX(ENCR_MIN(v * 255, 255), 0);
            // green
            v = *texData++;
            (*p++) = (byte)ENCR_MAX(ENCR_MIN(v * 255, 255), 0);
            // blue
            v = *texData++;
            (*p++) = (byte)ENCR_MAX(ENCR_MIN(v * 255, 255), 0);
            // alpha 
            v = *texData++;
            (*p++) = (byte)ENCR_MAX(ENCR_MIN(v* 255, 255), 0);
        }
    }

    stbi_write_png(filename.c_str(), width, height, comp, pImgData, width * comp);

}

CTexture* CTextureManager::ReadTextureFromENI(string filename)
{
    std::fstream textureFile(filename.c_str(), std::ios_base::in);

    if(textureFile.is_open() == false)
    { 
        return NULL;
    }

    int width, height;

    textureFile >> width >> height;

    CTexture *tex = new CTexture(width, height, RGBA);

    for(int y = 0; y < tex->GetHeight(); ++y)
    for(int x = 0; x < tex->GetWidth(); ++x)
    {
        float r, g, b, a;
        textureFile >> r >> g >> b >> a;
        tex->SetPixel(x, y, CTexel(r, g, b, a));
    }

    return tex;
}

void CTextureManager::WriteTextureToENI(string filename, CTexture *tex)
{
    std::fstream textureFile(filename.c_str(), std::ios_base::out);

    textureFile << tex->GetWidth() << " " << tex->GetHeight() << std::endl;

    for(int y = 0; y < tex->GetHeight(); ++y)
    {
        for(int x = 0; x < tex->GetWidth(); ++x)
        {
            CTexel t = tex->GetPixel(x, y);
            textureFile << t.R() << " " << t.G() << " " << t.B() << " " << t.A() << " ";
        }
        textureFile << std::endl;
    }

    textureFile.close();
}

void CTextureManager::WriteTextureToENI(string filename, TextureGL<GLfloat> *tex)
{
    std::fstream textureFile(filename.c_str(), std::ios_base::out);

    textureFile << tex->Width() << " " << tex->Height() << std::endl;

    GLfloat *texData = tex->Data();
    
    for(int y = 0; y < tex->Height(); ++y)
    {
        for(int x = 0; x < tex->Width(); ++x)
        {
            GLfloat r = *texData; texData++;
            GLfloat g = *texData; texData++;
            GLfloat b = *texData; texData++;
            GLfloat a = *texData; texData++;

            textureFile << r << " " << g << " " << b << " " << a << " ";
        }
        textureFile << std::endl;
    }

    textureFile.close();
}
