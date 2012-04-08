#include "Scene.h"
#include "PointLightPrim.h"
#include "RectangleLight.h"
#include "SpherePrim.h"
#include "ObjModelloader.h"

#include <algorithm>

using namespace std;

Scene::Scene( void )
{
}

Scene::Scene( std::string filename )
{
    load( filename );
}

Scene::~Scene( void )
{
    // cleanup the models
    std::list< IModel* >::iterator im;
    for ( im = m_pModelList.begin(); im != m_pModelList.end(); im++ )
    {   
        delete *im;
        *im = NULL;    
    }
    m_pModelList.clear();

    // cleanup the lights
    std::list< IEmissive* >::iterator il;
    for ( il = m_pLightList.begin(); il != m_pLightList.end(); il++ )
    {
        delete *il;
        *il = NULL; 
    }
    m_pLightList.clear();

}

std::vector<IEmissive*> Scene::GetLightVector()
{
    std::list<IEmissive*>::iterator iter = m_pLightList.begin();

    std::vector<IEmissive*> modelVector;

    while(iter != m_pLightList.end())
    {
        modelVector.push_back(*iter);
        iter++;
    }

    return modelVector;
}

std::list< Triangle* >* Scene::getNewTesselation ( void )
{
    std::list<Triangle*>::iterator itri;
    std::list<Triangle*>* ptris;
    std::list<Triangle*>* pTriangles = new std::list<Triangle*>;

    std::list< IModel* >::iterator im;
    for ( im = m_pModelList.begin(); im != m_pModelList.end(); im++ )
    {
        ptris = (*im)->getNewTesselation();
        for ( itri = ptris->begin(); itri != ptris->end(); itri++ )
        {
            pTriangles->push_back( *itri );
        }
        ptris->clear();
        delete ptris;
        ptris = NULL;
    }
    return pTriangles;
}

void Scene::HandleSceneVariable( std::string variable, std::fstream& file )
{
    if ( variable == "BACKGROUND_COLOR" )
    {
        float r,g,b,a;
        file >> r >> g >> b >> a;
        m_BackgroundColor = Color( r, g, b, a );
    }
    else if ( variable == "POINT_LIGHT" ) 
    {
        float x,y,z,r,g,b;
        file >> x >> y >> z >> r >> g >> b;
        PointLightPrim* pl = new PointLightPrim( x, y, z, r, g, b );
        m_pLightList.push_front( pl );
    }
    else if(variable == "RECT_LIGHT")
    {
        RectangleLight *newLightModel = new RectangleLight();
        RectangleLight *newLight = new RectangleLight();

        Color c;
        file >> c.R() >> c.G() >> c.B();

        newLightModel->SetEmissiveColor(c);
        newLight->SetEmissiveColor(c);

        Point3f center = Point3f();
        float scale = 0;
        float angle = 0;
        Vector3f rAxis = Vector3f();

        Material mat = m_currentMaterial;
        mat.SetEmissive(true);
        Model *m = new Model(center, scale, angle, rAxis, mat);
        
        m->AddPrimitive(newLightModel);

        m_pModelList.push_back(m);
        m_pLightList.push_back(newLight);
    }
    else if(variable == "MATERIAL")
    {
        Color diffuse, specular, ambient;
        file >> diffuse.R()  >> diffuse.G()  >> diffuse.B();
        file >> specular.R() >> specular.G() >> specular.B();
        file >> ambient.R()  >> ambient.G()  >> ambient.B();
        m_currentMaterial = Material(diffuse, specular, ambient);

        float reflectivity, transparency, refractionIndex, transmittance;
        file >> reflectivity >> transparency >> refractionIndex >> transmittance;
        m_currentMaterial.SetReflectivity(reflectivity);
        m_currentMaterial.SetTransparency(transparency);
        m_currentMaterial.SetRefractionIndex(refractionIndex);
        m_currentMaterial.SetTransimttance(transmittance);
    }
    else if(variable == "SPHERE")
    {
        Point3f center;
        float radius;
        file >> center.X() >> center.Y() >> center.Z() >> radius;
        SpherePrim *sphere = new SpherePrim(center, radius);

        float scale;
        float angle;
        Vector3f rAxis;

        file >> scale >> angle >> rAxis.X() >> rAxis.Y() >> rAxis.Z();

        Model *m = new Model(center, scale, angle, rAxis, m_currentMaterial);
        m->AddPrimitive(sphere);

        m_pModelList.push_back(m);
    }
    else if ( variable == "CAMERA" )
    {
        Point3f eye, look;
        Vector3f up;
        float ang, asp, nd, fd;
        file >> eye.X() >> eye.Y() >> eye.Z();
        file >> look.X() >> look.Y() >> look.Z();
        file >> up.X() >> up.Y() >> up.Z();
        file >> ang >> asp;
        file >> nd >> fd;
        m_Camera.set( eye, look, up );
        m_Camera.SetViewVolume( ang, asp, nd, fd );
    }
    else if ( variable == "MODEL" )
    {
        Point3f c;
        float scale;
        float angle;
        Vector3f rAxis;
        std::string filename = "";
        file >> filename;
        file >> c.X() >> c.Y() >> c.Z() >> scale;
        file >> angle >> rAxis.X() >> rAxis.Y() >> rAxis.Z();

        Model* m = new Model( filename, c, scale, angle, rAxis, m_currentMaterial );
        m_pModelList.push_back( m );
    }
    else
    {
        // we don't understand what the command is, so display an error message
        printf( "Scene Parser does not understand '%s', please comment out that line.\n", variable.c_str() );
    }
}


void Scene::load( std::string filename )
{
    std::string var;
    char temp[1000];

    fstream file;
    file.open( filename.c_str(), fstream::in );
    if ( !file.good() )
    {
        cout << "Can't read scene file" << endl;
        exit(1);
    }
    while ( !file.eof() )
    {
        file >> var;
        if (var.at(0) == '*'){ // this line is a comment
            //file.ignore(1000,'*');
            file.getline(temp, 1000);
        }
        else
        {  // a command
            HandleSceneVariable( var, file );
        }
    }

    file.close();
}
