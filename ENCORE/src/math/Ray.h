/****************************************************************************
Brandon Light
12/04/2005

Ray.h  
  
A basic ray class

****************************************************************************/

#pragma once 

#include "TPoint3.h"
#include "TVector3.h"

namespace encore 
{
  class Ray
  {
  public:
		Ray();
		Ray(Point3f origin, Vector3f direction);
        Ray(Point3f origin, Vector3f direction, int dbgX, int dbgY);
        virtual ~Ray();

		// accessors
        inline Point3f& Origin(){ return m_Origin; }
		inline Vector3f& Direction(){ return m_Direction; }

        Point3f GetPositionAtTime(float time);

        Point3f GetTracePosition(){ return m_TracePosition; }
        void    SetTracePosition(Point3f position);

        float GetTraceTime(){ return m_TraceTime; }
        void  SetTraceTime(float time);

        int IncrementRecurseLevel(){ return ++m_cRecurseLevel;}

  public:
    int m_dbgX;
    int m_dbgY;

  private:
    Point3f  m_Origin;
    Vector3f m_Direction;

    Point3f m_TracePosition;
    float   m_TraceTime;

    int     m_cRecurseLevel;
  };

}




