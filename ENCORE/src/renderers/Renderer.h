//////////////////////////////////////////////////////////////////////////
// ENCORE PROJECT
// 
// Project description and license goes here.
// 
// Developed By
//      Chen-Hao Chang:  chocobo7@WPI.EDU
//      Brandon Light:   brandon@alum.wpi.edu
//      Peter Lohrman:   plohrmann@alum.WPI.EDU
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "AccelerationStructure.h"
#include "Scene.h"
#include "Camera.h"
#include <string>
#include "Options.h"

// forward declarations
class Scene;
class AccelerationStructure;
class Camera;

/// Renderer Class
///
/// Defines the interface for renderers in the ENCORE software
///
class Renderer
{
public: 

    Renderer();
    virtual ~Renderer();

public: 
    /// Prints the usage information for Renderer to STDOUT
    virtual void usage( void ) = 0;
    /// Configures the renderer's properties
    virtual void configure( Options* l_pOptions ) = 0;
    /// Initialize the renderer.  Must be called before calling render()
    virtual void init( Scene* l_pScene, AccelerationStructure* l_pAccelStruct, Camera* l_pCamera ) = 0;
    /// Render a single frame
    virtual void render( void ) = 0;
    /// Deinitializes the renderer
    virtual void deinit( void ) = 0;
    /// Keyboard input handler
    virtual void keyboard( unsigned char key ) = 0;

public:
    /// Improves the image until completion
    virtual void Refine(){ render(); }
    /// Returns whether the image has been refined completely
    bool IsImageComplete(){ return m_bImageComplete; }
    /// Returns whether the renderer is in the process of rendering an image
    bool IsRefineInProgress(){ return m_bRefineInProgress; }

protected:
    
    /// A pointer to the scene to be rendered
    Scene* m_pScene;
    /// A pointer to the camera positioned in the scene
    Camera* m_pCamera;
    /// A pointer to the acceleration structure, built using the scene pointed to by m_pScene
    AccelerationStructure* m_pAccelStruct;

    bool m_bRefineInProgress;
    bool m_bImageComplete;
};



