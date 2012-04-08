/****************************************************************************
Brandon Light
07/02/2006

FrameBufferManager.h

A class that manages the framebuffer attachments for FBOs


****************************************************************************/

#pragma once

#include <GLEW/glew.h>

///////   MACROS  /////////////////////////////////////////////////////////

#define GET_GLERROR(ret)                                          \
{                                                                 \
    GLenum err = glGetError();                                    \
    if (err != GL_NO_ERROR) {                                     \
    fprintf(stderr, "[%s line %d] GL Error: %s\n",                \
    __FILE__, __LINE__, gluErrorString(err));                     \
    fflush(stderr);                                               \
    }                                                             \
}

#define TEXTURELIMIT 4096
#define INVTL 1/TEXTURELIMIT


/////  DEFINITION  //////////////////////////////////////////////////////

enum GL_ATTACHMENT
{
    ZERO  = GL_COLOR_ATTACHMENT0_EXT,
    ONE   = GL_COLOR_ATTACHMENT1_EXT,
    TWO   = GL_COLOR_ATTACHMENT2_EXT,
    THREE = GL_COLOR_ATTACHMENT3_EXT,
};


class CFrameBufferObject
{
public:
    CFrameBufferObject();

    ~CFrameBufferObject();

    void Initalize(GLenum target, GLenum texTarget);

    void Bind();
    void Disable();

    void AttachTexture(GL_ATTACHMENT attachment, GLuint textureId);

    bool AddDrawBuffer(GL_ATTACHMENT attachment);

    void DrawBuffer(GL_ATTACHMENT attachment);
    void DrawBuffers();

    void ClearAttachment(GL_ATTACHMENT attachment);

private:  // METHODS

    void CheckFramebufferStatus();

private:  // DATA

    GLenum m_Target;
    GLenum m_TexTarget;

    GLuint m_FBOId;  // color render target
    bool m_bExtensionsInitialized;

    static const int m_MAX_ATTACHMENTS = 4;

    GLenum m_Buffers[m_MAX_ATTACHMENTS]; 
    int m_BuffersToDraw;
};