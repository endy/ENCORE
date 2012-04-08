/****************************************************************************
Brandon Light
03/11/2006

Photon.cpp

****************************************************************************/

#include "Photon.h"

Photon::Photon()
{
    m_IsShadowPhoton = false;
}

Photon::Photon(Point3f in_position, Color in_power, Vector3f in_direction):
	m_position(in_position), m_power(in_power), m_incomingDir(in_direction), m_last(NONE)
{
    m_IsShadowPhoton = false;
}

Photon::~Photon()
{

}
