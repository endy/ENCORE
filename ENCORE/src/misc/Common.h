/****************************************************************************
Brandon Light
03/21/2006

Common.h

****************************************************************************/
#pragma once

#include <float.h>
#include "TVector3.h"

#include <string>
#include <iostream>

using encore::Vector3f;

///// COMMON MACROS  ///////////////////////////////////////

#ifndef NULL
#define NULL 0
#endif

#define POS_INF FLT_MAX
#define NEG_INF FLT_MIN

#define EPSILON 0.001 


///// COMMON GLOBALS   /////////////////////////////////////

const float PI = 3.14159f;

///// COMMON METHODS  /////////////////////////////////////

/*
*  Returns a random number n where min < n < max
*/
float randNumber(float inmin, float inmax);

/*
*  Returns a random number with a Normal distribution between inMin & inMax
*/
float randNumberN(float inMin, float inMax);

/*
*  Returns a random direction vector 
*/
Vector3f randDirection();

/*
*  Get random surface reflection vector
*/
Vector3f randSurfaceReflection(Vector3f surfNormal);

/*
*  Return a cosine weighted random direction using the normal n
*/
Vector3f randDirectionN(Vector3f n);

Vector3f randDirectionSuperN(Vector3f n);

// generates a random number on a disc
void randPointOnDisc(float &phi, float &r);


std::string intToString( int value );

std::string floatToString( float value);

inline void PTestPrint(std::string s)
{
#ifdef POWER_TEST_PRINT
    std::cout << s;
#endif
}
