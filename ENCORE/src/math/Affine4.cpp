#include "Affine4.h"

/**************
* Affine4
**************/
Affine4::Affine4( void )
{
    this->setIdentityMatrix();
}

// modified to comform to Opengl matrix
void Affine4::setIdentityMatrix( void )
{
    m[0] = m[5]  = m[10] = m[15] = 1.0;
    m[1] = m[2]  = m[3]  = m[4]  = 0.0;
    m[6] = m[7]  = m[8]  = m[9]  = 0.0;
    m[11]= m[12] = m[13] = m[14] = 0.0;
}

void Affine4::set( const Affine4& l_Affine4 )
{	
    for(int i = 0; i < 16; i++)
    {
        m[i] = l_Affine4.m[i];
    }
}

void Affine4::preMult( const Affine4& l_Affine4 )
{
    float sum;
    Affine4 tmp; 
    tmp.set(*this); // tmp copy

    // following mult's : this = tmp * n
    for(int c = 0; c < 4; c++)
    {
        for(int r = 0; r < 4 ; r++)
        {
            sum = 0;
            for(int k = 0; k < 4; k++)
                sum += l_Affine4.m[4 * k + r]* tmp.m[4 * c + k];
            m[4 * c + r] = sum;
        }
    }
}

void Affine4::postMult( const Affine4& l_Affine4)
{
    float sum;
    Affine4 tmp; 
    tmp.set(*this); // tmp copy

    for(int c = 0; c < 4; c++)// form this = tmp * n
    {
        for(int r = 0; r < 4 ; r++)
        {
            sum = 0;
            for(int k = 0; k < 4; k++)
                sum += tmp.m[4 * k + r]* l_Affine4.m[4 * c + k];
            m[4 * c + r] = sum;
        }
    }
}