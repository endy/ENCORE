#pragma once
#include "Renderer.h"

#include "HitInfo.h"
#include "Pixel.h"

//forward declarations
class AccelerationStructure;
class Scene;
class Camera;


//@@@@@@@@@@@@@@@@@@@@ CPURTRenderer class @@@@@@@@@@@@@@@@@@@
class CPURTRenderer : public Renderer
{
public:

	CPURTRenderer(void);
	virtual ~CPURTRenderer(void);

    void init(Scene *scn, AccelerationStructure *accelStruct, Camera* cam);
	void render( void );
	void deinit(void);
    void keyboard( unsigned char key );
    void usage( void );
    void configure( Options* l_pOptions );

    Color shade(Ray *l_Ray);
	void setRecursionDepth(int depth){recursionDepth = depth;}

private:	
    enum ViewState { PHONG, NORMAL, VOXEL };
    ViewState m_view;

	bool alreadyBuiltScene;
    int traversals;

    int m_pixelSize;

	int recursionDepth;
    int numCols;
	int numRows;
	Pixel** rayStartMap;
	Pixel** rayDirMap;
	Pixel** hitInfoMap;
	Pixel** finalColorMap;

	//Extra textures we can write/display
	Pixel** voxelMap;

	Pixel* vertexArray;
	Pixel* normalArray;
	Pixel*** gridInfoMap;
	Pixel* vert0;
	Pixel* vert1;
	Pixel* vert2;
	Pixel* norm0;
	Pixel* norm1;
	Pixel* norm2;

	HitInfo traverseAndIntersect( Ray* aRay );
	Color phongShade( HitInfo bestHit, Ray* l_Ray );


//	bool isInShadow(Ray& feeler);
//	void getFirstHit(Ray& ray, Intersection& best);
};


