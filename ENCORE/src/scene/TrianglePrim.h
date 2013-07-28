#pragma once

#include "Primitive.h"
#include "Triangle.h"

#include <cmath>

class TrianglePrim : public IPrimitive
{
public:
    TrianglePrim( Triangle& l_Triangle );
    TrianglePrim( Vertex& l_v0, Vertex& l_v1, Vertex& l_v2 );
    virtual ~TrianglePrim(void);

    //// IPrimitive Interface ////
    virtual bool intersect(Ray& l_Ray, HitInfo* pHitInfo);
    virtual bool intersectAABB(AABB l_Box );
    virtual AABB getAABB( void ) const;
    virtual std::list<Triangle*>* getNewTesselation ( void );

private:
    Triangle* m_pTriangle;
};

/////////////////
// some defines
////////////////

#define dmin(a,b) (((a)>(b)?(b):(a)))
#define dmax(a,b) (((a)>(b)?(a):(b)))

#define FINDMINMAX(x0,x1,x2,min,max) \
  min = max = x0;   \
  if(x1<min) min=x1;\
  if(x1>max) max=x1;\
  if(x2<min) min=x2;\
  if(x2>max) max=x2;

/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)             \
    p0 = a*v0.Y() - b*v0.Z();                      \
    p2 = a*v2.Y() - b*v2.Z();                      \
    if(p0<p2) {minCoord=p0; maxCoord=p2;} else {minCoord=p2; maxCoord=p0;} \
    rad = fa * boxhalfsize.Y() + fb * boxhalfsize.Z();   \
    if(minCoord>rad || maxCoord < -rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)              \
    p0 = a*v0.Y() - b*v0.Z();                      \
    p1 = a*v1.Y() - b*v1.Z();                      \
    if(p0<p1) {minCoord=p0; maxCoord=p1;} else {minCoord=p1; maxCoord=p0;} \
    rad = fa * boxhalfsize.Y() + fb * boxhalfsize.Z();   \
    if(minCoord>rad || maxCoord < -rad) return 0;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)             \
    p0 = -a*v0.X() + b*v0.Z();                 \
    p2 = -a*v2.X() + b*v2.Z();                     \
    if(p0<p2) {minCoord=p0; maxCoord=p2;} else {minCoord=p2; maxCoord=p0;} \
    rad = fa * boxhalfsize.X() + fb * boxhalfsize.Z();   \
    if(minCoord>rad || maxCoord < -rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)              \
    p0 = -a*v0.X() + b*v0.Z();                 \
    p1 = -a*v1.X() + b*v1.Z();                     \
    if(p0<p1) {minCoord=p0; maxCoord=p1;} else {minCoord=p1; maxCoord=p0;} \
    rad = fa * boxhalfsize.X() + fb * boxhalfsize.Z();   \
    if(minCoord>rad || maxCoord < -rad) return 0;

/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)             \
    p1 = a*v1.X() - b*v1.Y();                      \
    p2 = a*v2.X() - b*v2.Y();                      \
    if(p2<p1) {minCoord=p2; maxCoord=p1;} else {minCoord=p1; maxCoord=p2;} \
    rad = fa * boxhalfsize.X() + fb * boxhalfsize.Y();   \
    if(minCoord>rad || maxCoord < -rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)              \
    p0 = a*v0.X() - b*v0.Y();                  \
    p1 = a*v1.X() - b*v1.Y();                      \
    if(p0<p1) {minCoord=p0; maxCoord=p1;} else {minCoord=p1; maxCoord=p0;} \
    rad = fa * boxhalfsize.X() + fb * boxhalfsize.Y();   \
    if(minCoord>rad || maxCoord < -rad) return 0;
		
