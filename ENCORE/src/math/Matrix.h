/*****************************************************
Author:       Brandon Light
Last Update:  10/06/2005

Notes:  
Simple NxM Matrix template class for graphics projects.

** INCOMPLETE **

TODO:
math operators,
identity matrix (static function?),
transpose, 
determinant, 
inverse, 
adjoint functions

*****************************************************/

#pragma once

#include <iostream>
#include <cassert>

#include "Vector3.h"

using encore::Point3;
using encore::Vector3; 

template <class T> class TMatrix
{
protected:
    int m_rowCount, m_colCount;
    T *m_data;

public:
    TMatrix(int in_rows, int in_cols);
    TMatrix(const TMatrix<T> &matrix);

    virtual ~TMatrix();   

    ///// OPERATORS  //////////////////////////////////
    TMatrix<T>& operator=(const TMatrix<T> &rhs);

    // MATRIX MATH
    TMatrix<T> operator+(const TMatrix<T> &rhs);
    TMatrix<T>& operator+=(const TMatrix<T> &rhs);

    TMatrix<T> operator-(const TMatrix<T> &rhs);
    TMatrix<T>& operator-=(const TMatrix<T> &rhs);

    TMatrix<T> operator*(const TMatrix<T> &rhs);
    TMatrix<T>& operator*=(const TMatrix<T> &rhs);

    // SCALAR MATH

    // access operators
    T& operator()(unsigned int row, unsigned int col)
    { return m_data[row * m_colCount + col]; }
    T operator()(unsigned int row, unsigned int col) const
    { return m_data[row * m_colCount + col]; }

    // i/o stream operators
    friend std::istream& operator>> (std::istream &input, TMatrix<T> &matrix)
    {
        // delete old data
        delete [] matrix.m_data;

        input >> matrix.m_rowCount;
        input >> matrix.m_colCount;

        matrix.m_data = new T[matrix.m_rowCount * matrix.m_colCount];

        // assign data to new 
        for(int row = 0; row < matrix.m_rowCount; row++)
            for(int col = 0; col < matrix.m_colCount; col++)
                input >> matrix.m_data[row * matrix.m_colCount + col];

        return input;
    }

    friend std::ostream& operator<<  (std::ostream &output, const TMatrix<T> &matrix)
    {
        for(int row = 0; row < matrix.m_rowCount; row++)
        {
            for(int col = 0; col < matrix.m_colCount; col++)
            {
                output << matrix.m_data[row * matrix.m_colCount + col] << " ";
            }
            output << std::endl;
        }
        return output;
    }
};

//////////   IMPLEMENTATION    ////////////////////////////////////////////////////

template <class T> TMatrix<T>::TMatrix(int in_rowCount, int in_colCount):
m_rowCount(in_rowCount), m_colCount(in_colCount)
{
    m_data = new T[m_rowCount * m_colCount];

    // init values of matrix to 0
    for(int row = 0; row < m_rowCount; row++)
    {
        for(int col = 0; col < m_colCount; col++)
        {
            m_data[row * m_colCount + col] = 0;
        }
    }
}

template <class T> TMatrix<T>::TMatrix(const TMatrix<T> &matrix)
{
    m_rowCount = matrix.m_rowCount;
    m_colCount = matrix.m_colCount;

    m_data = new T[m_rowCount * m_colCount];

    for(int row = 0; row < m_rowCount; row++)
    {
        for(int col = 0; col < m_colCount; col++)
        {
            m_data[row * m_colCount + col] = matrix(row, col);
        }
    }
}

template <class T> TMatrix<T>::~TMatrix()
{
    delete[] m_data;
}

template <class T> TMatrix<T>& TMatrix<T>::operator=(const TMatrix<T> &rhs)
{
    m_rowCount = rhs.m_rowCount;
    m_colCount = rhs.m_colCount;

    delete[] m_data;
    m_data = new T[m_rowCount * m_colCount];

    for(int row = 0; row < m_rowCount; row++)
    {
        for(int col = 0; col < m_colCount; col++)
        {
            m_data[row * m_colCount + col] = rhs(row, col);
        }
    }

    return *this;
}

template <class T> TMatrix<T> TMatrix<T>::operator+(const TMatrix<T> &rhs)
{
    TMatrix<T> sum = *this;
    sum += rhs;
    return sum;
}

template <class T> TMatrix<T>& TMatrix<T>::operator+=(const TMatrix<T> &rhs)
{
    assert((m_rowCount == rhs.m_rowCount) && (m_colCount == rhs.m_colCount));

    for(int row = 0; row < m_rowCount; ++row)
    {
        for(int col = 0; col < m_colCount; ++col)
        {
            (*this)(row, col) += rhs(row, col);
        }
    }
    return *this;
}

template <class T> TMatrix<T> TMatrix<T>::operator-(const TMatrix<T> &rhs)
{
    TMatrix<T> difference = *this;
    difference -= rhs;
    return rhs;
}

template <class T> TMatrix<T>& TMatrix<T>::operator-=(const TMatrix<T> &rhs)
{
    assert((m_rowCount == rhs.m_rowCount) && (m_colCount == rhs.m_colCount));

    for(int row = 0; row < m_rowCount; ++row)
    {
        for(int col = 0; col < m_colCount; ++col)
        {
            (*this)(row, col) -= rhs(row, col);
        }
    }
    return *this;
}

template <class T> TMatrix<T> TMatrix<T>::operator*(const TMatrix<T> &rhs)
{
    TMatrix<T> product = *this;
    product *= rhs;
    return product;
}

template <class T> TMatrix<T>& TMatrix<T>::operator*=(const TMatrix<T> &rhs)
{
    assert(m_colCount == rhs.m_rowCount);

    TMatrix<T> product(m_rowCount, rhs.m_colCount);

    for(int productRow = 0; productRow < product.m_rowCount; ++productRow)
    {
        for(int productCol = 0; productCol < product.m_colCount; ++productCol)
        {
            for(int termIndex = 0; termIndex < m_colCount; ++termIndex)
            {
               product(productRow, productCol) += ((*this)(productRow, termIndex) * rhs(termIndex, productCol));
            }

        }
    }
    *this = product;
    return *this;
}
