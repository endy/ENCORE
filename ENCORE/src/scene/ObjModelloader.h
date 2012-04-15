#pragma once

#include<string>
#include<list>
#include<deque>
#include "Vector3.h"
#include "DynamicModel.h"

using namespace std;
using encore::Vector3;

class ObjModelloader
{
public:
    ObjModelloader(void):toCenter(0,0,0), scale(0), rAngle(0), rAxis(0,0,0) {}
    ~ObjModelloader(void) {}

    list<DynamicModel*> loadmulti(const string& fname);
    //list<DynamicModel*>* loadcomplete(const string& fname) {} // generates a warning...
    list<DynamicModel*>* loadsimple(const string& fname);

    void setTransform( const Point& center, float size, float angle, const Vector3& axis)
    {
        toCenter = center; scale = size; rAngle = angle;
        rAxis = axis;
    }

private:
    void skipLine(string::const_iterator cit, string::const_iterator cend);
    void skipWhiteSpace(string::const_iterator &cit);
    float gatherstf(string::const_iterator &cit);
    int gathersti(string::const_iterator &cit);

    Point toCenter;
    float scale;
    float rAngle;
    Vector3 rAxis;  
};
