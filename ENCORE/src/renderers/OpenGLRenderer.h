#pragma once
#include "renderer.h"

class Camera;

#ifdef WIN32
#include <GL/glut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#endif

class OpenGLRenderer : public Renderer
{
public:
	OpenGLRenderer(void);
	virtual ~OpenGLRenderer(void);

	// functions from renderer
    virtual void usage( void ) {}
    virtual void configure( Options* l_pOptions ) {}
	virtual void init(Scene* scn, AccelerationStructure* accelStruct, Camera* cam);
	virtual void render(void);
	virtual void deinit(void);
    virtual void keyboard( unsigned char key ) {}

private:
    unsigned int m_nVertexIndexCount;
    unsigned int* m_pVertexIndices;
    float* m_pVertexCoords;
    float* m_pVertexNormals;
    float* m_pVertexColors;
};
