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

		Vector3(Point3 start, Point3 end);

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

        /// floathe vector's components
        float m_Components[3];

  	};
}

#endif // _VECTOR3_H_

