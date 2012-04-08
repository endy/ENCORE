/****************************************************************************
Brandon Light
07/02/2006

FrameBufferManager.cpp

Implementation of a class that manages the framebuffer attachments for FBOs

****************************************************************************/

#include "FrameBufferObject.h"


#define GLH_EXT_SINGLE_FILE
#include <glh/glh_extensions.h>

/////////  MACROS  /////////////////////////////////////////////////////////

#define REQUIRED_EXTENSIONS	"GL_ARB_fragment_program "          \
                            "GL_EXT_framebuffer_object "        \
                            "GL_ATI_draw_buffers "              \
                            "GL_ARB_vertex_program "            \
                            "GL_ARB_texture_non_power_of_two "  \
                            "GL_NV_occlusion_query "            \
                            "GL_ARB_occlusion_query "

//        "GL_NV_vertex_program "
//        "GL_NV_fragment_program "

/////////   IMPLEMENTATION ////////////////////////////////////////////////



#include <cassert>

CFrameBufferObject::CFrameBufferObject()
: m_bExtensionsInitialized(false)
{
    m_BuffersToDraw = 0;
}

CFrameBufferObject::~CFrameBufferObject()
{
    if ( m_bExtensionsInitialized == true )
    {
        glDeleteFramebuffersEXT(1, &m_FBOId);
    }
}

void CFrameBufferObject::Initalize(GLenum target, GLenum texTarget)
{
    m_Target = target;
    m_TexTarget = texTarget;

    if (!glh_init_extensions(REQUIRED_EXTENSIONS)) 
    {
	    printf("Unable to load the following extension(s): %s\n\nExiting...\n", glh_get_unsupported_extensions());
	    exit(-1);
    }
    
    m_bExtensionsInitialized = true;

    glDeleteFramebuffersEXT(1, &m_FBOId);
    glGenFramebuffersEXT(1, &m_FBOId);
}

void CFrameBufferObject::Bind()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FBOId);
}

void CFrameBufferObject::Disable()
{
    // reset the draw target to the window
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void CFrameBufferObject::AttachTexture(GL_ATTACHMENT attachment, GLuint textureId)
{
    glFramebufferTexture2DEXT(m_Target, attachment, m_TexTarget, textureId, 0);
}

bool CFrameBufferObject::AddDrawBuffer(GL_ATTACHMENT attachment)
{
    if(m_BuffersToDraw < m_MAX_ATTACHMENTS)
    {
        int index = 0;
        while(index < m_BuffersToDraw)
        {
            if(m_Buffers[index] == attachment)
            {
                // buffer already added
                return true;
            }

            index++;
        }
        // add buffer since not in array yet
        m_Buffers[index] = attachment;
        m_BuffersToDraw++;

        return true;
    }
    else
    {
        // buffer list full!
        return false;
    }
}

void CFrameBufferObject::DrawBuffer(GL_ATTACHMENT attachment)
{
    glDrawBuffer(attachment);
    m_BuffersToDraw = 0;
}

void CFrameBufferObject::DrawBuffers()
{
    if(m_BuffersToDraw > 0)
    {
        // remember to init correct extension or a run time error will show up -- Jason?
        glDrawBuffersATI(m_BuffersToDraw, m_Buffers);
        m_BuffersToDraw = 0;
    }
}

void CFrameBufferObject::ClearAttachment(GL_ATTACHMENT attachment)
{
    glDrawBuffer(attachment);		
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CFrameBufferObject::CheckFramebufferStatus()
{
    GLenum status;
    status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            printf("Unsupported framebuffer format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            printf("Framebuffer incomplete, missing attachment\n");
            break;
        //case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
        //    printf("Framebuffer incomplete, duplicate attachment\n");
        //    break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            printf("Framebuffer incomplete, attached images must have same dimensions\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            printf("Framebuffer incomplete, attached images must have same format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            printf("Framebuffer incomplete, missing draw buffer\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            printf("Framebuffer incomplete, missing read buffer\n");
            break;
        default:
            assert(0);
    }
}