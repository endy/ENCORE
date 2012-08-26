#include "UniformGrid.h"

#include <iostream>

#include "Common.h"
#include "Pixel.h"

#include "Model.h"

float sqr(float a){ return a*a;}

UniformGrid::UniformGrid(void): 
    m_bHaveModels(false), 
    sizeset(false), 
    m_cellSize(1), 
    m_Grid(NULL) 
{ 
#ifdef NEW
    m_Grid = 0;
    gdata = 0;
    num_Grid = NULL;
#endif
    m_Grid = 0;
    m_gridDimension = 1;
    //m_tempPrim = new TrianglePrim( Vertex(), Vertex(), Vertex()); 
}


UniformGrid::~UniformGrid(void)
{
    /*
    if ( m_tempPrim )
    {
        delete m_tempPrim;
    }
    */
#ifdef NEW
    delete [] gdata;
    gdata = 0;
    if(m_Grid)
    {
        for(int x =0; x < m_gridDimension; x++)
        {
            for(int y = 0; y < m_gridDimension; y++)
            {
                delete [] m_Grid[x][y];
                delete [] num_Grid[x][y];
            }
            delete [] m_Grid[x];
            delete [] num_Grid[x];
        }
        delete m_Grid;
        delete num_Grid;
        m_Grid = 0;
        num_Grid = 0;
    }
#else
    if ( m_Grid )
    {
        for (int x=0; x < m_gridDimension; x++)
        {
            for (int y=0; y < m_gridDimension; y++)
            {
                for (int z=0; z < m_gridDimension; z++)
                {
                    m_Grid[x][y][z]->clear();
                    delete m_Grid[x][y][z];
                }
                delete [] m_Grid[x][y];
            }
            delete [] m_Grid[x];
        }
        delete [] m_Grid;
        m_Grid = NULL;
    }
#endif
}


void UniformGrid::usage( void ) 
{
    printf("UniformGrid Configuration Options\n");
    printf("\tGRID_SIZE\n");
}

void UniformGrid::configure( Options* l_pOptions )
{
    int gridsize = 0;
    if ( l_pOptions->doesOptionExist( "GRID_SIZE" ) )
    {
        gridsize = atoi( l_pOptions->getOption( "GRID_SIZE" ).c_str() );
    }
    
    setGridDimension( gridsize );
}

void UniformGrid::setOptimalGridDimension( void )
{
    // if grid size is set to zero
    // use triangle count to pick a good grid size
    if ( m_gridDimension == 0 && m_num_Diff_Prims > 0 )
    {
        // want to average 1 triangles per voxel
        float totalDesiredVoxels = m_num_Diff_Prims / 1.0f;
        int desiredGridSize = (int)ceil( pow( totalDesiredVoxels, 1.0f/3.0f ) );
        setGridDimension( desiredGridSize );
    }
}

void UniformGrid::build(std::list<IModel*> &modelList)
{
    DWORD dwBuildTime = EncoreGetTime();

    setModels(modelList);
    setOptimalGridDimension();
    buildGrid(modelList);

    dwBuildTime = EncoreGetTime() - dwBuildTime;
}

#ifdef WIN32
/************************
* void buildGPU( Scene& l_pScene, GPUAccelerationStructureData& l_pASD )
*
*
*************************/
void UniformGrid::buildGPU(std::list<IModel*> &modelList, std::list<Triangle*> &triangleList, GPUAccelerationStructureData& l_pASD )
{
    build(modelList);
    //setModels(modelList);
    //setOptimalGridDimension();
    //buildGrid(modelList);

    l_pASD.setTraversalShaderFilename( "shaders\\UniformGrid.cg" );

    l_pASD.setVertexCount( m_num_Rep_Prims*3 ); // each triangle has 3 vertices
    l_pASD.setNormalCount( m_num_Rep_Prims*3 ); // each triangle has 3 normals
    l_pASD.setMaterialCount( m_num_Rep_Prims ); // each triangle has 1 material

    l_pASD.setCellData0Count( m_gridDimension*m_gridDimension*m_gridDimension ); // x = index of first tri, y = #ofTris, z = 0

    // store the triangle data using pointers into the current lists
    // this is faster than using indices
    float* pv0 = l_pASD.m_pVertex0Array;
    float* pv1 = l_pASD.m_pVertex1Array;
    float* pv2 = l_pASD.m_pVertex2Array;
    float* pn0 = l_pASD.m_pNormal0Array;
    float* pn1 = l_pASD.m_pNormal1Array;
    float* pn2 = l_pASD.m_pNormal2Array;
    float* pm0 = l_pASD.m_pMaterial0Array;
    float* gi0 = l_pASD.m_pCellData0;

    Vertex v;
    Material m;
    float GPUCellIndex = 0.0f;

    std::list<IPrimitive*>::iterator primIter;
    std::list<Triangle*>::iterator itp;

    // for every cell in the uniform grid
    for (int z = 0; z < m_gridDimension; z++ )
    {
        for (int y = 0; y < m_gridDimension; y++ )
        {
            for(int x = 0; x < m_gridDimension; x++ )
            {
#ifndef NEW
                std::list<IPrimitive*>* pPrims = m_Grid[x][y][z];
#endif
                float triCount = 0.0f;

                // store index to first cell
                *gi0 = GPUCellIndex; gi0++;
#ifdef NEW
                unsigned int mybase = m_Grid[x][y][z];
                for(unsigned short i = 0; i < num_Grid[x][y][z]; i++)
                {
                    std::list<Triangle*>* pTris = gdata[mybase+i]->getNewTesselation();
#else
                for ( primIter = m_Grid[x][y][z]->begin(); primIter != m_Grid[x][y][z]->end(); primIter++ )
                {
                    std::list<Triangle*>* pTris = (*primIter)->getNewTesselation();
#endif

                    for ( itp = pTris->begin(); itp != pTris->end(); itp++ )
                    {
                        // increment cell index
                        GPUCellIndex++;

                        // increment the number of triangles within this cell
                        triCount++;

                        // Material
                        m = *((*itp)->getMaterial());

                        *pm0 = m.GetDiffuse().R();            pm0++;
                        *pm0 = m.GetDiffuse().G();            pm0++;
                        *pm0 = m.GetDiffuse().B();            pm0++;
                        *pm0 = m.GetSpecularExponent();       pm0++;

                        // vertex 0
                        v = *((*itp)->getVertex0());

                        *pv0 = v.getCoordinates().X();    pv0++;
                        *pv0 = v.getCoordinates().Y();    pv0++;
                        *pv0 = v.getCoordinates().Z();    pv0++;
                        *pv0 = 0.0f;    pv0++;

                        *pn0 = v.getNormal().X();     pn0++;
                        *pn0 = v.getNormal().Y();     pn0++;
                        *pn0 = v.getNormal().Z();     pn0++;
                        *pn0 = 0.0f;     pn0++;

                        // vertex 1
                        v = *((*itp)->getVertex1());

                        *pv1 = v.getCoordinates().X();    pv1++;
                        *pv1 = v.getCoordinates().Y();    pv1++;
                        *pv1 = v.getCoordinates().Z();    pv1++;
                        *pv1 = 0.0f;    pv1++;

                        *pn1 = v.getNormal().X();     pn1++;
                        *pn1 = v.getNormal().Y();     pn1++;
                        *pn1 = v.getNormal().Z();     pn1++;
                        *pn1 = 0.0f;     pn1++;

                        // vertex 2
                        v = *((*itp)->getVertex2());

                        *pv2 = v.getCoordinates().X();    pv2++;
                        *pv2 = v.getCoordinates().Y();    pv2++;
                        *pv2 = v.getCoordinates().Z();    pv2++;
                        *pv2 = 0.0f;    pv2++;

                        *pn2 = v.getNormal().X();     pn2++;
                        *pn2 = v.getNormal().Y();     pn2++; 
                        *pn2 = v.getNormal().Z();     pn2++;
                        *pn2 = 0.0f;     pn2++;

                        // cleanup New Triangles from getNewTesselation()
                        delete *itp;
                        *itp = NULL;
                    } // triangles

                    // cleanup after getNewTesselation()
                    pTris->clear();
                    if ( pTris ) { delete pTris; }
                    pTris = NULL;

                } // primitives

                // store number of triangles in this cell
                *gi0 = triCount; gi0++;
                *gi0 = 0.0f;     gi0++;
                *gi0 = 0.0f;     gi0++;

                triCount = 0;

            } // X
        } // Y
    } // Z
}

/************************
* void setGPUParameters( CShader& l_Shader, GPUAccelerationStructureData& l_ASD )
*
* set shader parameters for Uniform Grid
*************************/
void UniformGrid::setGPUParameters( CShader& l_Shader, GPUAccelerationStructureData& l_ASD )
{
    CGparameter& cell_info = l_Shader.GetNamedParameter("cellData0", true);
    cgGLSetTextureParameter(cell_info, l_ASD.m_CellTexture[0]);

    CGparameter& v0 = l_Shader.GetNamedParameter("v0t", true);
    cgGLSetTextureParameter(v0, l_ASD.m_VertexTexture[0]);

    CGparameter& v1 = l_Shader.GetNamedParameter("v1t", true);
    cgGLSetTextureParameter(v1, l_ASD.m_VertexTexture[1]);

    CGparameter& v2 = l_Shader.GetNamedParameter("v2t", true);
    cgGLSetTextureParameter(v2, l_ASD.m_VertexTexture[2]);

    CGparameter& length = l_Shader.GetNamedParameter("len");
    cgGLSetParameter1f( length, m_cellSize );

    CGparameter& gridSize = l_Shader.GetNamedParameter("gridsize");
    cgGLSetParameter1f( gridSize, (float) m_gridDimension );

    CGparameter& minPoint = l_Shader.GetNamedParameter("gmin");
    cgGLSetParameter3f( minPoint, m_MinPt.X(), m_MinPt.Y(), m_MinPt.Z() );

    CGparameter& maxPoint = l_Shader.GetNamedParameter("gmax");
    cgGLSetParameter3f( maxPoint, m_MaxPt.X(), m_MaxPt.Y(), m_MaxPt.Z() );

    CGparameter& maxloop = l_Shader.GetNamedParameter("maxloop");
    cgGLSetParameter1f( maxloop, 1000 );
}

#endif

/*******
 UniformGrid::SetModels
********
* load the models into the Uniform Grid
* find the min and max coords of all the models
* Written by: Peter Lohrmann
*******/
void UniformGrid::setModels(std::list<IModel*> &modelList)
{ 
    m_bHaveModels = true;

    m_num_Diff_Prims = 0;
    m_MaxPt = Point3( NEG_INF, NEG_INF, NEG_INF );
    m_MinPt = Point3( POS_INF, POS_INF, POS_INF );

    std::list< IModel* >::iterator model;

    for( model = modelList.begin(); model != modelList.end(); model++ )
    {
        m_num_Diff_Prims += (*model)->GetPrimitiveCount();

        // update uniform grids min and max values;
        // TODO: why did I need to add/subtract the epsilon so the grid should not segfault? (look at buildGrid()) -- Brandon
        m_MinPt.X() = min((*model)->MinPoint().X(), m_MinPt.X()) - 0.0001f;
        m_MinPt.Y() = min((*model)->MinPoint().Y(), m_MinPt.Y()) - 0.0001f;
        m_MinPt.Z() = min((*model)->MinPoint().Z(), m_MinPt.Z()) - 0.0001f;

        m_MaxPt.X() = max((*model)->MaxPoint().X(), m_MaxPt.X()) + 0.0001f;
        m_MaxPt.Y() = max((*model)->MaxPoint().Y(), m_MaxPt.Y()) + 0.0001f;
        m_MaxPt.Z() = max((*model)->MaxPoint().Z(), m_MaxPt.Z()) + 0.0001f;
    }

    float trueMin = min( m_MinPt.X(), min( m_MinPt.Y(),m_MinPt.Z() ) );
    float trueMax = max( m_MaxPt.X(), max( m_MaxPt.Y(),m_MaxPt.Z() ) );
    m_MinPt = Point3( trueMin, trueMin, trueMin );
    m_MaxPt = Point3( trueMax, trueMax, trueMax );
}

/*************************
* UniformGrid::buildGrid
**************************
* put the correct triangles into
* proper cells of the grid. 
* Based on code by: 
* Revised for these data structures by: Peter Lohrmann
* Modifications made by: Chen-hao Chang
*************************/

void UniformGrid::buildGrid(std::list<IModel*> &modelList)
{
    if ( m_bHaveModels == false )
    {
        std::cout << "ERROR: Uniform Grid does not have Models to get data from." << std::endl;
        exit(0);
    }

    // calculate cell width, height and depth
    float temp = 1.0f/m_gridDimension;

    Vector3 delta = Vector3(m_MinPt, m_MaxPt) * temp;
    this->m_cellSize = delta.X();

    Vector3 dReci(1.0f/delta.X(), 1.0f/delta.Y(), 1.0f/delta.Z());

#ifdef NEW
    if( gdata )
    {
        delete [] gdata;
        gdata = 0;
        for(int x = 0; x < m_gridDimension; x++)
        {
                for(int y = 0; y < m_gridDimension; y++)
                {
                    for(int z = 0; z < m_gridDimension; z++)
                    {
                        m_Grid[x][y][z] = 0;
                        num_Grid[x][y][z] = 0;
                    }
                }
        }
    }
    else
    {
        num_Grid = new unsigned short**[m_gridDimension];
        m_Grid = new unsigned int**[m_gridDimension];
        for (int x=0; x < m_gridDimension; x++)
        {
            num_Grid[x] = new unsigned short*[m_gridDimension];
            m_Grid[x] = new unsigned int*[m_gridDimension];
            for (int y=0; y < m_gridDimension; y++)
            {
                num_Grid[x][y] = (unsigned short*)calloc(m_gridDimension, sizeof(unsigned short));//new unsigned short[m_gridDimension];
                m_Grid[x][y] = new unsigned int[m_gridDimension];
            }
        }
    }

    // building
    // intialize number of repeated Primitives
    m_num_Rep_Prims = 0;
    // record max space needed for this build
    unsigned int globalsize = 0;

    // for every Primitive of every model
    std::list< IModel* >::iterator model;

    // gather size information
    for( model = modelList.begin(); model != modelList.end(); model++ )
    {
        std::list< IPrimitive* >::iterator aPrim;
        std::list< IPrimitive* >& pPrimList = (*model)->getPrimitiveList();

        for ( aPrim = pPrimList.begin(); aPrim != pPrimList.end(); aPrim++ )
        {
            // get bounding box of Prim
            AABB primBox = (*aPrim)->getAABB();

            Point3 boxMin = primBox.getPos();
            Point3 boxMax = primBox.getPos() + primBox.getSize();

            // find out the range of cells that could contain the triangle's bounding box
            int x1 = (int)((boxMin.X() - m_MinPt.X()) * dReci.X()), x2 = (int)((boxMax.X() - m_MinPt.X()) * dReci.X());
            int y1 = (int)((boxMin.Y() - m_MinPt.Y()) * dReci.Y()), y2 = (int)((boxMax.Y() - m_MinPt.Y()) * dReci.Y());
            int z1 = (int)((boxMin.Z() - m_MinPt.Z()) * dReci.Z()), z2 = (int)((boxMax.Z() - m_MinPt.Z()) * dReci.Z());

            x1 = (x1 < 0)?0:x1, x2 = (x2 > (m_gridDimension - 1))?m_gridDimension - 1:x2;
            y1 = (y1 < 0)?0:y1, y2 = (y2 > (m_gridDimension - 1))?m_gridDimension - 1:y2;
            z1 = (z1 < 0)?0:z1, z2 = (z2 > (m_gridDimension - 1))?m_gridDimension - 1:z2;

            if( boxMin.X() == boxMax.X() ) { x2 = x1; }
            if( boxMin.Y() == boxMax.Y() ) { y2 = y1; }
            if( boxMin.Z() == boxMax.Z() ) { z2 = z1; }

            //globalsize += (x2-x1+1)+(y2-y1+1)+(z2-z1+1);
            for ( int x = x1; x <= x2; x++ )
            {
                for ( int y = y1; y <= y2; y++ )
                {
                    for ( int z = z1; z <= z2; z++ )
                    {
                        num_Grid[x][y][z] += 1;
                        globalsize++;
                    } // end z
                } // end y
            } // end x
        }
    } // end gather size

    /*
    unsigned int what = 0;
    for(int x = 0; x < m_gridDimension; x++)
    {
        for(int y = 0; y < m_gridDimension; y++)
        {
            for(int z = 0; z < m_gridDimension; z++)
                what += num_Grid[x][y][z];
        }
    }
    */
    // allocate the space
    try {
        gdata = new IPrimitive*[globalsize]; }
    catch(std::bad_alloc&) {
        printf("out of memory\n");
        exit(1);
    }
    // set up look up index
    unsigned int baseC = 0;
    for(int x = 0; x < m_gridDimension; x++)
    {
        for(int y = 0; y < m_gridDimension; y++)
        {
            for(int z = 0; z < m_gridDimension; z++)
            {
                m_Grid[x][y][z] = baseC; 
                baseC += num_Grid[x][y][z];
                //baseC += (num_Grid[x][y][z]+1);
                num_Grid[x][y][z] = 0;
            }
        }
    }

    // input information
    for( model = modelList.begin(); model != modelList.end(); model++ )
    {
        std::list< IPrimitive* >::iterator aPrim;
        std::list< IPrimitive* >& pPrimList = (*model)->getPrimitiveList();

        for ( aPrim = pPrimList.begin(); aPrim != pPrimList.end(); aPrim++ )
        {
            // get bounding box of Prim
            AABB primBox = (*aPrim)->getAABB();

            Point3 boxMin = primBox.getPos();
            Point3 boxMax = primBox.getPos() + primBox.getSize();

            // find out the range of cells that could contain the triangle's bounding box
            int x1 = (int)((boxMin.X() - m_MinPt.X()) * dReci.X()), x2 = (int)((boxMax.X() - m_MinPt.X()) * dReci.X());
            int y1 = (int)((boxMin.Y() - m_MinPt.Y()) * dReci.Y()), y2 = (int)((boxMax.Y() - m_MinPt.Y()) * dReci.Y());
            int z1 = (int)((boxMin.Z() - m_MinPt.Z()) * dReci.Z()), z2 = (int)((boxMax.Z() - m_MinPt.Z()) * dReci.Z());

            x1 = (x1 < 0)?0:x1, x2 = (x2 > (m_gridDimension - 1))?m_gridDimension - 1:x2;
            y1 = (y1 < 0)?0:y1, y2 = (y2 > (m_gridDimension - 1))?m_gridDimension - 1:y2;
            z1 = (z1 < 0)?0:z1, z2 = (z2 > (m_gridDimension - 1))?m_gridDimension - 1:z2;

            if( boxMin.X() == boxMax.X() ) { x2 = x1; }
            if( boxMin.Y() == boxMax.Y() ) { y2 = y1; }
            if( boxMin.Z() == boxMax.Z() ) { z2 = z1; }

            for ( int x = x1; x <= x2; x++ )
            {
                for ( int y = y1; y <= y2; y++ )
                {
                    for ( int z = z1; z <= z2; z++ )
                    {
#ifdef NOTRIBOX
                        if((m_Grid[x][y][z]+num_Grid[x][y][z]) >= globalsize)
                            printf("mmm");
                        gdata[(m_Grid[x][y][z]+num_Grid[x][y][z])] = (*aPrim);
                        num_Grid[x][y][z]++;
                        //m_Grid[x][y][z]->push_back( *aPrim );
                        m_num_Rep_Prims++;
#else
                        // construct aabb for current cell position
                        Point3 minCorner( m_MinPt.X() + x * delta.X(), m_MinPt.Y() + y * delta.Y(), m_MinPt.Z() + z * delta.Z() );
                        AABB cell( minCorner, minCorner + delta.ToPoint());

                        // test if the primitive actually intersects the cell.
                        if ( (*aPrim)->intersectAABB( cell ) )
                        {
                            // Add this Primitive to the current cell in the Grid
                            //m_Grid[x][y][z]->push_back( *aPrim );
                            gdata[(m_Grid[x][y][z]+num_Grid[x][y][z])] = (*aPrim);
                            num_Grid[x][y][z]++;

                            // a single Primitive could be in multiple cells, so we need to count repeated Prims here
                            m_num_Rep_Prims++;
                        }
#endif
                    } // end z
                } // end y
            } // end x
        }
    } // end input information


#else
    // initialize 3D Grid of TriangleLists
    if ( m_Grid )
    {
        for (int x=0; x < m_gridDimension; x++)
            for (int y=0; y < m_gridDimension; y++)
                for (int z=0; z < m_gridDimension; z++)
                    m_Grid[x][y][z]->clear();
    }
    else
    {
        m_Grid = new std::list< IPrimitive* >***[m_gridDimension];
        for (int x=0; x < m_gridDimension; x++)
        {
            m_Grid[x] = new std::list< IPrimitive* >**[m_gridDimension];
            for (int y=0; y < m_gridDimension; y++)
            {
                m_Grid[x][y] = new std::list< IPrimitive* >*[m_gridDimension];
                for (int z=0; z < m_gridDimension; z++)
                {
                    m_Grid[x][y][z] = new std::list< IPrimitive* >;
                }
            }
        }
    }

    // intialize number of repeated Primitives
    m_num_Rep_Prims = 0;

    // for every Primitive of every model
    std::list< IModel* >::iterator model;

    for( model = modelList.begin(); model != modelList.end(); model++ )
    {
        std::list< IPrimitive* >::iterator aPrim;
        std::list< IPrimitive* >& pPrimList = (*model)->getPrimitiveList();

        for ( aPrim = pPrimList.begin(); aPrim != pPrimList.end(); aPrim++ )
        {
            // get bounding box of Prim
            AABB primBox = (*aPrim)->getAABB();

            Point3 boxMin = primBox.getPos();
            Point3 boxMax = primBox.getPos() + primBox.getSize();

            // find out the range of cells that could contain the triangle's bounding box
            int x1 = (int)((boxMin.X() - m_MinPt.X()) * dReci.X()), x2 = (int)((boxMax.X() - m_MinPt.X()) * dReci.X());
            int y1 = (int)((boxMin.Y() - m_MinPt.Y()) * dReci.Y()), y2 = (int)((boxMax.Y() - m_MinPt.Y()) * dReci.Y());
            int z1 = (int)((boxMin.Z() - m_MinPt.Z()) * dReci.Z()), z2 = (int)((boxMax.Z() - m_MinPt.Z()) * dReci.Z());

            x1 = (x1 < 0)?0:x1, x2 = (x2 > (m_gridDimension - 1))?m_gridDimension - 1:x2;
            y1 = (y1 < 0)?0:y1, y2 = (y2 > (m_gridDimension - 1))?m_gridDimension - 1:y2;
            z1 = (z1 < 0)?0:z1, z2 = (z2 > (m_gridDimension - 1))?m_gridDimension - 1:z2;

            if( boxMin.X() == boxMax.X() ) { x2 = x1; }
            if( boxMin.Y() == boxMax.Y() ) { y2 = y1; }
            if( boxMin.Z() == boxMax.Z() ) { z2 = z1; }

            // now check those cells for intersection of the triangle
            //int maxindex = m_gridDimension-1;
            for ( int x = x1; x <= x2; x++ )
            {
                for ( int y = y1; y <= y2; y++ )
                {
                    for ( int z = z1; z <= z2; z++ )
                    {
#ifdef NOTRIBOX
                        m_Grid[x][y][z]->push_back( *aPrim );
                        m_num_Rep_Prims++;
#else
                        // construct aabb for current cell position
                        Point3 minCorner( m_MinPt.X() + x * delta.X(), m_MinPt.Y() + y * delta.Y(), m_MinPt.Z() + z * delta.Z() );
                        AABB cell( minCorner, minCorner + delta.ToPoint());

                        // test if the primitive actually intersects the cell.
                        if ( (*aPrim)->intersectAABB( cell ) )
                        {
                            // Add this Primitive to the current cell in the Grid
                            m_Grid[x][y][z]->push_back( *aPrim );

                            // a single Primitive could be in multiple cells, so we need to count repeated Prims here
                            m_num_Rep_Prims++;
                        }
#endif
                    }
                }
            }
        }
    }
#endif
}




bool UniformGrid::rayBoxIntersect( Ray *aRay, Point3 bmin, Point3 bmax, float* timeIn ) const
{
    Vector3 rayD = aRay->Direction();
    Point3 rayStart = aRay->Origin();

    Point3 inv_rayD(1/rayD.X(), 1/rayD.Y(), 1/rayD.Z());

    float timeOut;

    // calculate time to hit the bounding box
    Point3 tempIn((bmin.X() - rayStart.X())*inv_rayD.X(),
        (bmin.Y() - rayStart.Y())*inv_rayD.Y(),
        (bmin.Z() - rayStart.Z())*inv_rayD.Z());
    Point3 tempOut((bmax.X() - rayStart.X())*inv_rayD.X(),
        (bmax.Y() - rayStart.Y())*inv_rayD.Y(),
        (bmax.Z() - rayStart.Z())*inv_rayD.Z());

    Point3 tIn = tempIn;
    Point3 tOut = tempOut;

    // if the ray diection is negative, we have to switch in and out times
    if(rayD.X() < 0){
        tIn.X() = tempOut.X();
        tOut.X() = tempIn.X();
    }
    if(rayD.Y() < 0){
        tIn.Y() = tempOut.Y();
        tOut.Y() = tempIn.Y();
    }
    if(rayD.Z() < 0){
        tIn.Z() = tempOut.Z();
        tOut.Z() = tempIn.Z();
    }

    // get highest in time and lowest out time
    *timeIn = max(max(tIn.X(), tIn.Y()), tIn.Z());
    timeOut = min(min(tOut.X(), tOut.Y()), tOut.Z());

    // if it goes in after it goes out, then it must be invalid
    return (timeOut > *timeIn);
}

void UniformGrid::stepgrid(Ray *l_pRay, Point3 l_step, Point3 *l_tMax) const
{
    float tmin = min(min(l_tMax->X(), l_tMax->Y()), l_tMax->Z());

    /* improved on the fast voxel traversal algorithm using code below
    /* taken from a MA thesis that Chen-Hao read. */
    Point3 mask(tmin,tmin,tmin);
    mask.X() = (tmin == l_tMax->X())?1.0f:0.0f;
    mask.Y() = (tmin == l_tMax->Y())?1.0f:0.0f;
    mask.Z() = (tmin == l_tMax->Z())?1.0f:0.0f;

    Vector3 rayDir = l_pRay->Direction();

    Point3 tDelta( (rayDir.X()==0) ? POS_INF : abs(m_cellSize/rayDir.X()),
                    (rayDir.Y()==0) ? POS_INF : abs(m_cellSize/rayDir.Y()),
                    (rayDir.Z()==0) ? POS_INF : abs(m_cellSize/rayDir.Z()) );

    Point3 voxel = l_pRay->GetTracePosition();
	voxel = voxel + Point3( mask.X()*l_step.X(), mask.Y()*l_step.Y(), mask.Z()*l_step.Z() );
	*l_tMax = *l_tMax + Point3( mask.X()*tDelta.X(), mask.Y()*tDelta.Y(), mask.Z()*tDelta.Z() );

    // move ray through the grid
    // by setting it to what will be the new tmin
    l_pRay->SetTracePosition( voxel );
    l_pRay->SetTraceTime( min( min( l_tMax->X(), l_tMax->Y() ), l_tMax->Z() ) );
}

Point3 UniformGrid::sign(Point3 value) const
{
    Point3 ret((value.X() > 0)?1.0f:-1.0f,(value.Y() > 0)?1.0f:-1.0f, (value.Z() > 0)?1.0f:-1.0f);
    return ret;
}

Point3 UniformGrid::clamp( Point3 value, Point3 minimum, Point3 maximum ) const
{
    Point3 ret = value;
    if (value.X() < minimum.X()) ret.X() = minimum.X();
    if (value.Y() < minimum.Y()) ret.Y() = minimum.Y();
    if (value.Z() < minimum.Z()) ret.Z() = minimum.Z();

    if (value.X() > maximum.X()) ret.X() = maximum.X();
    if (value.Y() > maximum.Y()) ret.Y() = maximum.Y();
    if (value.Z() > maximum.Z()) ret.Z() = maximum.Z();
    return ret;
}

void UniformGrid::getVoxel(Ray *aRay) const
{
    float inv_len = 1.0f/m_cellSize;

    // get current position of ray
    Vector3 rayDir = aRay->Direction();
    Point3 rayPos = aRay->Origin() + (rayDir * aRay->GetTraceTime()).ToPoint();

    // clamp between 0 and m_gridDimension-1
    Point3 tfloor( floor((rayPos.X()-m_MinPt.X())*inv_len),
        floor((rayPos.Y()-m_MinPt.Y())*inv_len),
        floor((rayPos.Z()-m_MinPt.Z())*inv_len) );

    Point3 voxel = clamp( tfloor, Point3(0.0, 0.0, 0.0), Point3( (float)m_gridDimension-1, (float)m_gridDimension-1, (float)m_gridDimension-1) );

    aRay->SetTracePosition( voxel );
}

HitInfo UniformGrid::intersect(Ray& l_Ray)
{
    HitInfo bestHit;

    Point3 tMax( POS_INF, POS_INF, POS_INF );
    Pixel cell_info;
    bool bStopTraversing = false;

    float timeIn = 0;

    // if this is the first time we've seen this ray
    if ( l_Ray.GetTraceTime() == 0 )
    {
        if ( !rayBoxIntersect( &l_Ray, m_MinPt, m_MaxPt, &timeIn) )
        {
            // ray misses the box, so there can't be a hit
            HitInfo noHit;
            return noHit;
        }
        else
        {
            // ray hits the box, so set current time as timeIn
            // NOTE: this has to happen before getVoxel so that 
            // the ray is in the proper location 
            l_Ray.SetTraceTime( timeIn );

            // set the position of the ray to be the correct voxel indices
            getVoxel( &l_Ray);
        }
    }

    Vector3 rayDir = l_Ray.Direction();
    Point3 rayVoxel = l_Ray.GetTracePosition();
    Point3 rayPos = l_Ray.Origin() + (rayDir * l_Ray.GetTraceTime()).ToPoint();

    // determine which direction to step the ray
    // determine what values indicate the ray left the grid
    // determine a mask of positive directions to calculate tMax
    // all assume rays are traveling in a positive direction
    Point3 outS( (float)m_gridDimension, (float)m_gridDimension, (float)m_gridDimension );
    Point3 step(1,1,1);
    Point3 pos(1,1,1);

    if(rayDir.X() < 0) { step.X() = -1; outS.X() = -1; pos.X() = 0; }
    if(rayDir.Y() < 0) { step.Y() = -1; outS.Y() = -1; pos.Y() = 0; }
    if(rayDir.Z() < 0) { step.Z() = -1; outS.Z() = -1; pos.Z() = 0; }

    // find tmax of ray in the initial grid
    Point3 tempPoint = (((rayVoxel + pos) * m_cellSize + m_MinPt) - rayPos );
    tMax.X() = tempPoint.X() / rayDir.X();
    tMax.Y() = tempPoint.Y() / rayDir.Y();
    tMax.Z() = tempPoint.Z() / rayDir.Z();

    // bStopTraversing = true if we don't need to traverse:
    //0) if ray doesn't hit grid (checked above)
    //1) if PrimList is empty, then this ray is done (below)
    //2) if PrimList has triangles, then the renderer should intersect with them (below)
    while( bStopTraversing == false )
    {
        Point3 rayVoxelPosition = l_Ray.GetTracePosition();

        // find out if the ray has gone outside the grid
        Point3 outside( rayVoxelPosition.X() == outS.X(),
            rayVoxelPosition.Y() == outS.Y(),
            rayVoxelPosition.Z() == outS.Z());

        if ((outside.X() + outside.Y() + outside.Z()) > 0)
        {
            // we've gone outside the grid
            // so there can't be a hit
            bStopTraversing = true;
        }
        else
        {
            // we are in the grid
#ifdef NEW
            // get Primitives at this cell
            //std::list< IPrimitive* >* lpPrims = m_Grid[(int)rayVoxelPosition.X()][(int)rayVoxelPosition.Y()][(int)rayVoxelPosition.Z()];
            unsigned int mybase = m_Grid[(int)rayVoxelPosition.X()][(int)rayVoxelPosition.Y()][(int)rayVoxelPosition.Z()];
            unsigned short mysize = num_Grid[(int)rayVoxelPosition.X()][(int)rayVoxelPosition.Y()][(int)rayVoxelPosition.Z()];
            // check intersect triangles and record best hit
            //std::list< IPrimitive* >::iterator ilpPrims;
            if ( mysize > 0 )
            {
                for( unsigned int i = 0; i < mysize; i++ )
                {
                    HitInfo newHit = gdata[mybase+i]->intersect( l_Ray );
#else
            // get Primitives at this cell
            std::list< IPrimitive* >* lpPrims = m_Grid[(int)rayVoxelPosition.X()][(int)rayVoxelPosition.Y()][(int)rayVoxelPosition.Z()];

            // check intersect triangles and record best hit
            std::list< IPrimitive* >::iterator ilpPrims;
            if ( lpPrims->size() > 0 )
            {
                for( ilpPrims = lpPrims->begin(); ilpPrims != lpPrims->end(); ilpPrims++ )
                {
                    HitInfo newHit = (*ilpPrims)->intersect( l_Ray );
#endif
                    if ( 0 < newHit.hitTime && 
                         newHit.hitTime < bestHit.hitTime )
                    {
                        bestHit = newHit;
                        bestHit.hitVoxel = rayVoxelPosition / (float)m_gridDimension;
                        bStopTraversing = true;
                    }
                }
            }
            else
            {
                bStopTraversing = false;
            }
            // then step the ray into the next cell
            // to prepare for the next traversal
            stepgrid( &l_Ray, step, &tMax);
        }
    }

    return bestHit;
}
