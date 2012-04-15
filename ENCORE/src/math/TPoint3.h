/****************************************************************************
Brandon Light
03/11/2006

TPoint3.h

A 3-dimentional point template class

****************************************************************************/

#pragma once

#include <cassert>
#include <memory>


namespace encore
{
  template <class T> class TPoint3
  {
   public:
		// constructors
		TPoint3<T>(){ m_Components[0] = m_Components[1] = m_Components[2] = 0; }

		TPoint3<T>(T x, T y, T z)
        {
			m_Components[0] = x;
            m_Components[1] = y;
            m_Components[2] = z;
        }

		// copy constructor
		TPoint3<T>(const TPoint3<T>& orig){ *this = orig; }
      
		// accessors
		inline const T X() const { return m_Components[0]; }
		inline T& X(){ return m_Components[0]; }

		inline T Y() const { return m_Components[1]; }
		inline T& Y(){ return m_Components[1]; }

		inline T Z() const { return m_Components[2]; }
		inline T& Z(){ return m_Components[2]; }

		//////  OPERATORS  //////////////////////////

		TPoint3<T>& operator=(const TPoint3<T>& rhs);

        TPoint3<T> operator+(const TPoint3<T>& rhs);
        TPoint3<T> operator-(const TPoint3<T>& rhs);

		bool operator==(const TPoint3<T>& rhs);

		// scalar operators
		TPoint3<T> operator*(const T& rhs);
		TPoint3<T>& operator*=(const T& rhs);
		TPoint3<T> operator/(const T& rhs);
		TPoint3<T>& operator/=(const T& rhs);

        float operator[](int i) const { assert((0 <= i)&&(i < 3)); return m_Components[i]; }
        float &operator[](int i) { assert((0 <= i)&&(i < 3)); return m_Components[i]; }

		// conversion operator
        operator T*(){ return m_Components; }

		private:
			T m_Components[3];
	};

	template <class T> TPoint3<T>& TPoint3<T>::operator=(const TPoint3<T>& rhs)
	{
		if(this == &rhs)
		{
			return *this;
		}

		memcpy(m_Components, rhs.m_Components, 3*sizeof(T));
  
		return *this;
	}



     template <class T>  TPoint3<T> TPoint3<T>::operator+(const TPoint3<T>& rhs)
    {
        TPoint3<T> sum;
        sum.X() = this->X() + rhs.X();
        sum.Y() = this->Y() + rhs.Y();
        sum.Z() = this->Z() + rhs.Z();

        return sum;
    }

    template <class T> TPoint3<T> TPoint3<T>::operator-(const TPoint3<T>& rhs)
    {
        TPoint3<T> difference;
        difference.X() = this->X() - rhs.X();
        difference.Y() = this->Y() - rhs.Y();
        difference.Z() = this->Z() - rhs.Z();

        return difference;
    }

	template <class T> bool TPoint3<T>::operator==(const TPoint3<T>& rhs)
	{
		return this->X() == rhs.X() &&
			   this->Y() == rhs.Y() &&
			   this->Z() == rhs.Z();
	}

	template <class T> TPoint3<T> TPoint3<T>::operator*(const T& rhs)
	{
		TPoint3<T> product = *this;
		product *= rhs;
		return product;
	}
	template <class T> TPoint3<T>& TPoint3<T>::operator*=(const T& rhs)
	{
		this->X() *= rhs;
		this->Y() *= rhs;
		this->Z() *= rhs;
		return *this;
	}
	template <class T> TPoint3<T> TPoint3<T>::operator/(const T& rhs)
	{
		TPoint3<T> quotient = *this;
		quotient /= rhs;
		return quotient;
	}
	template <class T> TPoint3<T>& TPoint3<T>::operator/=(const T& rhs)
	{
		this->X() /= rhs;
		this->Y() /= rhs;
		this->Z() /= rhs;
		return *this;
	}

  	// standard point types
	typedef TPoint3<float> Point3f;
}



