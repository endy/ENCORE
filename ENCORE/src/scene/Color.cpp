

#include "Color.h"
#include <memory>

namespace encore
{
	Color::Color()	
	{
        m_Power[0] = 0.0f;
        m_Power[1] = 0.0f;
        m_Power[2] = 0.0f;
        m_Power[3] = 1.0f;
	}

    Color::Color(const Color& c)
    {
        *this = c;
    }

    Color::Color(float r, float g, float b, float a /* = 1.0f */)
    {
        m_Power[0] = r;
        m_Power[1] = g;
        m_Power[2] = b;
        m_Power[3] = a;
    }

	Color::~Color()
	{

	}

    Color& Color::operator=(const Color& rhs)
    {
        // check assignment to self
        if(this == &rhs) return *this;
    
        std::memcpy(this->m_Power, rhs.m_Power, 4*sizeof(float));
        
        return *this;
    }

	Color Color::operator+(const Color& rhs)
	{
		Color sum = *this;
        sum += rhs;
		return sum;
	}
    Color& Color::operator+=(const Color& rhs)
    {
        this->R() += rhs.R();
        this->G() += rhs.G();
        this->B() += rhs.B();
        this->A() += rhs.A();

        return *this;
    }
    Color Color::operator-(const Color& rhs)
    {
        Color difference = *this;
        difference -= rhs;
        return difference;
    }
    Color& Color::operator-=(const Color& rhs)
    {
        this->R() -= rhs.R();
        this->G() -= rhs.G();
        this->B() -= rhs.B();
        this->A() -= rhs.A();

        return *this;
    }
	Color Color::operator*(const Color& rhs)
	{
		Color product = *this;
        product *= rhs;
		return product;
	}
    Color& Color::operator*=(const Color& rhs)
    {
        this->R() *= rhs.R();
        this->G() *= rhs.G();
        this->B() *= rhs.B();
        this->A() *= rhs.A();

        return *this;
    }
    Color Color::operator*(const float& rhs)
    {
        Color scaled = *this;
        scaled *= rhs;
        return scaled;
    }
    Color& Color::operator*=(const float& rhs)
    {
        this->R() *= rhs;
        this->G() *= rhs;
        this->B() *= rhs;
        this->A() *= rhs;

        return *this;
    }

    Color Color::operator/(const float& rhs)
    {
        Color scaled = *this;
        scaled /= rhs;
        return scaled;
    }

    Color& Color::operator/=(const float& rhs)
    {
        this->R() /= rhs;
        this->G() /= rhs;
        this->B() /= rhs;
        this->A() /= rhs;

        return *this;
    }

    Color Color::operator-()
    {
        return Color( -this->R(),
                      -this->G(),
                      -this->B(),
                      -this->A()
                    );
    }

    float Color::TotalIntensity()
    {
        return m_Power[0] + m_Power[1] + m_Power[2];
    }

    void Color::Clamp(float cMin, float cMax)
    {
        m_Power[0] = m_Power[0] < cMin ? cMin : m_Power[0] > cMax ? cMax : m_Power[0];
        m_Power[1] = m_Power[1] < cMin ? cMin : m_Power[1] > cMax ? cMax : m_Power[1];
        m_Power[2] = m_Power[2] < cMin ? cMin : m_Power[2] > cMax ? cMax : m_Power[2];
    }

}
