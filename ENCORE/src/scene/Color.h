/****************************************************************************
Brandon Light
03/13/2006

Color.h

Simple struct for storing color values

****************************************************************************/
#pragma once 

#include "TVector3.h"

using encore::Vector3f;

namespace encore
{
    /// Color class
    ///
    /// Represents a RGBA tuple
    ///
	class Color
	{
    public:
        static Color One(){ return Color(1.0f, 1.0f, 1.0f); }
        static Color Zero(){ return Color(0.0f, 0.0f, 0.0f); }

	public:
		
        Color();
        Color(const Color& c);
        Color(float r, float g, float b, float a = 1.0f);
		
        ~Color();

    public: // accessors

        /// returns a reference to the red color component
        float& R(){ return m_Power[0]; }
        /// returns a const reference to the red color component
        const float& R() const { return m_Power[0]; }
        /// returns a reference to the green color component
		float& G(){ return m_Power[1]; }
        /// returns a const reference to the green color component
        const float& G() const { return m_Power[1]; }
        /// returns a reference to the blue color component
		float& B(){ return m_Power[2]; }
        /// returns a const reference to the blue color component
        const float& B() const { return m_Power[2]; }
        /// returns a reference to the alpha component
        float& A(){ return m_Power[3]; }
        /// returns a const reference to the alpha component
        const float& A() const { return m_Power[3]; }

    public: // operators

        /// assigns rhs to the left-hand side of the assignment operator
        Color& operator=(const Color& rhs);
        /// returns the sum of two colors
		Color operator+(const Color& rhs);
        ///
        Color&  operator+=(const Color& rhs); 
        ///
		Color operator-(const Color& rhs);
        ///
        Color&  operator-=(const Color& rhs);
        ///
        Color operator*(const Color& rhs);
        ///
        Color&  operator*=(const Color& rhs);
        ///
        Color operator*(const float& rhs);
        ///
        Color&  operator*=(const float& rhs);
        ///
        Color operator/(const float& rhs);
        ///
        Color& operator/=(const float& rhs);
        ///
        Color operator-();
        ///
        operator float*(){ return m_Power; }

    public: // general methods

        ///
        float TotalIntensity();

        ///
        void Clamp(float cMin, float cMax);

	private:

        /// RGBA tuple stored as a 4-float array
		float m_Power[4];

	};
}
