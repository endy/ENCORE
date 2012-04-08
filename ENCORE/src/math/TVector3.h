//////////////////////////////////////////////////////////////////////////
// ENCORE PROJECT
// 
// Project description and license goes here.
// 
// Developed By
//      Chen-Hao Chang:  chocobo7@WPI.EDU
//      Brandon Light:   brandon@alum.wpi.edu
//      Peter Lohrman:   plohrmann@alum.WPI.EDU
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "TPoint3.h"
#include <cmath>

#ifndef ENC_EPSILON
#define ENC_EPSILON 0.001
#endif

namespace encore
{
    /// TVector3 template
    ///
    /// A 3-dimentional vector class
    ///
	template <class T> 
		class TVector3
	{
	public:

        TVector3<T>(){ m_Components[0] = m_Components[1] = m_Components[2] = 1; }

		TVector3<T>(T x, T y, T z)
        {
			m_Components[0] = x;
            m_Components[1] = y;
            m_Components[2] = z;
        }

		TVector3<T>(TPoint3<T> start, TPoint3<T> end);

		TVector3<T>(const TVector3<T>& orig){ *this = orig; }

		~TVector3<T>(){};

    public:  // accessors

        /// returns a const copy of the X component
		inline T X() const { return m_Components[0]; }
        /// returns a reference to the X component
		inline T& X(){ return m_Components[0]; }
        /// returns a const copy of the Y component
		inline T Y() const { return m_Components[1]; }
        /// returns a reference of the Y component
		inline T& Y(){ return m_Components[1]; }
        /// returns a const copy of the Z component
		inline T Z() const { return m_Components[2]; }
        /// returns a reference to the Z component
		inline T& Z(){ return m_Components[2]; }

    public:  // operators

        /// Assigns one vector to another of the same type
		TVector3<T>& operator=(const TVector3<T>& rhs);
		/// Sums vectors of the same type, returning the sum
		TVector3<T> operator+(const TVector3<T>& rhs);
        /// Sums a vector of the same type and assigns it to the left-hand value
		TVector3<T>& operator+=(const TVector3<T>& rhs);
        /// Subtracts one vector from another of the same type, returning the difference
		TVector3<T> operator-(const TVector3<T>& rhs);
        /// Subtracts one vector from another of the same type, assigning it to the left-hand value
		TVector3<T>& operator-=(const TVector3<T>& rhs);
        
        /// Compares two vectors. Returns true if they are equal, false otherwise
		bool operator==(const TVector3<T>& rhs);
        
        float operator[](int i) const { return m_Components[i]; }
        float &operator[](int i) { return m_Components[i]; }

        // scalar operators //

        /// Multiplies the vector's components by the scalar rhs, returning the computed vector
        TVector3<T> operator*(const T& rhs);
        /// Multiplies the vector's components by the scalar and stores the new values in the vector
		TVector3<T>& operator*=(const T& rhs);
        /// Divides the vector's components by the scalar rhs, returning the computed vector
        TVector3<T> operator/(const T& rhs);
        /// Divides the vector's components by the scalar and stores the new values in the vector
		TVector3<T>& operator/=(const T& rhs);

		// conversion operators //

        /// cast the vector to a pointer to the 3-components
        operator T*(){ return m_Components; }

		// uninary operators //

        /// Invert the sign on the vector components
		TVector3<T> operator-();

    public: // useful methods

        /// Normalizes the vector
		void Normalize();
        
        /// Calculates and returns the magnitude
		T Magnitude();
        
        /// Calculates the magnitude squared
        /// This is useful for comparing magnitudes w/o the square-root calculation
		T MagnitudeSquared();
        
        // return a point structure w/ the x/y/z of this vector as the x/y/z of the point
        TPoint3<T> ToPoint(){ return TPoint3<T>(this->X(), this->Y(), this->Z()); }
        
        /// Clamps vector components to zero if they are within some epsilon
        /// May not be necessary...
        void ClampToZero();

    public: // friend methods

        /// Compute a dot product with another vector
#ifdef WIN32
		template <class T> 
#else
        template <T>
#endif
        friend T Dot(const TVector3<T>& v1, const TVector3<T>& v2);

        /// Compute a cross product with another vector
#ifdef WIN32
		template <class T>
#else
        template <T>
#endif
        friend TVector3<T> Cross(const TVector3<T>& v1, const TVector3<T>& v2);

	private: // data

        /// The vector's components
        T m_Components[3];

  	};

	// construct a TVector3 from two 3-dimensional points
	//   vector = end - start
	template <class T> TVector3<T>::TVector3(TPoint3<T> start, TPoint3<T> end)
	 {
		this->X() = end.X() - start.X();
		this->Y() = end.Y() - start.Y();
		this->Z() = end.Z() - start.Z();
	}

	template <class T> TVector3<T>& TVector3<T>::operator=(const TVector3<T>& rhs)
	{
		if(this == &rhs){ return *this; }

        memcpy(m_Components, rhs.m_Components, 3*sizeof(T));
  
		return *this;
	}

	template <class T> TVector3<T> TVector3<T>::operator+(const TVector3<T>& rhs)
	{
		TVector3<T> sum = *this;
		sum += rhs;
		return sum;
	}
	template <class T> TVector3<T>& TVector3<T>::operator+=(const TVector3<T>& rhs)
	{
		this->X() += rhs.X();
		this->Y() += rhs.Y();
		this->Z() += rhs.Z();
		return *this;
	}

	template <class T> TVector3<T> TVector3<T>::operator-(const TVector3<T>& rhs)
	{
		TVector3<T> difference = *this;
		difference -= rhs;
		return difference;
	}
	template <class T> TVector3<T>& TVector3<T>::operator-=(const TVector3<T>& rhs)
	{
		this->X() -= rhs.X();
		this->Y() -= rhs.Y();
		this->Z() -= rhs.Z();
		return *this;
	}

    template <class T> TVector3<T> TVector3<T>::operator*(const T& rhs)
    {
        TVector3<T> product = *this;
		product *= rhs;
        return product;
    }
	template <class T> TVector3<T>& TVector3<T>::operator*=(const T& rhs)
	{
		this->X() *= rhs;
		this->Y() *= rhs;
		this->Z() *= rhs;
		return *this;
	}

    template <class T> TVector3<T> TVector3<T>::operator/(const T& rhs)
    {
        TVector3<T> quotient = *this;
		quotient /= rhs;
        return quotient;
    }

	template <class T> TVector3<T>& TVector3<T>::operator/=(const T& rhs)
	{
		this->X() /= rhs;
		this->Y() /= rhs;
		this->Z() /= rhs;
		return *this;
	}

	template <class T> bool TVector3<T>::operator==(const TVector3<T>& rhs)
	{
		return this->X() == rhs.X() &&
			   this->Y() == rhs.Y() &&
			   this->Z() == rhs.Z();
	}

	template <class T> TVector3<T> TVector3<T>::operator-()
	{
		return TVector3<T>(
			-this->X(),
			-this->Y(),
			-this->Z()
			);
	}

    template <class T> void TVector3<T>::Normalize()
	{
		T magnitude = this->Magnitude();

		this->X() = this->X() / magnitude;
		this->Y() = this->Y() / magnitude;
		this->Z() = this->Z() / magnitude;

        ClampToZero();
	}

	template <class T> T TVector3<T>::Magnitude()
	{ 
		return (T) std::sqrt(MagnitudeSquared());
	}

	template <class T> T TVector3<T>::MagnitudeSquared()
	{
		return (this->X() * this->X()) + (this->Y() * this->Y()) + (this->Z() * this->Z());
	}

    template <class T> void TVector3<T>::ClampToZero()
    {
        if(-ENC_EPSILON < this->X() && this->X() < ENC_EPSILON)
        {
            this->X() = 0;
        }
        if(-ENC_EPSILON < this->Y() && this->Y() < ENC_EPSILON)
        {
            this->Y() = 0;
        }
        if(-ENC_EPSILON < this->Z() && this->Z() < ENC_EPSILON)
        {
            this->Z() = 0;
        }
    }

	/// floating point Vector3
	typedef TVector3<float>  Vector3f;
    /// double-precision Vector3
	typedef TVector3<double> Vector3d;
    /// floating point Normal3
	typedef TVector3<float>  Normal3f;
    /// double-precision Normal3
	typedef TVector3<double> Normal3d;


	///////////  Friend Function //////////////////////////////

	template <class T> 
		T Dot(const TVector3<T>& v1, const TVector3<T>& v2)
	{
		T product = v1.X() * v2.X() +
					v1.Y() * v2.Y() +
					v1.Z() * v2.Z();
        
		return product;
	}


	template <class T> 
		TVector3<T> Cross(const TVector3<T>& v1, const TVector3<T>& v2)
	{
		TVector3<T> product;

		// (u.y * v.z - u.z * v.y)i
		product.X() = v1.Y() * v2.Z() - v1.Z() * v2.Y();
		// -(u.x * v.z - u.z * v.x)j
		product.Y() = -1 * (v1.X() * v2.Z() - v1.Z() * v2.X());
		// (u.x * v.y - u.y * v.x)k
		product.Z() = v1.X() * v2.Y() - v1.Y() * v2.X();

        product.ClampToZero();

		return product;
	}
}
