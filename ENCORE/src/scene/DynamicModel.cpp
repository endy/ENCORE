#include "DynamicModel.h"
#include "TrianglePrim.h"
#include <assert.h>

#include <iostream>
using namespace std;

DynamicModel::DynamicModel(void)
: m_bUpdated(false), m_FramePerKeyframe(1), m_TotalF(1), m_FrameCount(1),
  m_CurrentF(0)
{
}

DynamicModel::~DynamicModel(void)
{
// there is probably more cleaning up that has to be done
// since there are memory leaks now
    std::list< std::vector< Vertex >* >::iterator vit;
    for ( vit = m_VerticesList.begin(); vit != m_VerticesList.end(); vit++ )
    {
        if ( (*vit) != NULL )
        {
            //delete (*vit);
            (*vit) = NULL;
        }
    }
    m_VerticesList.clear();
}

void DynamicModel::add( DynamicModel* dmodel )
{
    //printf("%i and %i ", dmodel->getVertexCount(), this->getVertexCount());
    assert( ( dmodel->getVertexCount() == getVertexCount() ) );  

    std::vector<Vertex>* m;
    // if the size is one = its pointing to its vertices data right now
    // create a new copy as first one (avoid wrong pointer later on when interpolate happen)
    /*
    if(m_VerticesList.size() == 1)
    {
        m = new std::vector<Vertex>(this->getCVertices());
        m_VerticesList.clear(); // clear old one
        m_VerticesList.push_back(m);
        resetFrame();
    }
    */
    m = new std::vector<Vertex>(dmodel->getCVertices());
    m_VerticesList.push_back(m);
    //printf("list size: %i\n", verticesList.size());
}

void DynamicModel::resetFrame()
{
    m_itNextF = m_itPrevF = m_itCurF = m_VerticesList.begin();
    
    // move keyframe iterator to 2nd keyframe
    m_itNextF++;
    
    // make sure the keyframe exists
    if ( m_itNextF == m_VerticesList.end() )
    {
        // if so, set next keyframe to be the first key frame
        // since there is only one frame
        m_itNextF = m_VerticesList.begin();
    }

    // now set the triangles / build the primitives
    if(this->m_bUpdated)
        setTriangles( (*m_itCurF), (*m_itNextF) );
/*
    if( m_itCurF == m_VerticesList.end() )
    {
        m_itNextF = m_VerticesList.begin();
    }
    else
    {
        m_itNextF = m_itCurF;
        m_itNextF++; 
    }
    m_CurrentF = 0;
    setTriangles((*m_itCurF), (*m_itCurF));
*/
}

void DynamicModel::getNextFrame()
{
    // First, increase to next frame
    
    m_CurrentF++;
    if ( m_CurrentF == m_FramePerKeyframe )
    {
        // we're at the start of the next keyframe
        m_CurrentF = 0;

        // so we need to iterate to the next keyframes
        m_itCurF = m_itNextF;
        m_itNextF++;
        
        // make sure we haven't run out of keyframes
        if ( m_itNextF == m_VerticesList.end() )
        {
            //reset back to beginning
            m_itNextF = m_VerticesList.begin();
        }
    }
    // early out if its the same!
    if(m_itCurF == m_itNextF)
        m_bUpdated = false;
    else
        m_bUpdated = true;

    // then, set triangles/build primitives
    if(this->m_bUpdated)
        setTriangles((*m_itCurF), (*m_itNextF));
    
}

void DynamicModel::getPrevFrame()
{
    // First, decrement to prev frame
    m_CurrentF--;
    if ( m_CurrentF == 0 )
    {
        // we're at the end of the prev keyframe
        m_CurrentF = m_FramePerKeyframe;

        // so we need to iterate to the prev keyframes
        m_itNextF = m_itCurF;
        m_itCurF--;
        
        // make sure we haven't run out of keyframes
        if ( m_itCurF == m_VerticesList.begin() )
        {
            //reset back to end
            m_itCurF = m_VerticesList.end();
        }
    }

    // then, set triangles/build primitives
    setTriangles((*m_itCurF), (*m_itNextF));
}

void DynamicModel::setTriangles( std::vector<Vertex> *f1, std::vector<Vertex> *f2 )
{
    // ensure we have the same number of vertices
    unsigned int fs1 = (unsigned int)f1->size();
    unsigned int fs2 = (unsigned int)f2->size();
    assert( "DynamicModel.setTriangles(): vector sizes are different" && (fs1 == fs2) );

    unsigned int now;
    std::vector<Vertex>::iterator vit = m_Vertices.begin(); // iterator for display data

    // check frame state
    if( m_CurrentF == 0 )
    {
        // at the beginning of a keyframe,
        // so just make new triangles using the vertices from f1
        m_Triangles.clear();
        
        for( now = 0; now < m_FaceIndices.size(); now++ )
        {
            //cout << (*f1)[m_FaceIndices[now].Index[0]].getCoordinates().X() << " "
            //<< (*f1)[m_FaceIndices[now].Index[0]].getCoordinates().Y() << " "
            //<< (*f1)[m_FaceIndices[now].Index[0]].getCoordinates().Z() << endl;
        
            Triangle t ( (*f1)[m_FaceIndices[now].Index[0]],
                         (*f1)[m_FaceIndices[now].Index[1]],
                         (*f1)[m_FaceIndices[now].Index[2]] );
            m_Triangles.push_back( t );
        }
    }
    else
    {
        // we need to interpolate between keyframes

        const float ff = (const float)m_FramePerKeyframe;
        const float cf = (const float)m_CurrentF;

        // remove the old triangles
        m_Triangles.clear();

        // use the face information to generate the triangles
        for( now = 0; now < m_FaceIndices.size(); now++ )
        {
            Point3f c1,c2;
            Vector3f n1,n2;
            
            // interpolate the coordinates and normal for all three vertices
            Vertex v[3];
            for (int i = 0; i < 3; i++ )
            {
                c1 = (*f1)[m_FaceIndices[now].Index[i]].getCoordinates();
                c2 = (*f2)[m_FaceIndices[now].Index[i]].getCoordinates();
                n1 = (*f1)[m_FaceIndices[now].Index[i]].getNormal();
                n2 = (*f2)[m_FaceIndices[now].Index[i]].getNormal();

                Point3f nC = c1 + ((c2 - c1)/ff)*cf;
                Vector3f nN = n1 + ((n2 - n1)/ff)*cf;
                
                v[i].set( nC[0], nC[1], nC[2], nN[0], nN[1], nN[2] );
            }

            // make a triangle with it
            Triangle t ( v[0], v[1], v[2] );
            m_Triangles.push_back( t );
        }
    }

    // build primitives with the updated triangles
    buildPrimitiveList();
    //cout << "-";
}

void DynamicModel::setDynamicProperty(unsigned int fpkf)
{
    m_FramePerKeyframe = fpkf;
    m_itNextF = m_itPrevF = m_itCurF = m_VerticesList.begin();
    
    if( m_itCurF == m_VerticesList.end() )
        m_itNextF = m_VerticesList.begin();
    else 
    {
        m_itNextF = m_itCurF; 
        m_itNextF++; 
    }

    m_FrameCount = (unsigned int)m_VerticesList.size();
    m_TotalF = m_FrameCount*m_FramePerKeyframe;
    m_CurrentF = 0;
}

