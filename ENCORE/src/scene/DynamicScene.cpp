#include "DynamicScene.h"
#include "ObjModelloader.h"

#ifndef WIN32
#define Sleep(x) 0
#endif

DynamicScene::DynamicScene(void)
: m_FrameTime(-1), m_pDynamicModelList(0)
{
}

DynamicScene::~DynamicScene(void)
{
    // the models will be cleaned up by the parent Scene class
    // so just clear the list here

    if(m_pDynamicModelList)
        m_pDynamicModelList->clear();

}

void DynamicScene::UpdateFrame(float time)
{
    std::list< DynamicModel* >::iterator mit;

    if(m_pDynamicModelList)
    {
        for( mit = m_pDynamicModelList->begin(); mit != m_pDynamicModelList->end(); mit++ )
        {
            (*mit)->getNextFrame(); //time);
        }
    }
}

void DynamicScene::load( std::string filename )
{
    std::string var;
    char temp[1000];
    fstream file;
    file.open( filename.c_str(), fstream::in );
    if ( !file.good() )
    {
        cout << "Can't read scene file" << endl;
        Sleep( 5000 );
        exit(1);
    }
    while ( !file.eof() )
    {
        file >> var;

        if (var.at(0) == '*')
        {
            // this line is a comment

            //file.ignore(1000,'*');
            file.getline(temp, 1000);
        }
        else
        {  // a command
            if(var == "DYNAMIC")
            {
#ifdef WIN32
                string foldername;
                file >> foldername;
                // under program executable folder + foldername
                foldername = ".\\" + foldername + "\\";
                // object are sorted in alphabetical order
                std::vector<string> objFiles;
                unsigned int frames_count = 0;
                WIN32_FIND_DATA ffd;
                HANDLE h = ::FindFirstFile((foldername + "*.obj").c_str(), &ffd);
                objFiles.push_back(foldername+string(ffd.cFileName));
                while (::FindNextFile(h, &ffd))
                    objFiles.push_back(foldername+string(ffd.cFileName));
                ::FindClose(h);
                frames_count = (unsigned int)objFiles.size();
                
                // read in the model relocation information
                Point3f c;
                float scale = -1;
                float angle = -1;
                Vector3f rAxis;
                int frames;
                file >> frames;
                file >> c[0] >> c[1] >> c[2] >> scale;
                file >> angle >> rAxis[0] >> rAxis[1] >> rAxis[2];
                
                ObjModelloader modelloader;
                modelloader.setTransform(c, scale, angle, rAxis);
                //list<DynamicModel*>* test;
                
                // load each of the files
                std::vector<string>::const_iterator it = objFiles.begin();
                for(int i = 0; i < frames && it != objFiles.end(); it++, i++)
                {
                    if(i == 0)
                    {
                        // loading first frame
                        // load the base model into the list
                        m_pDynamicModelList = &modelloader.loadmulti(*it);
                        //test = modelloader.loadmulti(*it);
                    }
                    else
                    {
                        // loading following frames
                        // load keyFrames of each model to the list
                        list<DynamicModel*> pl = modelloader.loadmulti(*it);
                        list<DynamicModel*>::iterator mit;
                        list<DynamicModel*>::iterator dmit;
                        for(mit = m_pDynamicModelList->begin(), dmit = pl.begin(); mit != m_pDynamicModelList->end(), dmit != pl.end(); mit++, dmit++)
                        {
                            // adds the keyFrame model to each model in the list
                            (*mit)->add(*dmit);
                        }
                        pl.clear();
//                        delete pl;
                    }
                }
                
#endif
            }
            else
            {
                // let Scene parent class handle it
                HandleSceneVariable( var, file );
            }
        }
    }

    // add dynamic models to model list
    std::list< DynamicModel* >::iterator dmit;

    if(m_pDynamicModelList)
    {
        for ( dmit = m_pDynamicModelList->begin(); dmit != m_pDynamicModelList->end(); dmit++ )
        {
            (*dmit)->resetFrame();
            m_pModelList.push_back( *dmit );
        }
    }

    file.close();
}