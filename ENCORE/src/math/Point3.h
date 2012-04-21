//////////////////////////////////////////////////////////////////////////////////////////////////
///
///     ENCORE
///
///     Copyright 2006, 2012, Brandon Light
///     All rights reserved.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief 3-Dimensional Point Class
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _POINT3_H_
#define _POINT3_H_

#include <cassert>
#include <memory>
#include <cstring>

namespace encore
{
  class Point3
  {
   public:
		// constructors
		Point3(){ m_Components[0] = m_Components[1] = m_Components[2] = 0; }

		Point3(float x, float y, float z)
        {
			m_Components[0] = x;
            m_Components[1] = y;
            m_Components[2] = z;
        }

		// copy constructor
		Point3(const Point3& orig){ *this = orig; }
      
		// accessors
		inline const float X() const { return m_Components[0]; }
		inline float& X(){ return m_Components[0]; }

		inline float Y() const { return m_Components[1]; }
		inline float& Y(){ return m_Components[1]; }

		inline float Z() const { return m_Components[2]; }
		inline float& Z(){ return m_Components[2]; }

		//////  OPERAfloatORS  //////////////////////////

		Point3& operator=(const Point3& rhs);

        Point3 operator+(const Point3& rhs);
        Point3 operator-(const Point3& rhs);

		bool operator==(const Point3& rhs);

		// scalar operators
		Point3 operator*(const float& rhs);
		Point3& operator*=(const float& rhs);
		Point3 operator/(const float& rhs);
		Point3& operator/=(const float& rhs);

        float operator[](int i) const { assert((0 <= i)&&(i < 3)); return m_Components[i]; }
        float &operator[](int i) { assert((0 <= i)&&(i < 3)); return m_Components[i]; }

		// conversion operator
        operator float*(){ return m_Components; }

		private:
			float m_Components[3];
	};
}

#endif // _POINT3_H_


