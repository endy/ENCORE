#ifndef _SCENE_H
#define _SCENE_H

#include <string>
#include <stdio.h>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>

#include "Emissive.h"
#include "IModel.h"

using encore::Color;
using encore::IEmissive;

#include "Camera.h"

class Scene
{
public:
	Scene( void );
    Scene( std::string filename );
	virtual ~Scene( void );

	virtual void load( std::string filename );

    std::list< IModel* > getModelList( void ) { return m_pModelList; }
    std::list< IEmissive* >* getLightList( void ) { return &m_pLightList; }

    std::vector<IEmissive*> GetLightVector();

    std::list< Triangle* >* getNewTesselation ( void );

    Camera& getCamera( void ) { return m_Camera; }

    Color& getBackgroundColor( void ) { return m_BackgroundColor; }

protected:
    void HandleSceneVariable( std::string variable, std::fstream& file );

    Camera m_Camera;
	std::list< IModel* > m_pModelList;
	std::list< IEmissive*  > m_pLightList;
    Color m_BackgroundColor;

    Material m_currentMaterial; // used to load models 
};
#endif
