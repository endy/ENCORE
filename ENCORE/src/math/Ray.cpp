/****************************************************************************
Brandon Light
12/10/2005

Ray.cpp

****************************************************************************/

#include "Ray.h"

namespace encore
{
    Ray::Ray()
    {
        m_Origin = Point3();
        m_Direction = Vector3();

        m_TracePosition = m_Origin;
        m_TraceTime = 0;

        m_cRecurseLevel = 0;
    }

    Ray::Ray(Point3 origin, Vector3 direction)
    {
        m_Origin = origin;
        m_Direction = direction;

        m_TracePosition = m_Origin;
        m_TraceTime = 0;

        m_cRecurseLevel = 0;
    }

    Ray::Ray(Point3 origin, Vector3 direction, int dbgX, int dbgY)
    {
        m_Origin = origin;
        m_Direction = direction;

        m_TracePosition = m_Origin;
        m_TraceTime = 0;

        m_cRecurseLevel = 0;

        m_dbgX = dbgX;
        m_dbgY = dbgY;
    }

    Ray::~Ray()
    {

    }

	Point3 Ray::GetPositionAtTime(float time)
	{
		Point3 point;

		point.X() = m_Origin.X() + m_Direction.X() * time;
		point.Y() = m_Origin.Y() + m_Direction.Y() * time;
		point.Z() = m_Origin.Z() + m_Direction.Z() * time;

		return point;
	}

    void Ray::SetTracePosition(Point3 position)
    {
        m_TracePosition = position;
    }

    void Ray::SetTraceTime(float time)
    {
        m_TraceTime = time;
        
        // TODO: Figure out what current time and current position coorespond to ... -Brandon
        //m_CurrentPosition = GetPositionAtTime(m_CurrentTime);
    }
}

