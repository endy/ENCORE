/****************************************************************************
Brandon Light
03/11/2006

Point3.h

A 3-dimentional point template class

****************************************************************************/

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

	Point3& Point3::operator=(const Point3& rhs)
	{
		if(this == &rhs)
		{
			return *this;
		}

		memcpy(m_Components, rhs.m_Components, 3*sizeof(float));
  
		return *this;
	}



    Point3 Point3::operator+(const Point3& rhs)
    {
        Point3 sum;
        sum.X() = this->X() + rhs.X();
        sum.Y() = this->Y() + rhs.Y();
        sum.Z() = this->Z() + rhs.Z();

        return sum;
    }

    Point3 Point3::operator-(const Point3& rhs)
    {
        Point3 difference;
        difference.X() = this->X() - rhs.X();
        difference.Y() = this->Y() - rhs.Y();
        difference.Z() = this->Z() - rhs.Z();

        return difference;
    }

    bool Point3::operator==(const Point3& rhs)
	{
		return this->X() == rhs.X() &&
			   this->Y() == rhs.Y() &&
			   this->Z() == rhs.Z();
	}

	Point3 Point3::operator*(const float& rhs)
	{
		Point3 product = *this;
		product *= rhs;
		return product;
	}

	Point3& Point3::operator*=(const float& rhs)
	{
		this->X() *= rhs;
		this->Y() *= rhs;
		this->Z() *= rhs;
		return *this;
	}

	Point3 Point3::operator/(const float& rhs)
	{
		Point3 quotient = *this;
		quotient /= rhs;
		return quotient;
	}

	Point3& Point3::operator/=(const float& rhs)
	{
		this->X() /= rhs;
		this->Y() /= rhs;
		this->Z() /= rhs;
		return *this;
	}
}

#endif // _POINT3_H_


