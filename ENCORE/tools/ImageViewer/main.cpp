/*
 *  main.h
 *  SDLOpenGLTest
 *
 *  Created by Brandon Light on 6/3/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "SDL/SDL.h"
#include "SDL/SDL_OpenGL.h"

#ifndef WIN32
#include <OpenGL/gl.h>
#endif

#include <iostream>
#include <string>

#include "TextureManager.h"

///////////////////////  GLOBALS  ////////////////////////////////////////
const int WINDOW_POS_X  = 100;
const int WINDOW_POS_Y  = 100;

CTexture *g_Image = NULL;

//////////////////////////////////////////////////////////////////////////

void input()
{

}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, g_Image->GetWidth(), 0, g_Image->GetHeight(), -0.1, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 


    // draw image!

    for(int x = 0; x < g_Image->GetWidth(); ++x)
        for(int y = 0; y < g_Image->GetHeight(); ++y)
        {
            CTexel t = g_Image->GetPixel(x, y);
            glColor3f(t.R(), t.G(), t.B());
            glBegin(GL_POLYGON);
            glVertex3i(x, y, 0);
            glVertex3i(x+1, y, 0);
            glVertex3i(x+1, y+1, 0);
            glVertex3i(x, y+1, 0);
            glEnd();        
        }
}


int main(int argc, char *argv[])
{
    if(argc == 2)
    {
        std::string imageFile = argv[1];

        size_t periodPos = imageFile.rfind('.');

        int extLength = imageFile.length() - periodPos;

        char *ext = new char[extLength];
        std::fill(ext, ext+extLength, 0);
        imageFile.copy(ext, extLength-1, periodPos+1);

        if(strcmp(ext,"png") == 0)
        {
            g_Image = CTextureManager::ReadTextureFromPNG(imageFile);
        }
    }
    else
    {
        return 0;
    }
    
    int error;
    error = SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    SDL_Surface* drawContext;
    
    Uint32 flags;
    
    flags = SDL_OPENGL | SDL_RESIZABLE;
    drawContext = SDL_SetVideoMode(g_Image->GetWidth(), g_Image->GetHeight(), 0, flags);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  
    // draw code
    glClear(GL_COLOR_BUFFER_BIT);
        
    display();
        
    SDL_GL_SwapBuffers();
        
    // get input 
    SDL_Event event;
        
    bool done = false;
    while((!done) && (SDL_WaitEvent(&event)))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                done = true;
                break;
            default:   
                break;
        }
    }
        
    
    SDL_Quit();
    

    return 0;
}