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
        m_Origin = Point3f();
        m_Direction = Vector3f();

        m_TracePosition = m_Origin;
        m_TraceTime = 0;

        m_cRecurseLevel = 0;
    }

    Ray::Ray(Point3f origin, Vector3f direction)
    {
        m_Origin = origin;
        m_Direction = direction;

        m_TracePosition = m_Origin;
        m_TraceTime = 0;

        m_cRecurseLevel = 0;
    }

    Ray::Ray(Point3f origin, Vector3f direction, int dbgX, int dbgY)
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

	Point3f Ray::GetPositionAtTime(float time)
	{
		Point3f point;

		point.X() = m_Origin.X() + m_Direction.X() * time;
		point.Y() = m_Origin.Y() + m_Direction.Y() * time;
		point.Z() = m_Origin.Z() + m_Direction.Z() * time;

		return point;
	}

    void Ray::SetTracePosition(Point3f position)
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

