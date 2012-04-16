#include "ObjModelloader.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>


/**********************************************************************/
// helper functions

void ObjModelloader::skipLine(string::const_iterator cit, string::const_iterator cend)
{
    while(*cit != '\n' && cit != cend)
        cit++;
    if(cit == cend)
        return;
    cit++;
}

void ObjModelloader::skipWhiteSpace(string::const_iterator &cit)
{
    while(*cit == ' ')
        cit++;
}

// turn a continues string into float sperated by space and endline
float ObjModelloader::gatherstf(string::const_iterator &cit)
{
    string temp;
    while(*cit != ' ' && *cit != '\n') // append text onto the string 
    {
        temp += *cit;
        cit++;
    }
    cit++; // skip next space or endline
    return (float)atof(temp.c_str());
}

// turn a continuous string into float sperated by space and endline
int ObjModelloader::gathersti(string::const_iterator &cit)
{
    string temp;
    while(*cit != ' ' && *cit != '/' && *cit != '\n') // append text onto the string 
    {
        temp += *cit;
        cit++;
    }
    cit++; // skip next space or endline
    return atoi(temp.c_str());
}

// end helper functions
/**********************************************************************/

list<DynamicModel*> ObjModelloader::loadmulti(const string& fname)
{
    // construct list of models to return
    list<DynamicModel*> myList;// = new list<DynamicModel*>();

    // reads file into memory so that we can do
    // string parsing on it
    FILE *f = fopen(fname.c_str(), "rb");
    if(!f)
    {
        printf("Unable to open obj file %s\n",fname.c_str());
        return myList;
    }
    fseek (f , 0 , SEEK_END);
    unsigned long filelength = ftell (f);
    rewind (f);
    char *memfile = new (nothrow) char[filelength];
    if(!memfile)
    {
        printf("out of memory when parsing file\n");
        exit(0);
    }
    fread( memfile, filelength, 1, f );
    fclose(f);

    string FFstring = memfile;
    delete [] memfile;
    // done reading string in from file


    string::const_iterator csi; // constant string iterator

    // construct necessary holder and class
    DynamicModel *m = new DynamicModel();
    deque<Vector3> normal, vertice;//, texcoord;
    //deque<FaceIndex> face;

    float x,y,z; // temp number to hold vertex, normal or texel
    FaceIndex temp_f;// temp face for holding coord Index and texture Index
    bool wasFace = false;

    int vminus = 0; // number of vertex index to minus from
    //int tminus = 0;
    int count = 0;
    csi = FFstring.begin();
    while(csi != FFstring.end())
    {
        switch(*csi)
        {
        case 'v':
            wasFace = false;
            csi++;
            if(*csi == 'n')
            {
                m->m_bHasNormals = true;
                csi++;
                skipWhiteSpace(csi);
                x = gatherstf(csi);
                skipWhiteSpace(csi);
                y = gatherstf(csi);
                skipWhiteSpace(csi);
                z = gatherstf(csi);
                normal.push_back(Vector3(x,y,z)); // add normal data
            }
            else if(*csi == 't')
            {
                m->m_bHasTexture = true;
                csi++;
                skipWhiteSpace(csi);
                x = gatherstf(csi);
                skipWhiteSpace(csi);
                y = gatherstf(csi);
                skipWhiteSpace(csi);
                z = gatherstf(csi);
                //texcoord.push_back(Vector3(x,y,z));
                m->getTexCoords()->push_back(Vector3(x,y,z)); // hard copy
            }
            else
            {
                skipWhiteSpace(csi);
                x = gatherstf(csi);
                skipWhiteSpace(csi);
                y = gatherstf(csi);
                skipWhiteSpace(csi);
                z = gatherstf(csi);
                vertice.push_back(Vector3(x,y,z)); // hard copy  

                m->MaxPoint()[0] = std::max(m->MaxPoint()[0], x);
                m->MaxPoint()[1] = std::max(m->MaxPoint()[1], y);
                m->MaxPoint()[2] = std::max(m->MaxPoint()[2], z);
                m->MinPoint()[0] = std::min(m->MinPoint()[0], x);
                m->MinPoint()[1] = std::min(m->MinPoint()[1], y);
                m->MinPoint()[2] = std::min(m->MinPoint()[2], z);
            }            
            break;
        case 'f':
            wasFace = true;
            csi++;
            //count++;
            skipWhiteSpace(csi);
            temp_f.Index[0] = gathersti(csi)-1-vminus;
            if(m->m_bHasNormals) /*temp_f.nIndex[0] = */gathersti(csi);
            if(m->m_bHasTexture) temp_f.tIndex[0] = gathersti(csi)-1;
            skipWhiteSpace(csi);
            temp_f.Index[1] = gathersti(csi)-1-vminus;
            if(m->m_bHasNormals) /*temp_f.nIndex[1] = */gathersti(csi);
            if(m->m_bHasTexture) temp_f.tIndex[1] = gathersti(csi)-1;
            skipWhiteSpace(csi);
            temp_f.Index[2] = gathersti(csi)-1-vminus;
            if(m->m_bHasNormals) /*temp_f.nIndex[2] = */gathersti(csi);
            if(m->m_bHasTexture) temp_f.tIndex[2] = gathersti(csi)-1;

            //face.push_back(temp_f);
            m->getFaces()->push_back(temp_f); // push the information in by copy
            break;
        case 's':
            skipLine(csi, FFstring.end());
            break;
        case 'u':
            skipLine(csi, FFstring.end());
            break;
        default:
            if(wasFace)
            {
                count++;
                wasFace = false;
                // set up vertex
                for(unsigned int i = 0; i < vertice.size(); i++)
                {
                    Vertex v;
                    if(!m->m_bHasNormals)
                        v.set(vertice[i].X(), vertice[i].Y(), vertice[i].Z(),0,0,0);
                    else
                        v.set(vertice[i].X(), vertice[i].Y(), vertice[i].Z(), 
                            normal[i].X(), normal[i].Y(),normal[i].Z());
                    
                    m->getVertices()->push_back(v);

                }
                // need this to calculate normal!!!
                for(unsigned int i = 0; i < m->getFaces()->size(); i++)
                {
                    Triangle tt;
                    tt.setVertex0((*m->getVertices())[(*m->getFaces())[i].Index[0]]);
                    tt.setVertex1((*m->getVertices())[(*m->getFaces())[i].Index[1]]);
                    tt.setVertex2((*m->getVertices())[(*m->getFaces())[i].Index[2]]);
                    m->getTriangles()->push_back(tt);
                }
                vminus += (int)vertice.size();
                //m->centerModel(toCenter, scale, rAngle, rAxis);
                if(!m->m_bHasNormals) 
                {
                    m->calculateNormals();
                }
                
                m->buildPrimitiveList();
                vector<Vertex>* pv = new std::vector<Vertex>(m->getCVertices());
                m->getVerticesList()->push_front(pv); // push self in

                myList.push_back(m); // push into the list
                
                m = new DynamicModel(); // set up new space for next model
                normal.clear(); vertice.clear(); // clear the helper holder
                //if(count > 10)
                //    return myList;
            }
            skipLine(csi, FFstring.end());
            break;
        }
    }
    printf(".");
    return myList;
}

list<DynamicModel*>* ObjModelloader::loadsimple(const string& fname)
{
    // construct list of models to return
    list<DynamicModel*>* myList = new list<DynamicModel*>();

    // reads file into memory so that we can do
    // string parsing on it
    FILE *f = fopen(fname.c_str(), "rb");
    if(!f)
    {
        printf("Unable to open obj file %s\n",fname.c_str());
        return myList;
    }
    fseek (f , 0 , SEEK_END);
    unsigned long filelength = ftell (f);
    rewind (f);
    char *memfile = new (nothrow) char[filelength];
    if(!memfile)
    {
        printf("out of memory when parsing file\n");
        exit(0);
    }
    fread( memfile, filelength, 1, f );
    fclose(f);

    string FFstring = memfile;
    delete [] memfile;
    // done reading string in from file


    string::const_iterator csi; // constant string iterator

    // construct necessary holder and class
    DynamicModel *m = new DynamicModel();
    deque<Vector3> normal, vertice;//, texcoord;
    //deque<FaceIndex> face;

    float x,y,z; // temp number to hold vertex, normal or texel
    FaceIndex temp_f;// temp face for holding coord Index and texture Index
    bool wasFace = false;

    int vminus = 0; // number of vertex index to minus from
    //int tminus = 0;
    int count = 0;
    csi = FFstring.begin();
    while(csi != FFstring.end())
    {
        switch(*csi)
        {
        case 'v':
            wasFace = false;
            csi++;
            if(*csi == 'n')
            {
                m->m_bHasNormals = true;
                csi++;
                skipWhiteSpace(csi);
                x = gatherstf(csi);
                skipWhiteSpace(csi);
                y = gatherstf(csi);
                skipWhiteSpace(csi);
                z = gatherstf(csi);
                normal.push_back(Vector3(x,y,z)); // add normal data
            }
            else if(*csi == 't')
            {
                m->m_bHasTexture = true;
                csi++;
                skipWhiteSpace(csi);
                x = gatherstf(csi);
                skipWhiteSpace(csi);
                y = gatherstf(csi);
                skipWhiteSpace(csi);
                z = gatherstf(csi);
                //texcoord.push_back(Vector3(x,y,z));
                m->getTexCoords()->push_back(Vector3(x,y,z)); // hard copy
            }
            else
            {
                skipWhiteSpace(csi);
                x = gatherstf(csi);
                skipWhiteSpace(csi);
                y = gatherstf(csi);
                skipWhiteSpace(csi);
                z = gatherstf(csi);
                vertice.push_back(Vector3(x,y,z)); // hard copy  

                //m->MaxPoint()[0] = std::max(m->MaxPoint()[0], x);
                //m->MaxPoint()[1] = std::max(m->MaxPoint()[1], y);
                //m->MaxPoint()[2] = std::max(m->MaxPoint()[2], z);
                //m->MinPoint()[0] = std::min(m->MinPoint()[0], x);
                //m->MinPoint()[1] = std::min(m->MinPoint()[1], y);
                //m->MinPoint()[2] = std::min(m->MinPoint()[2], z);
            }            
            break;
        case 'f':
            wasFace = true;
            csi++;
            //count++;
            skipWhiteSpace(csi);
            temp_f.Index[0] = gathersti(csi)-1-vminus;
            if(m->m_bHasNormals) /*temp_f.nIndex[0] = */gathersti(csi);
            if(m->m_bHasTexture) temp_f.tIndex[0] = gathersti(csi)-1;
            skipWhiteSpace(csi);
            temp_f.Index[1] = gathersti(csi)-1-vminus;
            if(m->m_bHasNormals) /*temp_f.nIndex[1] = */gathersti(csi);
            if(m->m_bHasTexture) temp_f.tIndex[1] = gathersti(csi)-1;
            skipWhiteSpace(csi);
            temp_f.Index[2] = gathersti(csi)-1-vminus;
            if(m->m_bHasNormals) /*temp_f.nIndex[2] = */gathersti(csi);
            if(m->m_bHasTexture) temp_f.tIndex[2] = gathersti(csi)-1;

            //face.push_back(temp_f);
            m->getFaces()->push_back(temp_f); // push the information in by copy
            break;
        case 's':
            skipLine(csi, FFstring.end());
            break;
        case 'u':
            skipLine(csi, FFstring.end());
            break;
        default:
            if(wasFace)
            {
                count++;
                wasFace = false;
                // set up vertex
                for(unsigned int i = 0; i < vertice.size(); i++)
                {
                    Vertex v;
                    if(!m->m_bHasNormals)
                        v.set(vertice[i].X(), vertice[i].Y(), vertice[i].Z(),0,0,0);
                    else
                        v.set(vertice[i].X(), vertice[i].Y(), vertice[i].Z(), 
                            normal[i].X(), normal[i].Y(),normal[i].Z());
                    
                    m->getVertices()->push_back(v);

                }
                // need this to calculate normal!!!
                for(unsigned int i = 0; i < m->getFaces()->size(); i++)
                {
                    Triangle tt;
                    tt.setVertex0((*m->getVertices())[(*m->getFaces())[i].Index[0]]);
                    tt.setVertex1((*m->getVertices())[(*m->getFaces())[i].Index[1]]);
                    tt.setVertex2((*m->getVertices())[(*m->getFaces())[i].Index[2]]);
                    m->getTriangles()->push_back(tt);
                }
                vminus += (int)vertice.size();
                //m->centerModel(toCenter, scale, rAngle, rAxis);
                if(!m->m_bHasNormals) 
                {
                    m->calculateNormals();
                }

                //vector<Vertex>* pv = new std::vector<Vertex>(m->getCVertices());
                //m->getVerticesList()->push_front(pv); // push self in

                myList->push_back(m); // push into the list
                
                m = new DynamicModel(); // set up new space for next model
                normal.clear(); vertice.clear(); // clear the helper holder
                //if(count > 10)
                //    return myList;
            }
            skipLine(csi, FFstring.end());
            break;
        }
    }
    printf(".");
    return myList;
}
