//////////////////////////////////////////////////////////////////////////////////////////////////
///
///     ENCORE
///
///     Copyright 2006, 2012, Brandon Light
///     All rights reserved.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector3.h"

namespace encore
{
    // construct a floatVector3 from two 3-dimensional points
    //   vector = end - start
    Vector3::Vector3(Point3 start, Point3 end)
    {
        this->X() = end.X() - start.X();
        this->Y() = end.Y() - start.Y();
        this->Z() = end.Z() - start.Z();
    }

    Vector3& Vector3::operator=(const Vector3& rhs)
    {
        if(this == &rhs){ return *this; }

        memcpy(m_Components, rhs.m_Components, 3*sizeof(float));

        return *this;
    }

    Vector3 Vector3::operator+(const Vector3& rhs)
    {
        Vector3 sum = *this;
        sum += rhs;
        return sum;
    }
    Vector3& Vector3::operator+=(const Vector3& rhs)
    {
        this->X() += rhs.X();
        this->Y() += rhs.Y();
        this->Z() += rhs.Z();
        return *this;
    }

    Vector3 Vector3::operator-(const Vector3& rhs)
    {
        Vector3 difference = *this;
        difference -= rhs;
        return difference;
    }
    Vector3& Vector3::operator-=(const Vector3& rhs)
    {
        this->X() -= rhs.X();
        this->Y() -= rhs.Y();
        this->Z() -= rhs.Z();
        return *this;
    }

    Vector3 Vector3::operator*(const float& rhs)
    {
        Vector3 product = *this;
        product *= rhs;
        return product;
    }
    Vector3& Vector3::operator*=(const float& rhs)
    {
        this->X() *= rhs;
        this->Y() *= rhs;
        this->Z() *= rhs;
        return *this;
    }

    Vector3 Vector3::operator/(const float& rhs)
    {
        Vector3 quotient = *this;
        quotient /= rhs;
        return quotient;
    }

    Vector3& Vector3::operator/=(const float& rhs)
    {
        this->X() /= rhs;
        this->Y() /= rhs;
        this->Z() /= rhs;
        return *this;
    }

    bool Vector3::operator==(const Vector3& rhs) const
    {
        return this->X() == rhs.X() &&
            this->Y() == rhs.Y() &&
            this->Z() == rhs.Z();
    }

    Vector3 Vector3::operator-()
    {
        return Vector3(
            -this->X(),
            -this->Y(),
            -this->Z()
            );
    }

    void Vector3::Normalize()
    {
        float magnitude = this->Magnitude();

        this->X() = this->X() / magnitude;
        this->Y() = this->Y() / magnitude;
        this->Z() = this->Z() / magnitude;

        ClampfloatoZero();
    }

    float Vector3::Magnitude()
    { 
        return (float) std::sqrt(MagnitudeSquared());
    }

    float Vector3::MagnitudeSquared()
    {
        return (this->X() * this->X()) + (this->Y() * this->Y()) + (this->Z() * this->Z());
    }

    void Vector3::ClampfloatoZero()
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

    ///////////  Friend Function //////////////////////////////	 
    float Dot(const Vector3& v1, const Vector3& v2)
    {
        float product = v1.X() * v2.X() +
            v1.Y() * v2.Y() +
            v1.Z() * v2.Z();

        return product;
    }



    Vector3 Cross(const Vector3& v1, const Vector3& v2)
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
