#ifndef _INFINIT_TEXTURE_H
#define _INFINIT_TEXTURE_H

#include "noise.h"
#include "vec.h"

class InfinitTexture2D;

InfinitTexture2D* sum(const InfinitTexture2D* texture1, const InfinitTexture2D* texture2);
InfinitTexture2D* sub(const InfinitTexture2D* texture1, const InfinitTexture2D* texture2);
InfinitTexture2D* product(const InfinitTexture2D* texture1, const InfinitTexture2D* texture2);
InfinitTexture2D* min(const InfinitTexture2D* texture1, const InfinitTexture2D* texture2);
InfinitTexture2D* max(const InfinitTexture2D* texture1, const InfinitTexture2D* texture2);
InfinitTexture2D* translation(const InfinitTexture2D* texture, const vec2& t);
InfinitTexture2D* zoom(const InfinitTexture2D* texture, const vec2& zoom);
InfinitTexture2D* offset(const InfinitTexture2D* texture, float offset);
InfinitTexture2D* scale(const InfinitTexture2D* texture, float scale);
InfinitTexture2D* rotation(const InfinitTexture2D* texture, float alpha);


//! A texture that can be sampled for any point (x, y)
class InfinitTexture2D
{
public:
    //! The value of the texture at point (x, y)
    virtual float value(float x, float y) const = 0;
};

//! An infinit texture created from a noise
class InfinitTexture2DFromNoise: public InfinitTexture2D
{
private:
    const Noise2D *noise;
public:
    InfinitTexture2DFromNoise(const Noise2D *noise): noise(noise){}
    ~InfinitTexture2DFromNoise(){};
    //TODO find a way to destroy the noise, or not ?
    //! Overide
    float value(float x, float y) const;
};

//! An infinit texture created from the translation of a texture
class InfinitTexture2DTranslated: public InfinitTexture2D
{
private:
    const InfinitTexture2D *texture;
    const vec2 t;
public:
    InfinitTexture2DTranslated(const InfinitTexture2D *texture, const vec2& t): texture(texture), t(t) {}
    ~InfinitTexture2DTranslated(){};
    //! Overide
    float value(float x, float y) const;
};

//! An infinit texture created from the zoom of a texture
class InfinitTexture2DZoom: public InfinitTexture2D
{
private:
    const InfinitTexture2D *texture;
    const vec2 zoom;
public:
    InfinitTexture2DZoom(const InfinitTexture2D *texture, const vec2& zoom): texture(texture), zoom(zoom) {}
    ~InfinitTexture2DZoom(){};
    //! Overide
    float value(float x, float y) const;
};

//! An infinit texture created from the scaling of a texture
class InfinitTexture2DScaled: public InfinitTexture2D
{
private:
    const InfinitTexture2D *texture;
    float scale;
public:
    InfinitTexture2DScaled(const InfinitTexture2D *texture, float scale): texture(texture), scale(scale) {}
    ~InfinitTexture2DScaled(){};
    //! Overide
    float value(float x, float y) const;
};

//! An infinit texture created from the offset of a texture
class InfinitTexture2DOffseted: public InfinitTexture2D
{
private:
    const InfinitTexture2D *texture;
    float offset;
public:
    InfinitTexture2DOffseted(const InfinitTexture2D *texture, float offset): texture(texture), offset(offset) {}
    ~InfinitTexture2DOffseted(){};
    //! Overide
    float value(float x, float y) const;
};

//! An infinit texture created from the min of a two textures
class InfinitTexture2DMin: public InfinitTexture2D
{
private:
    const InfinitTexture2D *texture1;
    const InfinitTexture2D *texture2;
public:
    InfinitTexture2DMin(const InfinitTexture2D *texture1, const InfinitTexture2D *texture2): texture1(texture1), texture2(texture2) {}
    ~InfinitTexture2DMin(){};
    //! Overide
    float value(float x, float y) const;
};

//! An infinit texture created from the max of a two textures
class InfinitTexture2DMax: public InfinitTexture2D
{
private:
    const InfinitTexture2D *texture1;
    const InfinitTexture2D *texture2;
public:
    InfinitTexture2DMax(const InfinitTexture2D *texture1, const InfinitTexture2D *texture2): texture1(texture1), texture2(texture2) {}
    ~InfinitTexture2DMax(){};
    //! Overide
    float value(float x, float y) const;
};

//! An infinit texture created from the rotation of a texture
class InfinitTexture2DRotated: public InfinitTexture2D
{
private:
    const InfinitTexture2D *texture;
    float alpha;
public:
    InfinitTexture2DRotated(const InfinitTexture2D *texture, float alpha): texture(texture), alpha(alpha) {}
    ~InfinitTexture2DRotated(){};
    //! Overide
    float value(float x, float y) const;
};

//! An infinit texture created from the sum of a two textures
class InfinitTexture2DSum: public InfinitTexture2D
{
private:
    const InfinitTexture2D *texture1;
    const InfinitTexture2D *texture2;
public:
    InfinitTexture2DSum(const InfinitTexture2D *texture1, const InfinitTexture2D *texture2): texture1(texture1), texture2(texture2) {}
    ~InfinitTexture2DSum(){};
    //! Overide
    float value(float x, float y) const;
};

//! An infinit texture created from the substraction of a two textures
class InfinitTexture2DSub: public InfinitTexture2D
{
private:
    const InfinitTexture2D *texture1;
    const InfinitTexture2D *texture2;
public:
    InfinitTexture2DSub(const InfinitTexture2D *texture1, const InfinitTexture2D *texture2): texture1(texture1), texture2(texture2) {}
    ~InfinitTexture2DSub(){};
    //! Overide
    float value(float x, float y) const;
};

//! An infinit texture created from the product of a two textures
class InfinitTexture2DProduct: public InfinitTexture2D
{
private:
    const InfinitTexture2D *texture1;
    const InfinitTexture2D *texture2;
public:
    InfinitTexture2DProduct(const InfinitTexture2D *texture1, const InfinitTexture2D *texture2): texture1(texture1), texture2(texture2) {}
    ~InfinitTexture2DProduct(){};
    //! Overide
    float value(float x, float y) const;
};

#endif