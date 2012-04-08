#pragma once
#include <string>
#include <deque>
#include "TVector3.h"

using namespace std;
using encore::Vector3f;

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

    const deque<Vector3f>& getVertex() const { return vertice; }
    const deque<Vector3f>& getTexCoord() const { return texcoord; }
    const deque<Vector3f>& getNormal() const { return normal; }
    const deque<TFace>& getIndex() const { return index; }
    bool hasNormal() const { return hasNorm; }
    bool hasTexCoord() const { return hasTex; }
private:
    string mtlname;
    deque<Vector3f> vertice;
    deque<Vector3f> texcoord;
    deque<Vector3f> normal;
    deque<TFace> index;
    bool hasNorm;
    bool hasTex;
    
    void skipLine(string::const_iterator cit, string::const_iterator cend);
    void skipWhiteSpace(string::const_iterator &cit);
    float gatherstf(string::const_iterator &cit);
    int gathersti(string::const_iterator &cit);
};
