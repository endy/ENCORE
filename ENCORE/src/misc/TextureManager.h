#pragma once 

#include "Texture.h"

#include <string>

using std::string;

// ENI = ENcore Image

class CTextureManager
{
public:
    // write a TextureGLf object to disk as a PNG
    
    // PNG texture functions
    static CTexture* ReadTextureFromPNG(string filename);
    static void WriteTextureToPNG(string filename, CTexture *tex);
    static void WriteTextureToPNG(string filename, TextureGLf *tex);
    
    // ENI texture functions
    static CTexture* ReadTextureFromENI(string filename);
    static void WriteTextureToENI(string filename, CTexture *tex);
    static void WriteTextureToENI(string filename, TextureGLf *tex);
    
private:
    // prevent instantiation
    CTextureManager(){};
};
