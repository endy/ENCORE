#include "ObjLoader.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

/**********************************************************************/
// helper functions
void Objloader::skipLine(string::const_iterator cit, string::const_iterator cend)
{
    while(*cit != '\n' && cit != cend)
        cit++;
    if(cit == cend)
        return;
    cit++;
}

void Objloader::skipWhiteSpace(string::const_iterator &cit)
{
    while(*cit == ' ')
        cit++;
}

// turn a continues string into float sperated by space and endline
float Objloader::gatherstf(string::const_iterator &cit)
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

// turn a continues string into float sperated by space and endline
int Objloader::gathersti(string::const_iterator &cit)
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
Objloader::Objloader(const string &fname)
{
    hasNorm = hasTex = 0;
    load(fname);
}

void Objloader::load(const string &fname)
{
    FILE *f = fopen(fname.c_str(), "rb");
    if(!f)
    {
        printf("Unable to open obj file %s\n",fname.c_str());
        return;
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
    
    string::const_iterator csi; // constant string iterator
    hasNorm = hasTex = false;
    
    float x,y,z; // temp number to hold vertex, normal or texel
    TFace temp_f;// temp face for holding

    csi = FFstring.begin();
    while(csi != FFstring.end())
    {
        switch(*csi)
        {
        case '#':
            skipLine(csi, FFstring.end());
            break;
        case 'v':
            csi++;
            if(*csi == 'n')
            {
                hasNorm = true;
                csi++;
                skipWhiteSpace(csi);
                x = gatherstf(csi);
                skipWhiteSpace(csi);
                y = gatherstf(csi);
                skipWhiteSpace(csi);
                z = gatherstf(csi);
                normal.push_back(Vector3f(x,y,z)); // add normal data
            }
            else if(*csi == 't')
            {
                hasTex = true;
                csi++;
                skipWhiteSpace(csi);
                x = gatherstf(csi);
                skipWhiteSpace(csi);
                y = gatherstf(csi);
                skipWhiteSpace(csi);
                z = gatherstf(csi);
                texcoord.push_back(Vector3f(x,y,z)); // hard copy
            }
            else
            {
                skipWhiteSpace(csi);
                x = gatherstf(csi);
                skipWhiteSpace(csi);
                y = gatherstf(csi);
                skipWhiteSpace(csi);
                z = gatherstf(csi);
                vertice.push_back(Vector3f(x,y,z)); // hard copy                
            }            
            break;
        case 'f':
            csi++;
            skipWhiteSpace(csi);
            temp_f.vIndex[0] = gathersti(csi)-1;
            if(hasNorm) temp_f.nIndex[0] = gathersti(csi)-1;
            if(hasTex) temp_f.tIndex[0] = gathersti(csi)-1;
            skipWhiteSpace(csi);
            temp_f.vIndex[1] = gathersti(csi)-1;
            if(hasNorm) temp_f.nIndex[1] = gathersti(csi)-1;
            if(hasTex) temp_f.tIndex[1] = gathersti(csi)-1;
            skipWhiteSpace(csi);
            temp_f.vIndex[2] = gathersti(csi)-1;
            if(hasNorm) temp_f.nIndex[2] = gathersti(csi)-1;
            if(hasTex) temp_f.tIndex[2] = gathersti(csi)-1;

            index.push_back(temp_f); // push the information in by copy
            break;
        default:
            skipLine(csi, FFstring.end());
            break;
        }
    }
    printf(".");
}
