/****************************************************************************
Brandon Light
03/21/2006

Common.cpp

****************************************************************************/

#include "Common.h"
#include "TAffineMatrix.h"

#include <cstdlib>
#include <sstream>

using encore::Point3;

float randNumber(float inmin, float inmax) 
{ 
    return (inmax - inmin)*((rand() % 10000) / (float)10000) + inmin;
	//return 2.0f*((rand() % 10000) / (float)10000) - 1; 
}

float randNumberN(float inMin, float inMax)
{
    return 0.0f;
}


Vector3 randDirection()
{
    float x, y, z;

#if 0
    static const float thetaMin = -PI;
    static const float thetaMax = PI;

    y = randNumber(-1.0f, 1.0f);
    float theta = randNumber(thetaMin, thetaMax);
    float phi = acos(y);

    x = sin(phi)*cos(theta);
    z = sin(phi)*sin(theta);
#elif 0
    static const float thetaMin = 0;
    static const float thetaMax = 2*PI;
    static const float phiMin   = 0;
    static const float phiMax   = 2*PI;

    float phi = randNumber(thetaMin, thetaMax);
    float theta   = randNumber(phiMin, phiMax);

    x = sin(theta)*cos(phi);
    y = cos(theta);
    z = sin(theta)*sin(phi);
#else
    do
    {

        float E1 = randNumber(0, 1);   // E1 is a random number in the range of [0,1]
        float E2 = randNumber(0, 1);
        float E3 = randNumber(0, 1);

        x = 2 * E1 - 1;   // scale and bias the random variable into the [-1,1] range.
        y = 2 * E2 - 1;
        z = 2 * E3 - 1;

    } while ( (x*x + y*y + z*z) > 1 );  // reject sample if its not in the unit sphere.

#endif
    
	Vector3 dir(x, y, z);
	dir.Normalize();
    

	return dir;
}

Vector3 randSurfaceReflection(Vector3 surfNormal)
{
	Vector3 dir;

	do 
	{
		dir = randDirection();
		dir.Normalize();
	} while(Dot(dir, surfNormal) <= 0);
   

	return dir;
}

Vector3 randDirectionN(Vector3 n)
{
    Vector3 dir;

#if 1
    float e1 = randNumber(0.01, 1); // cutting off the very tip of the cosine-weighted distribution
    float e2 = randNumber(0, 1);

    float theta = acos(sqrt(e1));
    float phi = 2 * PI * e2;

    dir.X() = sin(theta) * cos(phi);
    dir.Y() = sin(theta) * sin(phi);
    dir.Z() = cos(theta);

#else

    float phi, r;
    randPointOnDisc(phi, r);

    dir.X() = r * cos(phi);
    dir.Y() = r * sin(phi);
    dir.Z() = sqrt(1 - (r*r));

#endif
    
    // dir aligned with +z axis.  need to align with N
    Vector3 zAxis(0, 0, 1);
    n.Normalize();

    if(zAxis == n)
    {
        // already aligned
        return dir;
    }
    else if(zAxis == -n)
    {
        // opposite direction, reverse direction
        return -dir;
    }

    float angle = acos(Dot(n, zAxis));

    Vector3 rv = Cross(zAxis, n);
    TAffineMatrix<float> m = TAffineMatrix<float>::GetRotationMatrix(rv, angle);
    dir =  m * dir;

    dir.Normalize();

    return dir;
}

Vector3 randDirectionSuperN(Vector3 n)
{
    Vector3 dir;

    float e1 = randNumber(0.001, 1);
    float e2 = randNumber(0, 1);

    float theta = acos(sqrt(e1));
    float phi = 2 * PI * e2;

    dir.X() = sin(theta) * cos(phi);
    dir.Y() = sin(theta) * sin(phi);
    dir.Z() = cos(theta);

    // dir aligned with +z axis.  need to align with N
    Vector3 zAxis(0, 0, 1);
    n.Normalize();

    if(zAxis == n)
    {
        // already aligned
        return dir;
    }
    else if(zAxis == -n)
    {
        // opposite direction, reverse direction
        return -dir;
    }

    float angle = acos(Dot(n, zAxis));

    Vector3 rv = Cross(zAxis, n);
    TAffineMatrix<float> m = TAffineMatrix<float>::GetRotationMatrix(rv, angle);
    dir =  m * dir;

    dir.Normalize();

    return dir;
}

void randPointOnDisc(float &phi, float &r)
{
    float e1 = randNumber(0, 1);   // E1 is a random number in the range of [0,1]
    float e2 = randNumber(0, 1);

    float r1 = 2 * e1 - 1;   // scale and bias the random variable into the [-1,1] range.
    float r2 = 2 * e2 - 1;
    
    if(r1 > r2 && r1 > -r2)
    {
        // first trianglular region
        r = r1;
        phi = (PI / 4.0f) * (r2 / r1);
    }
    else if(r1 < r2 && r1 > -r2)
    {
        // second
        r = r2;
        phi = (PI / 4.0f) * (2 - r1/r2);
    }
    else if(r1 < r2 && r1 < -r2 )
    {
        // third
        r = -r1;
        phi = (PI / 4.0f) * (4 + r2/r1);
    }
    else // r1 > r2 && r1 < -r2
    {   
        // fourth
        r = -r2;
        phi = (PI / 4.0f) *  (6 - r1/r2);
    }
}

std::string intToString( int value )
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

std::string floatToString( float value )
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

