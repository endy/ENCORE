#include "Renderer.h"


Renderer::Renderer(): 
    m_pScene(NULL), 
    m_pCamera(NULL),
    m_pAccelStruct(NULL)
{
    m_bImageComplete = false;
    m_bRefineInProgress = false;
}

Renderer::~Renderer()
{

}