/*****************************************************************************
TAffineMatrix.h

Brandon Light
10/30/2006

A lightweight 4x4 affine matrix for graphics

*****************************************************************************/

#pragma once

#include <iostream>
#include "TVector3.h"

using encore::TPoint3;
using encore::TVector3;

template <class T> class TAffineMatrix
{
private:
    int m_RowCount, m_ColCount;
    T *m_Data;
public:
    TAffineMatrix<T>();
    TAffineMatrix<T>(const TAffineMatrix<T> &m);
    ~TAffineMatrix<T>();

    ////  STATIC METHODS  ////////////////////////////////////////////////
    static TAffineMatrix<T> GetIdentityMatrix();
    static TAffineMatrix<T> GetTranslationMatrix(TVector3<T> transVector);
    static TAffineMatrix<T> GetScaleMatrix(TVector3<T> scaleVector);
    static TAffineMatrix<T> GetRotationMatrix(TVector3<T> axis, float angle);

    static TAffineMatrix<T> GetXRotationMatrix(T angle);
    static TAffineMatrix<T> GetYRotationMatrix(T angle);
    static TAffineMatrix<T> GetZRotationMatrix(T angle);

    //// OPERATORS  //////////////////////////////////////////////////////
    TAffineMatrix<T> operator=(const TAffineMatrix<T> &rhs);

    TAffineMatrix<T> operator+(const TAffineMatrix<T> &rhs);
    TAffineMatrix<T>& operator+=(const TAffineMatrix<T> &rhs);

    TAffineMatrix<T> operator-(const TAffineMatrix<T> &rhs);
    TAffineMatrix<T>& operator-=(const TAffineMatrix<T> &rhs);

    TAffineMatrix<T> operator*(const TAffineMatrix<T> &rhs);
    TAffineMatrix<T>& operator*=(const TAffineMatrix<T> &rhs);

    TPoint3<T> operator*(const TPoint3<T> &rhs);
    TVector3<T> operator*(const TVector3<T> &rhs);

    // access operators
    T& operator()(unsigned int row, unsigned int col)
    { return m_Data[row * m_ColCount + col]; }
    T operator()(unsigned int row, unsigned int col) const
    { return m_Data[row * m_ColCount + col]; }

    // i/o stream operators
    friend std::istream& operator>> (std::istream &input, TAffineMatrix<T> &matrix)
    {
        // delete old data
        delete [] matrix.m_Data;

        input >> matrix.m_RowCount;
        input >> matrix.m_ColCount;

        matrix.m_data = new T[matrix.m_RowCount * matrix.m_ColCount];

        // assign data to new 
        for(int row = 0; row < matrix.m_RowCount; row++)
            for(int col = 0; col < matrix.m_ColCount; col++)
                input >> matrix.m_Data[row * matrix.m_ColCount + col];

        return input;
    }

    friend std::ostream& operator<<  (std::ostream &output, const TAffineMatrix<T> &matrix)
    {
        for(int row = 0; row < matrix.m_RowCount; row++)
        {
            for(int col = 0; col < matrix.m_ColCount; col++)
            {
                output << matrix.m_Data[row * matrix.m_ColCount + col] << " ";
            }
            output << std::endl;
        }
        return output;
    }

    ////  METHODS  /////////////////////////////////////////
    TAffineMatrix<T> GetTranspose();
};

template <class T> TAffineMatrix<T>::TAffineMatrix()
{
    m_RowCount = 4;
    m_ColCount = 4;

    m_Data = new T[m_RowCount * m_ColCount];

    for(int row = 0; row < m_RowCount; row++)
    {
        for(int col = 0; col < m_ColCount; col++)
        {
            m_Data[row * m_ColCount + col] = 0;
        }
    }
}
template <class T> TAffineMatrix<T>::TAffineMatrix(const TAffineMatrix<T> &matrix)
{
    m_RowCount = 4;
    m_ColCount = 4;

    m_Data = new T[m_RowCount * m_ColCount];

    for(int row = 0; row < m_RowCount; row++)
    {
        for(int col = 0; col < m_ColCount; col++)
        {
            m_Data[row * m_ColCount + col] = matrix(row, col);
        }
    }
}

template <class T> TAffineMatrix<T>::~TAffineMatrix()
{
    delete[] m_Data;
}

template <class T> TAffineMatrix<T> TAffineMatrix<T>::GetIdentityMatrix()
{
    TAffineMatrix<T> m;
    m(0,0) = 1;
    m(1,1) = 1;
    m(2,2) = 1;
    m(3,3) = 1;
    return m;
}
template <class T> TAffineMatrix<T> TAffineMatrix<T>::GetTranslationMatrix(TVector3<T> transVector)
{
    TAffineMatrix<T> m = GetIdentityMatrix();
    m(0,3) = transVector.X();
    m(1,3) = transVector.Y();
    m(2,3) = transVector.Z();
    return m;
}
template <class T> TAffineMatrix<T> TAffineMatrix<T>::GetScaleMatrix(TVector3<T> scaleVector)
{
    TAffineMatrix<T> m;
    m(0,0) = scaleVector.X();
    m(1,1) = scaleVector.Y();
    m(2,2) = scaleVector.Z();
    m(3,3) = 1;
    return m;
}
template <class T> TAffineMatrix<T> TAffineMatrix<T>::GetRotationMatrix(TVector3<T> axis, float angle)
{
    TAffineMatrix<T> rMatrix;
    TVector3<T> r = axis;
    TVector3<T> s;

    T x = abs(axis.X());
    T y = abs(axis.Y());
    T z = abs(axis.Z());

    if(x <= y && x <= z)
    {
        // x is smallest
        s.X() = 0;
        s.Y() = -axis.Z();
        s.Z() = axis.Y();
    }
    else if(y <= x && y <= z)
    {
        // y is smallest
        s.X() = -axis.Z();
        s.Y() = 0;
        s.Z() = axis.X();
    }
    else
    {
        // z is smallest
        s.X() = -axis.Y();
        s.Y() = axis.X();
        s.Z() = 0;
    }

    s.Normalize();

    Vector3f t = Cross(r, s);
    t.Normalize();

    rMatrix(0,0) = r.X();
    rMatrix(0,1) = r.Y();
    rMatrix(0,2) = r.Z();

    rMatrix(1,0) = s.X();
    rMatrix(1,1) = s.Y();
    rMatrix(1,2) = s.Z();

    rMatrix(2,0) = t.X();
    rMatrix(2,1) = t.Y();
    rMatrix(2,2) = t.Z();

    return rMatrix.GetTranspose() * GetXRotationMatrix(angle) * rMatrix;
}


template <class T> TAffineMatrix<T> TAffineMatrix<T>::GetXRotationMatrix(T angle)
{
    TAffineMatrix<T> m;
    m(0,0) = 1;
    m(1,1) = cos(angle);
    m(1,2) = -sin(angle);
    m(2,1) = sin(angle);
    m(2,2) = cos(angle);
    m(3,3) = 1;

    return m;
}

template <class T> TAffineMatrix<T> TAffineMatrix<T>::GetYRotationMatrix(T angle)
{
    TAffineMatrix<T> m;
    m(0,0) = cos(angle);
    m(0,2) = sin(angle);
    m(1,1) = 1;
    m(2,0) = -sin(angle);
    m(2,2) = cos(angle);
    m(3,3) = 1;
    return m;
}

template <class T> TAffineMatrix<T> TAffineMatrix<T>::GetZRotationMatrix(T angle)
{
    TAffineMatrix<T> m;
    m(0,0) = cos(angle);
    m(0,1) = -sin(angle); 
    m(1,0) = sin(angle);
    m(1,1) = cos(angle);
    m(2,2) = 1;
    m(3,3) = 1;

    return m;
}

template <class T> TAffineMatrix<T> TAffineMatrix<T>::operator=(const TAffineMatrix<T> &rhs)
{
    for(int row = 0; row < m_RowCount; row++)
    {
        for(int col = 0; col < m_ColCount; col++)
        {
            m_Data[row * m_ColCount + col] = rhs(row, col);
        }
    }

    return *this;
}
template <class T> TAffineMatrix<T> TAffineMatrix<T>::operator+(const TAffineMatrix<T> &rhs)
{
    TAffineMatrix<T> sum = *this;
    sum += rhs;
    return sum;
}

template <class T> TAffineMatrix<T>& TAffineMatrix<T>::operator+=(const TAffineMatrix<T> &rhs)
{
    for(int row = 0; row < m_RowCount; ++row)
    {
        for(int col = 0; col < m_ColCount; ++col)
        {
            (*this)(row, col) += rhs(row, col);
        }
    }
    return *this;
}

template <class T> TAffineMatrix<T> TAffineMatrix<T>::operator-(const TAffineMatrix<T> &rhs)
{
    TAffineMatrix<T> difference = *this;
    difference -= rhs;
    return difference;
}

template <class T> TAffineMatrix<T>& TAffineMatrix<T>::operator-=(const TAffineMatrix<T> &rhs)
{
    for(int row = 0; row < m_RowCount; ++row)
    {
        for(int col = 0; col < m_ColCount; ++col)
        {
            (*this)(row, col) -= rhs(row, col);
        }
    }
    return *this;
}
template <class T> TAffineMatrix<T> TAffineMatrix<T>::operator*(const TAffineMatrix<T> &rhs)
{
    TAffineMatrix<T> product = *this;
    product *= rhs;
    return product;
}
template <class T> TAffineMatrix<T>& TAffineMatrix<T>::operator*=(const TAffineMatrix<T> &rhs)
{
    TAffineMatrix<T> product;

    for(int productRow = 0; productRow < product.m_RowCount; ++productRow)
    {
        for(int productCol = 0; productCol < product.m_ColCount; ++productCol)
        {
            for(int termIndex = 0; termIndex < m_ColCount; ++termIndex)
            {
                product(productRow, productCol) += ((*this)(productRow, termIndex) * rhs(termIndex, productCol));
            }

        }
    }
    *this = product;
    return *this;
}

template <class T> TPoint3<T> TAffineMatrix<T>::operator*(const TPoint3<T> &rhs)
{
    TPoint3<T> p;

    p.X() = (*this)(0,0) * rhs.X() + (*this)(0, 1) * rhs.Y() + (*this)(0, 2) * rhs.Z();
    p.Y() = (*this)(1,0) * rhs.X() + (*this)(1, 1) * rhs.Y() + (*this)(1, 2) * rhs.Z();
    p.Z() = (*this)(2,0) * rhs.X() + (*this)(2, 1) * rhs.Y() + (*this)(2, 2) * rhs.Z();

    return p;
}

template <class T> TVector3<T> TAffineMatrix<T>::operator *(const TVector3<T> &rhs)
{
    TVector3<T> v;

    v.X() = (*this)(0,0) * rhs.X() + (*this)(0, 1) * rhs.Y() + (*this)(0, 2) * rhs.Z() + (*this)(0,3) * 1;
    v.Y() = (*this)(1,0) * rhs.X() + (*this)(1, 1) * rhs.Y() + (*this)(1, 2) * rhs.Z() + (*this)(1,3) * 1;
    v.Z() = (*this)(2,0) * rhs.X() + (*this)(2, 1) * rhs.Y() + (*this)(2, 2) * rhs.Z() + (*this)(2,3) * 1;

    float w = (*this)(3,0) * rhs.X() + (*this)(3, 1) * rhs.Y() + (*this)(3, 2) * rhs.Z() + (*this)(3,3) * 1;

    if(w != 1.0f && w != 0)
    {
        v.X() /= w;
        v.Y() /= w;
        v.Z() /= w;
    }

    return v;
}

template <class T> TAffineMatrix<T> TAffineMatrix<T>::GetTranspose()
{
    TAffineMatrix<T> transpose;
    for(int row = 0; row < m_RowCount; ++row)
    {
        for(int col = 0; col < m_ColCount; ++col)
        {
            transpose(row, col) = (*this)(col, row);
        }
    }

    return transpose;
}