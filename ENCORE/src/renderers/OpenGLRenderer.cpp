#include "OpenGLRenderer.h"
#include <assert.h>
#include "Camera.h"
#include "Triangle.h"

OpenGLRenderer::OpenGLRenderer(void)
: m_pVertexIndices(NULL), m_pVertexCoords(NULL), m_pVertexNormals(NULL), m_pVertexColors(NULL)
{
}

OpenGLRenderer::~OpenGLRenderer(void)
{
    if ( m_pVertexIndices ) delete [] m_pVertexIndices;
    if ( m_pVertexCoords ) delete [] m_pVertexCoords;
    if ( m_pVertexNormals ) delete [] m_pVertexNormals;
    if ( m_pVertexColors ) delete [] m_pVertexColors;
}

// methods required for renderer
void OpenGLRenderer::init(Scene* scn, AccelerationStructure* accelStruct, Camera* cam)
{
    m_pScene = scn;
    m_pCamera = cam;

    std::list<Triangle*>::iterator pTri;
    std::list<Triangle*>* pTriangleList = scn->getNewTesselation();

    m_nVertexIndexCount = (unsigned int)pTriangleList->size();

    m_pVertexIndices = new unsigned int[ m_nVertexIndexCount*3 ];
    m_pVertexCoords = new float[ m_nVertexIndexCount*3*3 ];
    m_pVertexNormals = new float[ m_nVertexIndexCount*3*3 ];
    m_pVertexColors = new float[ m_nVertexIndexCount*3*3 ];

    assert( m_pVertexIndices );
    assert( m_pVertexCoords );
    assert( m_pVertexNormals );
    assert( m_pVertexColors );

    // get pointers that will traverse the arrays
    unsigned int* vi = m_pVertexIndices;
    float* vc = m_pVertexCoords;
    float* vn = m_pVertexNormals;
    float* vclr = m_pVertexColors;

    unsigned int count = 0;
    for( pTri = pTriangleList->begin(); pTri != pTriangleList->end(); pTri++ )
    {
        *vi = count; vi++; count++;
        *vi = count; vi++; count++;
        *vi = count; vi++; count++;

        *vc = (*pTri)->getVertex0()->getCoordinates().X(); vc++;
        *vc = (*pTri)->getVertex0()->getCoordinates().Y(); vc++;
        *vc = (*pTri)->getVertex0()->getCoordinates().Z(); vc++;
        *vc = (*pTri)->getVertex1()->getCoordinates().X(); vc++;
        *vc = (*pTri)->getVertex1()->getCoordinates().Y(); vc++;
        *vc = (*pTri)->getVertex1()->getCoordinates().Z(); vc++;
        *vc = (*pTri)->getVertex2()->getCoordinates().X(); vc++;
        *vc = (*pTri)->getVertex2()->getCoordinates().Y(); vc++;
        *vc = (*pTri)->getVertex2()->getCoordinates().Z(); vc++;

        *vn = (*pTri)->getVertex0()->getNormal().X(); vn++;
        *vn = (*pTri)->getVertex0()->getNormal().Y(); vn++;
        *vn = (*pTri)->getVertex0()->getNormal().Z(); vn++;
        *vn = (*pTri)->getVertex1()->getNormal().X(); vn++;
        *vn = (*pTri)->getVertex1()->getNormal().Y(); vn++;
        *vn = (*pTri)->getVertex1()->getNormal().Z(); vn++;
        *vn = (*pTri)->getVertex2()->getNormal().X(); vn++;
        *vn = (*pTri)->getVertex2()->getNormal().Y(); vn++;
        *vn = (*pTri)->getVertex2()->getNormal().Z(); vn++;

        float r = (*pTri)->getMaterial()->GetDiffuse().R();
        float g = (*pTri)->getMaterial()->GetDiffuse().G();
        float b = (*pTri)->getMaterial()->GetDiffuse().B();
        *vclr = r; vclr++;
        *vclr = g; vclr++;
        *vclr = b; vclr++;
        *vclr = r; vclr++;
        *vclr = g; vclr++;
        *vclr = b; vclr++;
        *vclr = r; vclr++;
        *vclr = g; vclr++;
        *vclr = b; vclr++;

        delete *pTri;
    }
    pTriangleList->clear();
    delete pTriangleList;
    pTriangleList = NULL;
}

void OpenGLRenderer::render( void )
{
#ifdef WIN32
    DWORD dwRenderTime = timeGetTime();
#endif
    glViewport(0, 0, m_pCamera->GetViewportWidth(), m_pCamera->GetViewportHeight());
    glEnable(GL_DEPTH_TEST);
    m_pCamera->setProjectionMatrix();
    m_pCamera->setModelViewMatrix();

    std::list< IEmissive* >::iterator light;
    std::list< IEmissive* >* lightList = m_pScene->getLightList();

    // enable lights
    int lightNum = GL_LIGHT0;
    if ( lightList->size() > 0 )
    {
        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
    }

    for( light = lightList->begin(); light != lightList->end(); light++ )
    {
        glEnable(lightNum);
        const Color c = (*light)->GetEmissiveColor();
        const Point3f p = (*light)->GetPointOfEmission();
        float lightIntensity[] = { c.R(), c.G(), c.B(), c.A() };
        float lightPosition[] = { p.X(), 
                                  p.Y(),
                                  p.Z(),
                                  1.0f };
        glLightfv(lightNum, GL_POSITION, lightPosition);
        glLightfv(lightNum, GL_DIFFUSE, lightIntensity);
        lightNum++;
    }

    Color bgColor = m_pScene->getBackgroundColor();
    glClearColor( bgColor.R(), bgColor.G(), bgColor.B(), bgColor.A() );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f( 1, 0, 0 );
    // draw scene
    glEnableClientState( GL_NORMAL_ARRAY );
    glNormalPointer( GL_FLOAT, sizeof(float)*3, m_pVertexNormals );

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer(3, GL_FLOAT, sizeof(float)*3, m_pVertexCoords);

//    glEnableClientState( GL_COLOR_ARRAY );
//    glColorPointer(3, GL_FLOAT, sizeof(float)*3, m_pVertexColors);

    glDrawElements(GL_TRIANGLES, m_nVertexIndexCount*3, GL_UNSIGNED_INT, m_pVertexIndices);

    // disable lights
    lightNum = GL_LIGHT0;
    if ( lightList->size() > 0 )
    {
        glDisable(GL_LIGHTING);
    }
    for( light = lightList->begin(); light != lightList->end(); light++ )
    {
        glDisable(lightNum);
        lightNum++;
    }
    glFlush();
    
#ifdef WIN32
    dwRenderTime = timeGetTime() - dwRenderTime;
//    printf("OpenGL Image. %.3f\n", (float)dwRenderTime/1000.0f );
#endif

    glutSwapBuffers();
}

void OpenGLRenderer::deinit(void)
{
    if ( m_pVertexIndices ) delete [] m_pVertexIndices;
    if ( m_pVertexCoords ) delete [] m_pVertexCoords;
    if ( m_pVertexNormals ) delete [] m_pVertexNormals;
    m_pVertexIndices = NULL;
    m_pVertexCoords = NULL;
    m_pVertexNormals = NULL;
}
