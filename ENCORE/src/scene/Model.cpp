
#include <cstdio>
#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include "Model.h"
#include <cassert>
#include "TrianglePrim.h"
#include "Objloader.h"

#include "Common.h"

using encore::Color;

#define ENCR_MIN(a,b) (((a) < (b)) ? (a) : (b))
#define ENCR_MAX(a,b) (((a) > (b)) ? (a) : (b))

Model::Model(Point3 center, float size, float angle, Vector3 axis, Material material)
{
    m_CenterPoint       = center;
    m_Size              = size;
    m_AngleOfRotation   = angle;
    m_AxisOfRotation    = axis;
    
    SetMaterial(material);

    m_bHasMaterial = false;
    m_bHasNormals = false;
    m_bHasTexture = false;
}

Model::Model(std::string filename, const Point3& center, float size, float angle, const Vector3& axis, Material material)
{
    m_CenterPoint       = center;
    m_Size              = size;
    m_AngleOfRotation   = angle;
    m_AxisOfRotation    = axis;
    
    SetMaterial(material);
    
    m_bHasMaterial = false;
    m_bHasNormals = false;
    m_bHasTexture = false;

    load( filename );
}

Model::~Model( void )
{
    std::list<IPrimitive*>::iterator i;
    for (i = m_PrimitiveList.begin(); i != m_PrimitiveList.end(); i++ )
    {
        delete *i;
    }
    m_PrimitiveList.clear();
}

void Model::load( std::string fname )
{
    m_maxPoint = Point3(NEG_INF, NEG_INF, NEG_INF);
    m_minPoint = Point3(POS_INF, POS_INF, POS_INF);

    // determine extension
    size_t extstart = fname.find_last_of('.',fname.length());
    if ((int)extstart == std::string::npos)
    {
        std::cout << "cannot parse file extension of " << fname << std::endl;
    }
    else
    {
        std::string extension = fname.substr(extstart+1,fname.length());
        std::cout << "parsing " << fname << std::endl;
        if (extension == "3DS" || extension == "3ds")
        {
            load3DS(fname);
        }
        else if (extension == "VNT" || extension == "vnt")
        {
            loadVNT(fname);
        }
        else if (extension == "PLY" || extension == "ply")
        {
            loadPLY(fname);
        }
        else if (extension == "OBJ" || extension == "obj")
        {
            loadOBJ(fname);
        }
        else
        {
            std::cout << "Cannot recognize file format of " << fname << std::endl;
        }
    }

    centerModel( m_CenterPoint, m_Size, m_AngleOfRotation, m_AxisOfRotation );

    if ( m_bHasNormals == false )
    {
        calculateNormals();
    }

    buildPrimitiveList( );

    std::cout << "Primitive Count: " << (unsigned int)m_PrimitiveList.size() << std::endl;
}

void Model::centerModel(const Point3& center, float size, float angle, const Vector3& axis)
{
    Affine4 centerMatrix = getCenterMatrix( center, size, angle, axis);

    // move max and min point
    float x,y,z;

    m_maxPoint[0] = m_maxPoint[1] = m_maxPoint[2] = NEG_INF;
    m_minPoint[0] = m_minPoint[1] = m_minPoint[2] = POS_INF;

    Point3 newCoord;

    // translate, scale and rotate vertex
    std::vector<Vertex>::iterator iv;
    for ( iv = m_Vertices.begin(); iv != m_Vertices.end(); iv++ )
    {
        x = iv->getCoordinates().X();
        y = iv->getCoordinates().Y();
        z = iv->getCoordinates().Z();

        newCoord.X() = x * centerMatrix.m[0] + y * centerMatrix.m[4] + z * centerMatrix.m[8] + centerMatrix.m[12];
        newCoord.Y() = x * centerMatrix.m[1] + y * centerMatrix.m[5] + z * centerMatrix.m[9] + centerMatrix.m[13];
        newCoord.Z() = x * centerMatrix.m[2] + y * centerMatrix.m[6] + z * centerMatrix.m[10] + centerMatrix.m[14];
  
        for(int i = 0; i < 3; i++)
        {
            m_maxPoint[i] = ENCR_MAX(m_maxPoint[i], newCoord[i]);
            m_minPoint[i] = ENCR_MIN(m_minPoint[i], newCoord[i]);
        }

        iv->setCoordinates( newCoord );
    }
}

Affine4 Model::getCenterMatrix( Point3 c, float s, float angle, const Vector3& axis )
{
    Vector3 modelLength( m_minPoint, m_maxPoint );

    float scale = s/ENCR_MAX(modelLength.X(),
                        ENCR_MAX(modelLength.Y(),
                            modelLength.Z()));

    Point3 center = (m_maxPoint+m_minPoint)*.5;

    Point3 scaledCenter(center.X() * scale, center.Y() * scale, center.Z() * scale);
    Vector3 offset(c, scaledCenter);

    Affine4 centerMatrix = rotate( angle, axis );
    // translate
    centerMatrix.m[12] = -offset.X()*centerMatrix.m[0]-offset.Y()*centerMatrix.m[4]-offset.Z()*centerMatrix.m[8];
    centerMatrix.m[13] = -offset.X()*centerMatrix.m[1]-offset.Y()*centerMatrix.m[5]-offset.Z()*centerMatrix.m[9];
    centerMatrix.m[14] = -offset.X()*centerMatrix.m[2]-offset.Y()*centerMatrix.m[6]-offset.Z()*centerMatrix.m[10];
    // scale
    centerMatrix.m[0] *= scale;
    centerMatrix.m[1] *= scale;
    centerMatrix.m[2] *= scale;
    centerMatrix.m[4] *= scale;
    centerMatrix.m[5] *= scale;
    centerMatrix.m[6] *= scale;
    centerMatrix.m[8] *= scale;
    centerMatrix.m[9] *= scale;
    centerMatrix.m[10] *= scale;

    return centerMatrix;
}

Affine4 Model::rotate( float angle, const Vector3& u )
{
    #define EPS 0.0000001
    Affine4 R;
    float rad = (float)(angle/180.0*M_PI);
    if( rad <= EPS && rad >= -EPS )
    {
        return R;
    }

    float c = cos(rad);
    float s = sin(rad);

    // constructive method for arbitary axis rotation
    R.m[0] = c + (1-c)*u.X()*u.X();
    R.m[4] = (1-c)*u.Y()*u.X() - s*u.Z();
    R.m[8] = (1-c)*u.Z()*u.X() + s*u.Y();

    R.m[1] = (1-c)*u.X()*u.Y() + s*u.Z();
    R.m[5] = c + (1-c)*u.Y()*u.Y();
    R.m[9] = (1-c)*u.Z()*u.Y() - s*u.X();

    R.m[2] = (1-c)*u.X()*u.Z() - s*u.Y();
    R.m[6] = (1-c)*u.Y()*u.Z() + s*u.X();
    R.m[10] = c + (1-c)*u.Z()*u.Z();

    return R;
}

void Model::calculateNormals( void )
{
    Point3 a,b,c;
    Vector3 n;

    std::vector<Triangle>::iterator it;

    // find per-Triangle Normal
    for ( it = m_Triangles.begin(); it != m_Triangles.end(); it++ )
    {
        a = Point3( it->getVertex0()->getCoordinates() );
        b = Point3( it->getVertex1()->getCoordinates() );
        c = Point3( it->getVertex2()->getCoordinates() );

        n = Cross(Vector3(a, b),Vector3(a, c));

        it->getVertex0()->setNormal( n );
        it->getVertex1()->setNormal( n );
        it->getVertex2()->setNormal( n );
    }

    // calculate vertex normals
    std::vector<Vertex>::iterator iv;
//    size_t count = 0;
//    printf("total vertex: %i\n", m_Vertices.size());
    for ( iv = m_Vertices.begin(); iv != m_Vertices.end(); iv++ )
    {
        n = Vector3(0,0,0); // reset normal
        for( it = m_Triangles.begin(); it != m_Triangles.end(); it++ )
        {
            // if the address of one of the triangle Vertices
            // is the same as the address of *iv,
            if ( it->getVertex0()->equals( *iv ) ||
                 it->getVertex1()->equals( *iv ) ||
                 it->getVertex2()->equals( *iv ) )
            {
                n += iv->getNormal();
            }
//            count++;
        }
        // normalize
        n.Normalize();
        // set
        iv->setNormal( n );
    }
//    printf("done normal\n %i", count);

    m_bHasNormals = true;
}

// this function assumes everything is a TrianglePrim
void Model::buildPrimitiveList( void )
{
    std::vector<Triangle>::iterator i;
    for( i = m_Triangles.begin(); i != m_Triangles.end(); i++ )
    {
        TrianglePrim* pTp = new TrianglePrim( *i );
        pTp->setMaterial(m_Material);
        m_PrimitiveList.push_back( pTp );
    }
}

/*************
* loading specific model types:
* 3DS   PLY   VNT
*************/
unsigned char temp[8];
unsigned char* aligned = (unsigned char*)(((unsigned long)temp + 4) & 0xFFFFFFFC);
inline float getf( char* addr )
{
  memcpy( aligned, addr, 4 );
  return *(float*)aligned;
}
inline unsigned short getw( char* addr )
{
  memcpy( aligned, addr, 2 );
  return *(unsigned short*)aligned;
}
inline unsigned long getd( char* addr )
{
  memcpy( aligned, addr, 4 );
  return *(unsigned long*)aligned;
}

long Model::EatChunk( char* buffer )
{

    short chunkid = getw( buffer );
    long chunklength = getd( buffer + 2 );

    unsigned int j, i = 6; 
    // unsigned int cp = 0;
    float x,y,z;
    switch (chunkid)
    {
        case 0x4D4D:
            while ((getw( buffer + i) != 0x3D3D) && (getw( buffer + i) != 0xB000)) i += 2;
            break;
        case 0x4000:
            while (*(buffer + (i++)) != 0);
            break;
        case 0x4110:
            m_uiVertexCount = getw( buffer + i );

            i += 2;
            for ( j = 0; j < m_uiVertexCount; j++ )
            {
                // x is correct, but need to swap y and z
                x = getf( buffer + i );
                z = getf( buffer + i + 4 );
                y = getf( buffer + i + 8 );
                m_maxPoint.X() = max(m_maxPoint.X(), x);
                m_maxPoint.Y() = max(m_maxPoint.Y(), y);
                m_maxPoint.Z() = max(m_maxPoint.Z(), z);
                m_minPoint.X() = min(m_minPoint.X(), x);
                m_minPoint.Y() = min(m_minPoint.Y(), y);
                m_minPoint.Z() = min(m_minPoint.Z(), z);
                i += 12;

                Vertex v;
                v.setCoordinates( Point3( x, y, z ) );
                m_Vertices.push_back( v );
            }
            break;
        case 0x4120:
            m_uiTriangleCount = getw( buffer + i );

            unsigned int v0, v1, v2;
            i += 2;
            for ( j = 0; j < m_uiTriangleCount; j++ )
            {
                v0 = getw( buffer + i );
                v1 = getw( buffer + i + 2 );
                v2 = getw( buffer + i + 4 );
                i += 8;

                Triangle t( m_Vertices[v0], m_Vertices[v1], m_Vertices[v2] );
                m_Triangles.push_back( t );
            }
            break;
        case 0x4140:
            m_uiTexcoordCount = getw( buffer + i );
            m_bHasTexture = true;

            float x, y;
            i += 2;
            for ( j = 0; j < m_uiTexcoordCount; j++ )
            {
                x = getf( buffer + i );
                y = getf( buffer + i + 4 );
                i += 8;

                m_Vertices[j].setTexCoord( x, y );
            }
        case 0x3D3D:
        case 0x4100:
            break;
        default:
            i = chunklength;
            break;
    }

    while (i < (unsigned int)chunklength)
    {
        i += EatChunk( buffer + i );
    }

   return chunklength;
}


void Model::load3DS( std::string fname )
{
    // load 3ds file to memory
    FILE* f = fopen( fname.c_str(), "rb" );
    if(f == NULL)
    {
        printf("Unable to open 3DS file %s",fname.c_str());
        return;
    }
    fseek (f , 0 , SEEK_END);
    unsigned long filelength = ftell (f);
    rewind (f);
    char* memfile = new (std::nothrow) char[filelength];
    fread( memfile, filelength, 1, f );
    fclose( f );

    // process chunks (This does all the file parsing/reading)
    EatChunk( memfile );
    delete [] memfile;
}

void Model::loadPLY( std::string fname )
{
    FILE *f = fopen(fname.c_str(), "rb");
    if(f == NULL)
    {
        printf("Unable to open ply file %s\n",fname.c_str());
        assert( !"Unable to open ply file. Abort this execution." );
        return;
    }
    fseek (f , 0 , SEEK_END);
    unsigned long filelength = ftell (f);
    rewind (f);
    char *memfile = new (std::nothrow) char[filelength];
    fread( memfile, filelength, 1, f );
    fclose(f);

    std::string line;
    std::string tf = memfile;
    if(tf.substr(0,3) != "ply")
    {
        printf("File %s is not a PLY file.", fname.c_str());
        return;
    }

    // find vertex count
    unsigned int t, s;
    t = (unsigned int)tf.find("element",0);
    t += 7;
    t = (unsigned int)tf.find("vertex", t);
    t += 7;
    s = (unsigned int)tf.find('\n', t);
    m_uiVertexCount =  (unsigned int)atoi(tf.substr(t,s-t).c_str());

     // find triangle count
    t = (unsigned int)tf.find("element",s);
    t += 7;
    t = (unsigned int)tf.find("face",t);
    t += 5;
    s = (unsigned int)tf.find('\n',t);
    m_uiTriangleCount = (unsigned int)atoi(tf.substr(t,s-t).c_str());

    float x,y,z;
    unsigned int begin, end, s1,s2,s3;
    begin = (unsigned int)tf.find("end_header",t);

    for (unsigned int i = 0; i < m_uiVertexCount; i++)
    {
        begin = (unsigned int)tf.find('\n',begin)+1;
        end = (unsigned int)tf.find('\n', begin);
        s1 = (unsigned int)tf.find(' ', begin);
        s2 = (unsigned int)tf.find(' ', s1+1);
        x = (float)atof(tf.substr(begin, s1-begin).c_str());
        y = (float)atof(tf.substr(s1, s2-s1).c_str());
        z = (float)atof(tf.substr(s2, end-s2).c_str());

        // save data
        Vertex v;
        v.setCoordinates( Point3( x, y, z ) );
        m_Vertices.push_back( v );

        // calculate max and min point as we read them in from the file
        m_maxPoint.X() = max(m_maxPoint.X(), x);
        m_maxPoint.Y() = max(m_maxPoint.Y(), y);
        m_maxPoint.Z() = max(m_maxPoint.Z(), z);
        m_minPoint.X() = min(m_minPoint.X(), x);
        m_minPoint.Y() = min(m_minPoint.Y(), y);
        m_minPoint.Z() = min(m_minPoint.Z(), z);
    }

    for (unsigned int i = 0; i < m_uiTriangleCount; i++)
    {
        unsigned int v0, v1, v2;
        begin = (unsigned int)tf.find('\n',begin)+1;
        end = (unsigned int)tf.find('\n', begin);
        s1 = (unsigned int)tf.find(' ', begin);
        s2 = (unsigned int)tf.find(' ', s1+1);
        s3 = (unsigned int)tf.find(' ', s2+1);
        v0 = (unsigned int)atoi(tf.substr(s1, s2-s1).c_str());
        v1 = (unsigned int)atoi(tf.substr(s2, s3-s2).c_str());
        v2 = (unsigned int)atoi(tf.substr(s3, end-s3).c_str());

        Triangle t( m_Vertices[v0], m_Vertices[v1], m_Vertices[v2] );
        m_Triangles.push_back( t );
    }
    delete [] memfile;
}

void Model::loadVNT( std::string fname )
{
    FILE *f = fopen(fname.c_str(), "rb");
    if(f == NULL)
    {
        printf("Unable to open VNT file %s\n",fname.c_str());
        return;
    }
    fseek (f , 0 , SEEK_END);
    unsigned long filelength = ftell (f);
    rewind (f);
    char *memfile = new (std::nothrow) char[filelength];
    fread( memfile, filelength, 1, f );
    fclose(f);

    std::string line;
    std::string tf = memfile;

    bool hasMaterial;
    bool hasNormals;
    bool hasTexture;

    // find vertex count
    unsigned int t = 0, s;

    t = (unsigned int)tf.find("ELEMENT VERTICES", 0);
    if(t != std::string::npos)
    {
        t += 16;
        s = (unsigned int)tf.find('\n', t);
        m_uiVertexCount =  (unsigned int)atoi(tf.substr(t,s-t).c_str());
    }

    // find triangle count
    t = (unsigned int)tf.find("ELEMENT TRIANGLES",0);
    if(t != std::string::npos)
    {
        t += 17;
        s = (unsigned int)tf.find('\n',t);
        m_uiTriangleCount = (unsigned int)atoi(tf.substr(t,s-t).c_str());
    }

    t = (unsigned int)tf.find("ELEMENT MATERIAL",0);
    if(t != std::string::npos)
        hasMaterial = true;

    t = (unsigned int)tf.find("ELEMENT NORMAL",0);
    if(t != std::string::npos)
        hasNormals = true;

    t = (unsigned int)tf.find("ELEMENT TEXTURE",0);
    if(t != std::string::npos)
    {
        t += 15;
        s = (unsigned int)tf.find('\n',t);
        m_Material->SetTextureFilename(tf.substr(t,s-t));
        hasTexture = true;
    }

    t = (unsigned int)tf.find("END_ELEMENT",0);
    // process material information
    unsigned int begin,end,s1,s2,s3;
    if ( hasMaterial )
    {
        // write Material data into file
        begin = (unsigned int)tf.find("AMBIENT",t);
        if(begin != std::string::npos)
        {
            begin += 8;
            s1 = (unsigned int)tf.find(' ',begin)+1;
            s2 = (unsigned int)tf.find(' ',s1)+1;
            end = (unsigned int)tf.find('\n',s2);

            m_Material->SetAmbient( Color((float)atof(tf.substr(begin,s1-begin).c_str()),
                                         (float)atof(tf.substr(s1,s2-s1).c_str()),
                                         (float)atof(tf.substr(s2,begin-s2).c_str()),
                                         1) );
        }
        begin = (unsigned int)tf.find("DIFFUSE",t);
        if(begin != std::string::npos)
        {
            begin += 8;
            s1 = (unsigned int)tf.find(' ',begin)+1;
            s2 = (unsigned int)tf.find(' ',s1)+1;
            end = (unsigned int)tf.find('\n',s2);

            m_Material->SetDiffuse( Color((float)atof(tf.substr(begin,s1-begin).c_str()),
                                         (float)atof(tf.substr(s1,s2-s1).c_str()),
                                         (float)atof(tf.substr(s2,begin-s2).c_str()),
                                         1) );
        }
        begin = (unsigned int)tf.find("SPECULAR",t);
        if(begin != std::string::npos)
        {
            begin += 9;
            s1 = (unsigned int)tf.find(' ',begin)+1;
            s2 = (unsigned int)tf.find(' ',s1)+1;
            end = (unsigned int)tf.find('\n',s2);

            m_Material->SetSpecular( Color((float)atof(tf.substr(begin,s1-begin).c_str()),
                                          (float)atof(tf.substr(s1,s2-s1).c_str()),
                                          (float)atof(tf.substr(s2,begin-s2).c_str()),
                                          1) );
        }
        begin = (unsigned int)tf.find("REFLECTIVITY",t);
        if(begin != std::string::npos)
        {
            begin += 13;
            end = (unsigned int)tf.find('\n',begin);

            m_Material->SetReflectivity( (float)atof(tf.substr(begin,end-begin).c_str()) );
        }
        begin = (unsigned int)tf.find("TRANSPARENCY",t);
        if(begin != string::npos)
        {
            begin += 13;
            end = (unsigned int)tf.find('\n',begin);

            m_Material->SetTransparency( (float)atof(tf.substr(begin,end-begin).c_str()) );
        }
        begin = (unsigned int)tf.find("REFRACTIONINDEX",t);
        if(begin != std::string::npos)
        {
            begin += 16;
            end = (unsigned int)tf.find('\n',begin);

            m_Material->SetRefractionIndex( (float)atof(tf.substr(begin,end-begin).c_str()) );
        }
    }
    begin = (unsigned int)tf.find("END_MATERIAL",0);

    float x=0, y=0, z=0;
    float nx=0, ny=0, nz=0;
    float tx=0, ty=0;
    unsigned int s4,s5,s6,s7;
    for (unsigned int i = 0; i < m_uiVertexCount; i++)
    {
        begin = (unsigned int)tf.find('\n',begin)+1;
        end = (unsigned int)tf.find('\n', begin);
        s1 = (unsigned int)tf.find(' ', begin);
        s2 = (unsigned int)tf.find(' ', s1+1);
        x = (float)atof(tf.substr(begin, s1-begin).c_str());
        y = (float)atof(tf.substr(s1, s2-s1).c_str());
        if( m_bHasNormals)
        {
            s3 = (unsigned int)tf.find(' ', s2+1);
            s4 = (unsigned int)tf.find(' ', s3+1);
            s5 = (unsigned int)tf.find(' ', s4+1);
            z = (float)atof(tf.substr(s2, s3-s2).c_str());
            nx = (float)atof(tf.substr(s3, s4-s3).c_str());
            ny = (float)atof(tf.substr(s4, s5-s4).c_str());
            if( m_bHasTexture)
            {
                s6 = (unsigned int)tf.find(' ', s5+1);
                s7 = (unsigned int)tf.find(' ', s6+1);
                nz = (float)atof(tf.substr(s5, s6-s5).c_str());
                tx = (float)atof(tf.substr(s6, s7-s6).c_str());
                ty = (float)atof(tf.substr(s7, end-s7).c_str());
            }
            else
            {
                nz = (float)atof(tf.substr(s5, end-s5).c_str());
            }
        }

        if( m_bHasTexture )
        {
            s3 = (unsigned int)tf.find(' ', s2+1);
            s4 = (unsigned int)tf.find(' ', s3+1);
            z = (float)atof(tf.substr(s2, s3-s2).c_str());
            tx = (float)atof(tf.substr(s3, s4-s3).c_str());
            ty = (float)atof(tf.substr(s4, end-s4).c_str());
        }
        else
        {
            z = (float)atof(tf.substr(s2, end-s2).c_str());
        }

        Vertex v( x, y, z, nx, ny, nz );
        v.setTexCoord( tx, ty );
        m_Vertices.push_back( v );

        m_maxPoint.X() = max(m_maxPoint.X(), x);
        m_maxPoint.Y() = max(m_maxPoint.Y(), y);
        m_maxPoint.Z() = max(m_maxPoint.Z(), z);
        m_minPoint.X() = min(m_minPoint.X(), x);
        m_minPoint.Y() = min(m_minPoint.Y(), y);
        m_minPoint.Z() = min(m_minPoint.Z(), z);
    }

    unsigned int v0, v1, v2;
    for (unsigned int i = 0; i < m_uiTriangleCount; i++)
    {
        begin = (unsigned int)tf.find('\n',begin)+1;
        end = (unsigned int)tf.find('\n', begin);
        s1 = (unsigned int)tf.find(' ', begin);
        s2 = (unsigned int)tf.find(' ', s1+1);
        //cout << tf.substr(begin,end-begin) << endl;
        v0 = (unsigned int)atoi(tf.substr(begin, s1-begin).c_str());
        v1 = (unsigned int)atoi(tf.substr(s1, s2-s1).c_str());
        v2 = (unsigned int)atoi(tf.substr(s2, end-s2).c_str());

        Triangle t( m_Vertices[v0], m_Vertices[v1], m_Vertices[v2] );
        m_Triangles.push_back( t );
    }
    delete [] memfile;
}

void Model::loadOBJ(const std::string &fname)
{
    Objloader obj(fname);
    // add all the vertices
    float x,y,z;
    std::deque<Vector3>::const_iterator cfi; // const float iterator
    std::deque<Vector3>::const_iterator nfi; // const float iterator
    nfi = obj.getNormal().begin();
    
    Vertex tv;
    // process vertex
    for(cfi = obj.getVertex().begin(); cfi != obj.getVertex().end(); cfi++)
    {
        x = (*cfi).X(); y = (*cfi)[1]; z = (*cfi)[2];
        tv.setCoordinates(Point3(x,y,z));
        //*p++ = x; *p++ = y; *p++ = z;
        m_maxPoint[0] = max(m_maxPoint[0], x);
        m_maxPoint[1] = max(m_maxPoint[1], y);
        m_maxPoint[2] = max(m_maxPoint[2], z);
        m_minPoint[0] = min(m_minPoint[0], x);
        m_minPoint[1] = min(m_minPoint[1], y);
        m_minPoint[2] = min(m_minPoint[2], z);
        if((this->m_bHasNormals = obj.hasNormal()) == true)
        {
            tv.setNormal((*nfi));
            nfi++;
        }
        m_Vertices.push_back(tv);
    }
    
    /******* texture are ignored ****************/
    /*
    deque<Vector3>::const_iterator cfi; // const float iterator
    // texture coordinates
    if((m_bHasTexture = obj.hasTexCoord()) == true)
    {
        for(cfi = obj.getTexCoord().begin(); cfi != obj.getTexCoord().end(); cfi++)
        {
            m_Texcoords.push_back(Vector3((*cfi)[0],(*cfi)[1],(*cfi)[2]));
            //*tex++ = (*cfi)[0]; *tex++ = (*cfi)[1]; *tex++ = (*cfi)[2];
        }
    }
    */
    
    // indexes
    Triangle tt;
    std::deque<TFace>::const_iterator cti; // constant triangle iterator
    for(cti = obj.getIndex().begin(); cti != obj.getIndex().end(); cti++)
    {
        tt.setVertex0(m_Vertices[(*cti).vIndex[0]]);
        tt.setVertex1(m_Vertices[(*cti).vIndex[1]]);
        tt.setVertex2(m_Vertices[(*cti).vIndex[2]]);
        m_Triangles.push_back(tt);
    }
  //  printf("size is: %i", m_Triangles.size());

}


