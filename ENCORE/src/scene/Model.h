#pragma once

#include "IModel.h"
#include "Triangle.h"
#include "Affine4.h"
#include <string>
#include <vector>

class Model: public IModel
{
public:

    Model(Point3 center, float size, float angle, Vector3 axis, Material material);
    Model( std::string filename, const Point3& l_Center, float l_Size, float l_Angle, const Vector3& l_Axis, Material material);
    Model(): m_Size(1), m_AngleOfRotation(0),
      m_bHasMaterial(false), m_bHasNormals(false), m_bHasTexture(false) {}
    virtual ~Model( void );
    
    void load( std::string filename );
    void centerModel( const Point3& center, float size, float angle, const Vector3& axis);
    void calculateNormals( void );

    bool m_bHasMaterial;
    bool m_bHasNormals;
    bool m_bHasTexture;

    void buildPrimitiveList( void );

protected:

    void load3DS( std::string filename );
    void loadPLY( std::string filename );
    void loadVNT( std::string filename );
    void loadOBJ(const std::string &);
    
    std::vector<Vertex> m_Vertices;
    std::vector<Triangle> m_Triangles;

    Point3 m_CenterPoint;
    float m_Size;
    float m_AngleOfRotation;
    Vector3 m_AxisOfRotation;
    
    Affine4 getCenterMatrix(Point3 center, float size, float angle, const Vector3& axis);
    Affine4 rotate( float angle, const Vector3& axis);

    long EatChunk( char* buffer );
    
    unsigned int m_uiVertexCount;
    unsigned int m_uiTriangleCount;
    unsigned int m_uiTexcoordCount;
};
