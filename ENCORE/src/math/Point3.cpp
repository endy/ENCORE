//////////////////////////////////////////////////////////////////////////////////////////////////
///
///     ENCORE
///
///     Copyright 2006, 2012, Brandon Light
///     All rights reserved.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Point3.h"

namespace encore
{

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
