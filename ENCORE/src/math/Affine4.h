#pragma once 

/*********************
* Affine4
*********************/
class Affine4
{
    // manages homogeneous affine transformations
    // including inverse transformations
    // and a stack to put them on
    // used by Scene class to read SDL files
public:

    float m[16]; // hold a 4 by 4 matrix

    Affine4( void );

    void setIdentityMatrix( void );
    void set( const Affine4& l_Affine4 );
    void preMult( const Affine4& l_Affine4 );
    void postMult( const Affine4& l_Affine4 );
}; 
