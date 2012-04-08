#pragma once

#include "Model.h"
#include "Triangle.h"
#include "Affine4.h"

#include <string>
#include <vector>
#include <list>

using namespace std;

struct FaceIndex
{
    unsigned int Index[3]; // indice to vertex data
    unsigned int tIndex[3]; // indice to texture coordinate data
};


class DynamicModel :
    public Model
{
public:
    DynamicModel(void);
    virtual ~DynamicModel(void);

    // add model of next keyframe to me
    void add( DynamicModel* dmodel );
    
    std::vector<Vertex>* getVertices() { return &m_Vertices; }
    std::vector<Triangle>* getTriangles() { return &m_Triangles; }

    vector<Vector3f>* getTexCoords() { return &m_TexCoords; }
    vector<FaceIndex>* getFaces() { return &m_FaceIndices; }
    std::list<std::vector<Vertex>*>* getVerticesList() { return &m_VerticesList; }

    const std::vector<Vertex>& getCVertices() const { return m_Vertices; }

    void getNextFrame();
    void getPrevFrame();
    bool isDirty() const { return m_bUpdated; }
    void setDynamicProperty(unsigned int fpkf);
    void resetFrame();
    

protected:

    unsigned int getVertexCount( void ) 
    {
        return (unsigned int)(*m_VerticesList.begin())->size(); 
    }

    void setTriangles(std::vector<Vertex>* f1, std::vector<Vertex>* f2);

    // model faces and texture are assumed to never change over the course of animation
    std::vector<Vector3f> m_TexCoords; // vector for texture coord data
    std::vector<FaceIndex> m_FaceIndices; // vector of indices

    // vertices are changing data
    std::list<std::vector<Vertex>*> m_VerticesList; // list of vertices

    // frame data
    std::list<std::vector<Vertex>*>::iterator m_itPrevF;             // prev keyframe
    std::list<std::vector<Vertex>*>::iterator m_itNextF;             // next keyframe
    std::list<std::vector<Vertex>*>::iterator m_itCurF;              // current keyframe
    unsigned int m_FramePerKeyframe;  // number of frames between key frames
    unsigned int m_FrameCount;        // total key frame count
    unsigned int m_TotalF;            // total frames base on frameperkeyframe
    unsigned int m_CurrentF;          // current frame out of frameperkeyframe
    bool m_bUpdated;
    
};
