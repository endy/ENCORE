#include "GPUAccelerationStructureData.h"

#include <cstdlib>
#include <fstream>

GPUAccelerationStructureData::GPUAccelerationStructureData(void)
    :  m_pCellData0(NULL),  m_pCellData1(NULL),
       m_pVertex0Array(NULL),  m_pVertex1Array(NULL),  m_pVertex2Array(NULL),
       m_pNormal0Array(NULL),  m_pNormal1Array(NULL),  m_pNormal2Array(NULL),
       m_pTexCoord0Array(NULL),  m_pTexCoord1Array(NULL),
       m_pMaterial0Array(NULL), m_MaterialCount(0),
       m_CellData0Count(0), m_CellData1Count(0), 
       m_VertexCount(0), m_NormalCount(0), m_TexCoordCount(0),
       m_strTraversalShaderFilename("")
{
    textureGen = false;
}

GPUAccelerationStructureData::~GPUAccelerationStructureData( void )
{
    if ( m_pCellData0 ) { delete [] m_pCellData0; m_pCellData0 = NULL; }
    if ( m_pCellData1 ) { delete [] m_pCellData1; m_pCellData1 = NULL; }
    if ( m_pVertex0Array ) { delete []  m_pVertex0Array; m_pVertex0Array = NULL; }
    if ( m_pVertex1Array ) { delete []  m_pVertex1Array; m_pVertex1Array = NULL; }
    if ( m_pVertex2Array ) { delete []  m_pVertex2Array; m_pVertex2Array = NULL; }
    if ( m_pNormal0Array ) { delete []  m_pNormal0Array; m_pNormal0Array = NULL; }
    if ( m_pNormal1Array ) { delete []  m_pNormal1Array; m_pNormal1Array = NULL; }
    if ( m_pNormal2Array ) { delete []  m_pNormal2Array; m_pNormal2Array = NULL; }
    if ( m_pTexCoord0Array ) { delete []  m_pTexCoord0Array; m_pTexCoord0Array = NULL; }
    if ( m_pTexCoord1Array ) { delete []  m_pTexCoord1Array; m_pTexCoord1Array = NULL; }
    if ( m_pMaterial0Array ) { delete []  m_pMaterial0Array; m_pMaterial0Array = NULL; }
}

/*******************
* generateTextures 
*******************/
void GPUAccelerationStructureData::generateTextures( void )
{
    GLenum texInternalFormat = GL_RGBA32F_ARB;
    GLenum texTarget = GL_TEXTURE_RECTANGLE_ARB;
    GLenum filterMode = GL_NEAREST;
    
    if(!textureGen)
    {
        glGenTextures(m_NUM_OF_VERTEX_TEXTURES, &m_VertexTexture[0]);
        glGenTextures(m_NUM_OF_NORMAL_TEXTURES, &m_NormalTexture[0]);
        glGenTextures(m_NUM_OF_TEXCOORD_TEXTURES, &m_TexCoordTexture[0]);
        glGenTextures(m_NUM_OF_CELL_TEXTURES, &m_CellTexture[0]);
        glGenTextures(m_NUM_OF_MATERIAL_TEXTURES, &m_MaterialTexture[0]);
        textureGen = true;
    }

    // vertices
    int textureWidth = (m_VertexCount > TEXTURE_SIZE_LIMIT) ? TEXTURE_SIZE_LIMIT : m_VertexCount;
    int textureHeight = (int)ceil(m_VertexCount * INVERSE_TEXTURE_SIZE_LIMIT);

    

    glBindTexture(texTarget, m_VertexTexture[0]);
    glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, filterMode);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // specify that data should be unpacked 1 value at a time
    glTexImage2D(texTarget, 0, texInternalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_pVertex0Array);

    glBindTexture(texTarget, m_VertexTexture[1]);
    glTexParameteri(texTarget,GL_TEXTURE_MIN_FILTER,filterMode);
    glTexParameteri(texTarget,GL_TEXTURE_MAG_FILTER,filterMode);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // specify that data should be unpacked 1 value at a time
    glTexImage2D(texTarget, 0, texInternalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_pVertex1Array);

    glBindTexture(texTarget, m_VertexTexture[2]);
    glTexParameteri(texTarget,GL_TEXTURE_MIN_FILTER,filterMode);
    glTexParameteri(texTarget,GL_TEXTURE_MAG_FILTER,filterMode);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // specify that data should be unpacked 1 value at a time
    glTexImage2D(texTarget, 0, texInternalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_pVertex2Array);

    // check for any errors
    GET_GLERROR(0);

    //normals
    textureWidth = (m_NormalCount > TEXTURE_SIZE_LIMIT) ? TEXTURE_SIZE_LIMIT : m_NormalCount;
    textureHeight = (int)ceil(m_NormalCount * INVERSE_TEXTURE_SIZE_LIMIT);

    

    glBindTexture(texTarget, m_NormalTexture[0]);
    glTexParameteri(texTarget,GL_TEXTURE_MIN_FILTER,filterMode);
    glTexParameteri(texTarget,GL_TEXTURE_MAG_FILTER,filterMode);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // specify that data should be unpacked 1 value at a time
    glTexImage2D(texTarget, 0, texInternalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_pNormal0Array);

    glBindTexture(texTarget, m_NormalTexture[1]);
    glTexParameteri(texTarget,GL_TEXTURE_MIN_FILTER,filterMode);
    glTexParameteri(texTarget,GL_TEXTURE_MAG_FILTER,filterMode);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // specify that data should be unpacked 1 value at a time
    glTexImage2D(texTarget, 0, texInternalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_pNormal1Array);

    glBindTexture(texTarget, m_NormalTexture[2]);
    glTexParameteri(texTarget,GL_TEXTURE_MIN_FILTER,filterMode);
    glTexParameteri(texTarget,GL_TEXTURE_MAG_FILTER,filterMode);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // specify that data should be unpacked 1 value at a time
    glTexImage2D(texTarget, 0, texInternalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_pNormal2Array);

    // check for any errors
    GET_GLERROR(0);

    // texcoords
    textureWidth = (m_TexCoordCount > TEXTURE_SIZE_LIMIT) ? TEXTURE_SIZE_LIMIT : m_TexCoordCount;
    textureHeight = (int)ceil(m_TexCoordCount * INVERSE_TEXTURE_SIZE_LIMIT);

    

    glBindTexture(texTarget, m_TexCoordTexture[0]);
    glTexParameteri(texTarget,GL_TEXTURE_MIN_FILTER,filterMode);
    glTexParameteri(texTarget,GL_TEXTURE_MAG_FILTER,filterMode);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // specify that data should be unpacked 1 value at a time
    glTexImage2D(texTarget, 0, texInternalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_pTexCoord0Array);

    glBindTexture(texTarget, m_TexCoordTexture[1]);
    glTexParameteri(texTarget,GL_TEXTURE_MIN_FILTER,filterMode);
    glTexParameteri(texTarget,GL_TEXTURE_MAG_FILTER,filterMode);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // specify that data should be unpacked 1 value at a time
    glTexImage2D(texTarget, 0, texInternalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_pTexCoord1Array);

    // check for any errors
    GET_GLERROR(0);

    // cellData
    

    textureWidth = (m_CellData0Count > TEXTURE_SIZE_LIMIT) ? TEXTURE_SIZE_LIMIT : m_CellData0Count;
    textureHeight = (int)ceil(m_CellData0Count * INVERSE_TEXTURE_SIZE_LIMIT);

    glBindTexture(texTarget, m_CellTexture[0]);
    glTexParameteri(texTarget,GL_TEXTURE_MIN_FILTER,filterMode);
    glTexParameteri(texTarget,GL_TEXTURE_MAG_FILTER,filterMode);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // specify that data should be unpacked 1 value at a time
    glTexImage2D(texTarget, 0, texInternalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_pCellData0);

    textureWidth = (m_CellData1Count > TEXTURE_SIZE_LIMIT) ? TEXTURE_SIZE_LIMIT : m_CellData1Count;
    textureHeight = (int)ceil(m_CellData1Count * INVERSE_TEXTURE_SIZE_LIMIT);

    glBindTexture(texTarget, m_CellTexture[1]);
    glTexParameteri(texTarget,GL_TEXTURE_MIN_FILTER,filterMode);
    glTexParameteri(texTarget,GL_TEXTURE_MAG_FILTER,filterMode);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // specify that data should be unpacked 1 value at a time
    glTexImage2D(texTarget, 0, texInternalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_pCellData1);

    GET_GLERROR(0);

    // Materials
    

    textureWidth = (m_MaterialCount > TEXTURE_SIZE_LIMIT) ? TEXTURE_SIZE_LIMIT : m_MaterialCount;
    textureHeight = (int)ceil(m_MaterialCount * INVERSE_TEXTURE_SIZE_LIMIT);

    glBindTexture(texTarget, m_MaterialTexture[0]);
    glTexParameteri(texTarget,GL_TEXTURE_MIN_FILTER,filterMode);
    glTexParameteri(texTarget,GL_TEXTURE_MAG_FILTER,filterMode);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // specify that data should be unpacked 1 value at a time
    glTexImage2D(texTarget, 0, texInternalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, m_pMaterial0Array);

    GET_GLERROR(0);
}


void GPUAccelerationStructureData::SaveTextureData( void )
{
    printf("Saving all traversal textures to file, please wait.");

    SaveTextureData( "GPUASD_Cell0.txt", m_CellTexture[0] );
    printf(".");
    SaveTextureData( "GPUASD_v0.txt", m_VertexTexture[0] );
    printf(".");
    SaveTextureData( "GPUASD_v1.txt", m_VertexTexture[1] );
    printf(".");
    SaveTextureData( "GPUASD_v2.txt", m_VertexTexture[2] );

    printf("Done saving texture values.\n");       
} 

void GPUAccelerationStructureData::SaveTextureData( std::string filename, GLuint textureID )
{
    GLenum texInternalFormat = GL_RGBA32F_ARB;
    GLenum texTarget = GL_TEXTURE_RECTANGLE_ARB;
    GLenum filterMode = GL_NEAREST;

//    printf("Saving traversal textures to file, please wait.");
    GLint twidth;
    GLint theight;
    GLint level;

    glBindTexture(texTarget, textureID);

    std::ofstream myfile;
    myfile.open ( filename.c_str() );
    // get base mipmap level, texture width and height
    glGetTexParameteriv(texTarget, GL_TEXTURE_BASE_LEVEL, &level);
    glGetTexLevelParameteriv(texTarget, level, GL_TEXTURE_WIDTH, &twidth);
    glGetTexLevelParameteriv(texTarget, level, GL_TEXTURE_HEIGHT, &theight);
    // determine how much space we need in memory for the texture
    int size = twidth*theight*4;
    GLfloat* pixels = new GLfloat[size];//(GLfloat*) malloc(size*sizeof(GLfloat));
    memset( pixels, -2, size );
    // initialize that space to a value we know

    // get texture information
    glGetTexImage(texTarget, level, GL_RGBA, GL_FLOAT, pixels);
    //GET_GLERROR(0);

    // print it out
    GLfloat x,y,z;
    GLfloat w;
    int r,c;
    for (int i = 0; i<size/4; i++)
    {
        x = pixels[i*4];
        y = pixels[i*4+1];
        z = pixels[i*4+2];
        w = pixels[i*4+3];

        r = i%theight;
        c = i/twidth;

        myfile << r << " " << c << "\tx " << x << "\ty " 
        << y << "\tz " << z << "\tw " << w;
        myfile << std::endl;
    }
    
    myfile.close();
    // clean up
    delete [] pixels;
//    printf("Done saving texture values.\n");       
} 
