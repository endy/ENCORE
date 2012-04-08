#pragma once
#include "Scene.h"

// forward declaration
class DynamicModel;

class DynamicScene :
    public Scene
{
public:
    DynamicScene(void);
    virtual ~DynamicScene(void);

	virtual void load( std::string filename );
    void UpdateFrame( float time );

    std::list< DynamicModel* >* GetDynamicModelList( void ) { return m_pDynamicModelList; }

private:
    std::list< DynamicModel* >* m_pDynamicModelList;
    float m_FrameTime;
};
