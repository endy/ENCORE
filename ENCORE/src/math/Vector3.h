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

#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include "Point3.h"
#include <cmath>

#ifndef ENC_EPSILON
#define ENC_EPSILON 0.001
#endif

namespace encore
{
    /// Vector3 template
    ///
    /// A 3-dimentional vector class
    /// 
    class Vector3
	{
	public:

        Vector3(){ m_Components[0] = m_Components[1] = m_Components[2] = 1; }

		Vector3(float x, float y, float z)
        {
			m_Components[0] = x;
            m_Components[1] = y;
            m_Components[2] = z;
        }

		Vector3(const Point3& start, const Point3& end);

		Vector3(const Vector3& orig){ *this = orig; }

		~Vector3(){};

    public:  // accessors

        /// returns a const copy of the X component
		inline float X() const { return m_Components[0]; }
        /// returns a reference to the X component
		inline float& X(){ return m_Components[0]; }
        /// returns a const copy of the Y component
		inline float Y() const { return m_Components[1]; }
        /// returns a reference of the Y component
		inline float& Y(){ return m_Components[1]; }
        /// returns a const copy of the Z component
		inline float Z() const { return m_Components[2]; }
        /// returns a reference to the Z component
		inline float& Z(){ return m_Components[2]; }

    public:  // operators

        /// Assigns one vector to another of the same type
		Vector3& operator=(const Vector3& rhs);
		/// Sums vectors of the same type, returning the sum
		Vector3 operator+(const Vector3& rhs);
        /// Sums a vector of the same type and assigns it to the left-hand value
		Vector3& operator+=(const Vector3& rhs);
        /// Subtracts one vector from another of the same type, returning the difference
		Vector3 operator-(const Vector3& rhs);
        /// Subtracts one vector from another of the same type, assigning it to the left-hand value
		Vector3& operator-=(const Vector3& rhs);
        
        /// Compares two vectors. Returns true if they are equal, false otherwise
		bool operator==(const Vector3& rhs) const;
        
        float operator[](int i) const { return m_Components[i]; }
        float &operator[](int i) { return m_Components[i]; }

        // scalar operators //

        /// Multiplies the vector's components by the scalar rhs, returning the computed vector
        Vector3 operator*(const float& rhs);
        /// Multiplies the vector's components by the scalar and stores the new values in the vector
		Vector3& operator*=(const float& rhs);
        /// Divides the vector's components by the scalar rhs, returning the computed vector
        Vector3 operator/(const float& rhs);
        /// Divides the vector's components by the scalar and stores the new values in the vector
		Vector3& operator/=(const float& rhs);

		// conversion operators //

        /// cast the vector to a pointer to the 3-components
        operator float*(){ return m_Components; }

		// uninary operators //

        /// Invert the sign on the vector components
		Vector3 operator-();

    public: // useful methods

        /// Normalizes the vector
		void Normalize();
        
        /// Calculates and returns the magnitude
		float Magnitude();
        
        /// Calculates the magnitude squared
        /// floathis is useful for comparing magnitudes w/o the square-root calculation
		float MagnitudeSquared();
        
        // return a point structure w/ the x/y/z of this vector as the x/y/z of the point
        Point3 ToPoint(){ return Point3(this->X(), this->Y(), this->Z()); }
        
        /// Clamps vector components to zero if they are within some epsilon
        /// May not be necessary...
        void ClampfloatoZero();

    public: // friend methods

        /// Compute a dot product with another vector
        friend float Dot(const Vector3& v1, const Vector3& v2);

        /// Compute a cross product with another vector
        friend Vector3 Cross(const Vector3& v1, const Vector3& v2);

	private: // data

        /// float vector's components
        float m_Components[3];

  	};

        ///////////  Friend Function //////////////////////////////	 
    inline float Dot(const Vector3& v1, const Vector3& v2)
    {
        float product = v1[0] * v2[0] +
                        v1[1] * v2[1] +
                        v1[2] * v2[2];

        return product;
    }



    inline Vector3 Cross(const Vector3& v1, const Vector3& v2)
    {
        Vector3 product;

        // (u.y * v.z - u.z * v.y)i
        product.X() = v1.Y() * v2.Z() - v1.Z() * v2.Y();
        // -(u.x * v.z - u.z * v.x)j
        product.Y() = -1 * (v1.X() * v2.Z() - v1.Z() * v2.X());
        // (u.x * v.y - u.y * v.x)k
        product.Z() = v1.X() * v2.Y() - v1.Y() * v2.X();

        product.ClampfloatoZero();

        return product;
    }
}

#endif // _VECTOR3_H_

