#include "CPURTRenderer.h"
#include <stdio.h>

#include "Primitive.h"

#ifdef WIN32
#include <GL/glut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#endif

/*********************
* CPURTRenderer class
**********************/

#define EPSILON 0.001

CPURTRenderer::CPURTRenderer(void)
{
    recursionDepth = 1;
    traversals = 1;
    this->alreadyBuiltScene = false;
    m_pixelSize = 1;
    m_view = PHONG;
}

CPURTRenderer::~CPURTRenderer(void)
{
}

void CPURTRenderer::usage( void )
{
    printf("CPU Ray Tracer Configuration Options\n");
    printf("\tPIXEL_SIZE\n");
}

void CPURTRenderer::configure( Options* l_pOptions )
{
    if ( l_pOptions->doesOptionExist( "PIXEL_SIZE" ) )
    {
        m_pixelSize = atoi( l_pOptions->getOption( "PIXEL_SIZE" ).c_str() );
    }
}

void CPURTRenderer::init(Scene* l_pScene, AccelerationStructure* l_pAccelStruct, Camera* l_pCamera)
{
    m_pCamera = l_pCamera;

    numCols = l_pCamera->GetViewportWidth(); // 512;
    numRows = l_pCamera->GetViewportHeight(); //512;

    //set up OpenGL for simple 2D drawing
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, numCols, 0, numRows);
    glDisable(GL_LIGHTING); // so glColor3f() works properly

    m_pScene = l_pScene;
    m_pAccelStruct = l_pAccelStruct;
    
    std::list<IModel*> models = m_pScene->getModelList();
    m_pAccelStruct->build(models);
}

void CPURTRenderer::deinit(void)
{
}

Color CPURTRenderer::phongShade(HitInfo bestHit, Ray* l_Ray)
{
    Color backgroundColor = m_pScene->getBackgroundColor();
    Color color;

    if (bestHit.bHasInfo == false)
    {
        color = Color( backgroundColor );
    }
    else
    {
        Material* newMat = bestHit.hitObject->getMaterial();

        color = Color( newMat->GetAmbient() );

/*
        Ray feeler;
        feeler.Origin() = Point3f( bestHit->hitPoint.X() + (l_Ray->Direction().X() - EPSILON),
                                   bestHit->hitPoint.Y() + (l_Ray->Direction().Y() - EPSILON),
                                   bestHit->hitPoint.Z() + (l_Ray->Direction().Z() - EPSILON) );
//        feeler.m_nRecurseLevel = 1;
*/
        std::list< IEmissive* >::iterator alight;
        std::list< IEmissive* >* lightList = m_pScene->getLightList();

        for (alight = lightList->begin(); alight != lightList->end(); alight++)
        {
            Color lightColor = (*alight)->GetEmissiveColor();
            const Point3f lightPos = (*alight)->GetPointOfEmission();

            Color contribution;
/*
            feeler.Direction() = lightPos - feeler.Origin();
            if ( IsInShadow( feeler ) )
            {
                // don't calculate light contribution for this light
                continue;
            }
*/
            // Compute the diffuse term

            Vector3f L(bestHit.hitPoint, lightPos);
            L.Normalize();

            float NdotL = Dot(L ,bestHit.hitNormal);

            if (NdotL > 0.0) // is normal within 90 degrees of light?
            {
                Color diffuse( lightColor * newMat->GetDiffuse() * NdotL );
                contribution += diffuse;
            }

            // Compute the specular term
            Vector3f V(bestHit.hitPoint, l_Ray->Origin());
            V.Normalize();

            Vector3f H(L + V);
            H.Normalize();

            float NdotH = Dot(H, bestHit.hitNormal);
            if ( NdotH > 0 )
            {
                float phong = pow( NdotH, newMat->GetSpecularExponent() );
                Color specular( lightColor * newMat->GetSpecular() * phong );
                contribution += specular;
            }

            color += contribution; // this should probably be multiply
        }
    }

    return color;
}

void CPURTRenderer::keyboard( unsigned char key )
{
    switch(key)
    {
        case '+':
            m_pixelSize++;
            break;
        case '-':
            if ( m_pixelSize > 0)
                m_pixelSize--;
            break;
        case 't':
            traversals++;
            break;
        case 'r':
            if ( traversals > 0 )
                traversals--;
            break;
        case 'p':
            m_view = PHONG;
            break;
        case 'n':
            m_view = NORMAL;
            break;
        case 'v':
            m_view = VOXEL;
            break;
    }
}


Color CPURTRenderer::shade( Ray* l_pRay )
{
    Color c;
    HitInfo hit = m_pAccelStruct->intersect( *l_pRay );
    if ( m_view == VOXEL )
    {
        if ( hit.bHasInfo == false )
        {
            c = m_pScene->getBackgroundColor();
        }
        else
        {
            Point3f ret = hit.hitVoxel;
            c = Color( ret.X(), ret.Y(), ret.Z(), 0 );
        }
    }
    else if ( m_view == PHONG )
    {
        c = phongShade( hit, l_pRay );
    }
    else if ( m_view == NORMAL )
    {
        c = Color( hit.hitNormal.X(), hit.hitNormal.Y(), hit.hitNormal.Z(), 0 );
    }

    return c;
}

void CPURTRenderer::render()
{
    glViewport(0, 0, m_pCamera->GetViewportWidth(), m_pCamera->GetViewportHeight());
    glEnable(GL_DEPTH_TEST);

    Color bgColor = m_pScene->getBackgroundColor();
//      //glClearColor( bgColor.R(), bgColor.G(), bgColor.B(), bgColor.A() );
    glClearColor( 0, 0, 0, 0 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float Wconst = 2*m_pCamera->GetWorldWidth()/(float)numCols;
    float Hconst = 2*m_pCamera->GetWorldHeight()/(float)numRows;
    
    for (int row=0; row < this->numRows; row += m_pixelSize){
        for (int col=0; col < this->numCols; col += m_pixelSize){
            float x = -m_pCamera->GetWorldWidth() + col*Wconst;
            float y = -m_pCamera->GetWorldHeight() + row*Hconst;
            Ray aRay;
            aRay.Direction() = Vector3f(  -m_pCamera->GetNearClipPlane()*m_pCamera->N().X() + x*m_pCamera->U().X() + y*m_pCamera->V().X(),
                                -m_pCamera->GetNearClipPlane()*m_pCamera->N().Y() + x*m_pCamera->U().Y() + y*m_pCamera->V().Y(),
                                -m_pCamera->GetNearClipPlane()*m_pCamera->N().Z() + x*m_pCamera->U().Z() + y*m_pCamera->V().Z());
            
            aRay.Origin() = Point3f(m_pCamera->GetEye().X(), m_pCamera->GetEye().Y(), m_pCamera->GetEye().Z());

            aRay.Direction().Normalize();

            Color finalColor = shade( &aRay );

            glColor3f(finalColor.R(), finalColor.G(), finalColor.B());
			glRecti(col,row, col+m_pixelSize, row+m_pixelSize);
        }
    }

    glutSwapBuffers();
}