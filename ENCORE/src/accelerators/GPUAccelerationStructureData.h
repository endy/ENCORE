#pragma once


#include "GLEW/glew.h"

#include <string>
#include <cmath>
#include <cstring>

static const int TEXTURE_SIZE_LIMIT = 4096;
static const float INVERSE_TEXTURE_SIZE_LIMIT = 0.000244140625; // 1/4096

/////// MACRO ///////
#define GET_GLERROR(ret)                                          \
{                                                                 \
    GLenum err = glGetError();                                    \
    if (err != GL_NO_ERROR) {                                     \
    fprintf(stderr, "[%s line %d] GL Error: %s\n",                \
    __FILE__, __LINE__, gluErrorString(err));                     \
    fflush(stderr);                                               \
    }                                                             \
}

inline int makeMod4096(int aSize)
{
	int bestSize = aSize;
	if (aSize > 4096){
		bestSize = (int)ceil(aSize  * INVERSE_TEXTURE_SIZE_LIMIT)*4096;
	}
	return bestSize;
}

/**************
* GPUAccelerationStructureData class
**************/
class GPUAccelerationStructureData
{
public:
    GPUAccelerationStructureData();
    ~GPUAccelerationStructureData();

    void generateTextures( void );

    void setTraversalShaderFilename( std::string l_strFilename ) { m_strTraversalShaderFilename = l_strFilename; }

    void setCellData0Count( unsigned int cellCount )
    { 
        if ( m_pCellData0 ) { delete [] m_pCellData0; m_pCellData0 = NULL; }

        m_CellData0Count = cellCount;
        m_pCellData0 = new (std::nothrow) float[makeMod4096(cellCount)*4];
        memset( m_pCellData0, 0, makeMod4096(cellCount)*4*sizeof(float) );
    }
    void setCellData1Count( unsigned int cellCount )
    {
        if ( m_pCellData1 ) { delete [] m_pCellData1; }

        m_CellData1Count = cellCount;
        m_pCellData1 = new (std::nothrow) float[makeMod4096(cellCount)*4]; 
        memset( m_pCellData0, 0, makeMod4096(cellCount)*4*sizeof(float) );
    }
    void setVertexCount( unsigned int vertexCount )
    {
        vertexCount /= 3;
        if ( m_pVertex0Array ) { delete [] m_pVertex0Array; }
        if ( m_pVertex1Array ) { delete [] m_pVertex1Array; }
        if ( m_pVertex2Array ) { delete [] m_pVertex2Array; }

        m_VertexCount = vertexCount;
        m_pVertex0Array = new (std::nothrow) float[makeMod4096(vertexCount)*4];
        m_pVertex1Array = new (std::nothrow) float[makeMod4096(vertexCount)*4];
        m_pVertex2Array = new (std::nothrow) float[makeMod4096(vertexCount)*4];
        memset( m_pVertex0Array, 0, makeMod4096(vertexCount)*4*sizeof(float) );
        memset( m_pVertex1Array, 0, makeMod4096(vertexCount)*4*sizeof(float) );
        memset( m_pVertex2Array, 0, makeMod4096(vertexCount)*4*sizeof(float) );
    }
    void setNormalCount( unsigned int normalCount ) 
    {
        normalCount /= 3;
        if ( m_pNormal0Array ) { delete [] m_pNormal0Array; }
        if ( m_pNormal1Array ) { delete [] m_pNormal1Array; }
        if ( m_pNormal2Array ) { delete [] m_pNormal2Array; }

        m_NormalCount = normalCount;
        m_pNormal0Array = new (std::nothrow) float[makeMod4096(normalCount)*4];
        m_pNormal1Array = new (std::nothrow) float[makeMod4096(normalCount)*4];
        m_pNormal2Array = new (std::nothrow) float[makeMod4096(normalCount)*4];
        memset( m_pNormal0Array, 0, makeMod4096(normalCount)*4*sizeof(float) );
        memset( m_pNormal1Array, 0, makeMod4096(normalCount)*4*sizeof(float) );
        memset( m_pNormal2Array, 0, makeMod4096(normalCount)*4*sizeof(float) );
    }
    void setTexCoordCount( unsigned int texCoordCount ) 
    {
        texCoordCount /= 2;
        if ( m_pTexCoord0Array ) { delete [] m_pTexCoord0Array; }
        if ( m_pTexCoord1Array ) { delete [] m_pTexCoord1Array; }

        m_TexCoordCount = texCoordCount;
        m_pTexCoord0Array = new (std::nothrow) float[makeMod4096(texCoordCount)*4];
        m_pTexCoord1Array = new (std::nothrow) float[makeMod4096(texCoordCount)*4];
        memset( m_pTexCoord0Array, 0, makeMod4096(texCoordCount)*4*sizeof(float) );
        memset( m_pTexCoord1Array, 0, makeMod4096(texCoordCount)*4*sizeof(float) );
    }
    void setMaterialCount( unsigned int materialCount )
    {
        if ( m_pMaterial0Array ) { delete [] m_pMaterial0Array; }

        m_MaterialCount = materialCount;
        m_pMaterial0Array = new (std::nothrow) float[makeMod4096(materialCount)*4];
        memset( m_pMaterial0Array, 0, makeMod4096(materialCount)*4*sizeof(float) );
    }

public: // member data

    const static int m_NUM_OF_CELL_TEXTURES = 2;
    const static int m_NUM_OF_VERTEX_TEXTURES = 3;
    const static int m_NUM_OF_NORMAL_TEXTURES = 3;
    const static int m_NUM_OF_TEXCOORD_TEXTURES = 2;
    const static int m_NUM_OF_MATERIAL_TEXTURES = 1;

    bool textureGen;

    GLuint m_CellTexture[m_NUM_OF_CELL_TEXTURES];         // tex id for grid cell textures
    GLuint m_VertexTexture[m_NUM_OF_VERTEX_TEXTURES];     // tex id for vertex
    GLuint m_NormalTexture[m_NUM_OF_NORMAL_TEXTURES];     // tex id for normal per vertex
    GLuint m_TexCoordTexture[m_NUM_OF_TEXCOORD_TEXTURES]; // tex id for texture coordinates
    GLuint m_MaterialTexture[m_NUM_OF_MATERIAL_TEXTURES]; // tex id for triangle materials

    std::string m_strTraversalShaderFilename;

    void SaveTextureData( void );
    void SaveTextureData( std::string filename, GLuint textureID );

public: // member data
    float* m_pCellData0; // assumed to be 4 values per cell
    float* m_pCellData1; // assumed to be 4 values per cell
    float* m_pVertex0Array;     // assumed to be 3 values per vertex
    float* m_pVertex1Array;     // assumed to be 3 values per vertex
    float* m_pVertex2Array;     // assumed to be 3 values per vertex
    float* m_pNormal0Array;     // assumed to be 3 values per normal
    float* m_pNormal1Array;     // assumed to be 3 values per normal
    float* m_pNormal2Array;     // assumed to be 3 values per normal
    float* m_pTexCoord0Array;   // assumed to be 3 values per texcoord
    float* m_pTexCoord1Array;   // assumed to be 3 values per texcoord
    float* m_pMaterial0Array;    // assumed to be 4 values per Materal

private:
    unsigned int m_CellData0Count;
    unsigned int m_CellData1Count;
    unsigned int m_VertexCount;
    unsigned int m_NormalCount;
    unsigned int m_TexCoordCount;
    unsigned int m_MaterialCount;

};
