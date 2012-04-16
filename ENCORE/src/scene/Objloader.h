#pragma once
#include <string>
#include <deque>
#include "Vector3.h"

using namespace std;
using encore::Vector3;

struct TFace
{
    unsigned int vIndex[3];
    unsigned int nIndex[3];
    unsigned int tIndex[3];
};

class Objloader
{
public:
    Objloader(void): hasNorm(0), hasTex(0) {}
    Objloader(const string &fname);
    ~Objloader(void) { vertice.clear(); texcoord.clear(); normal.clear(); index.clear(); }
    
    void load(const string &fname);

    const deque<Vector3>& getVertex() const { return vertice; }
    const deque<Vector3>& getTexCoord() const { return texcoord; }
    const deque<Vector3>& getNormal() const { return normal; }
    const deque<TFace>& getIndex() const { return index; }
    bool hasNormal() const { return hasNorm; }
    bool hasTexCoord() const { return hasTex; }
private:
    string mtlname;
    deque<Vector3> vertice;
    deque<Vector3> texcoord;
    deque<Vector3> normal;
    deque<TFace> index;
    bool hasNorm;
    bool hasTex;
    
    void skipLine(string::const_iterator cit, string::const_iterator cend);
    void skipWhiteSpace(string::const_iterator &cit);
    float gatherstf(string::const_iterator &cit);
    int gathersti(string::const_iterator &cit);
};
