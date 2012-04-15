/****************************************************************************
Brandon Light
12/04/2005

Ray.h  
  
A basic ray class

****************************************************************************/

#pragma once 

#include "Vector3.h"

namespace encore 
{
  class Ray
  {
  public:
		Ray();
		Ray(Point3 origin, Vector3 direction);
        Ray(Point3 origin, Vector3 direction, int dbgX, int dbgY);
        virtual ~Ray();

		// accessors
        inline Point3& Origin(){ return m_Origin; }
		inline Vector3& Direction(){ return m_Direction; }

        Point3 GetPositionAtTime(float time);

        Point3 GetTracePosition(){ return m_TracePosition; }
        void    SetTracePosition(Point3 position);

        float GetTraceTime(){ return m_TraceTime; }
        void  SetTraceTime(float time);

        int IncrementRecurseLevel(){ return ++m_cRecurseLevel;}

  public:
    int m_dbgX;
    int m_dbgY;

  private:
    Point3  m_Origin;
    Vector3 m_Direction;

    Point3 m_TracePosition;
    float   m_TraceTime;

    int     m_cRecurseLevel;
  };

}




