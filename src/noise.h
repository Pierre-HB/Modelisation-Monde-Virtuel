#ifndef _NOISE_H
#define _NOISE_H

inline float smoothstep1(float x){
    return x * x * (3-2*x);
}

inline float smoothstep2(float x){
    return x * x * x * (x * (x * 6 - 15) + 10);
}

inline float smoothstep3(float x){
    return x * x * x * x * (35 + x*(-84 + x*(70 - 20*x)));
}

// -20x^{7}+70x^{6}-84x^{5}+35x^{4}.}

//! virtual class for a 1D noise
class Noise1D
{
    public:
    //! virtual function to sampled the noise
    virtual float value(float) const = 0;
};

//! virtual class for a 2D noise
class Noise2D
{
    public:
    //! virtual function to sampled the noise
    virtual float value(float, float) const = 0;
};

//! A value grid based noise in 1D
class ValueGrid1D: public Noise1D
{
    public:
    ValueGrid1D(){}
    ~ValueGrid1D(){}
    //! Overide
    float value(float x) const;
};

//! A value grid based noise in 2D
class ValueGrid2D: public Noise2D
{
    public:
    ValueGrid2D(){}
    ~ValueGrid2D(){}
    //! Overide
    float value(float x, float y) const;
};
#endif