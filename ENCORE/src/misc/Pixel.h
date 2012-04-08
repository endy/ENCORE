#pragma once

// @@@@@@@@@@@@@@@@@@@@@ Pixel class @@@@@@@@@@@@@@@@
class Pixel { // holds x,y,z,w values just like a pixel
public:

    Pixel( void )
    {
        x=y=z=w=0;
    }
    Pixel( float nx, float ny, float nz, float nw )
    {
        x=nx; y=ny; z=nz; w=nw;
    }

    //////// ACCESSORS ///////////////////////////
    inline float X() const { return x; }
    inline float& X() { return x; }

    inline float Y() const { return y; }
    inline float& Y(){ return y; }

    inline float Z() const { return z; }
    inline float& Z(){ return z; }

    inline float W() const { return w; }
    inline float& W(){ return w; }

    //////////////////////////////////////////////
    void normalize( void )
    {
        Vector3f p(x,y,z);
        p.Normalize();
        x = p.X(); y = p.Y(); z = p.Z();
    }

    void set( const Pixel& p )
    {
        x = p.X(); y = p.Y(); z = p.Z(); w = p.W();
    }
    void set( float nx, float ny, float nz, float nw )
    {
        x=nx; y=ny; z=nz; w=nw;
    }

    ~Pixel( void ){}

    void add( const Pixel& p)
    {
        x += p.X(); y += p.Y(); z += p.Z(); w += p.W();
    }

private:
    float x,y,z,w;
};