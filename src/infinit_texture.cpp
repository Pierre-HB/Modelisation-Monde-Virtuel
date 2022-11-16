#include "infinit_texture.h"
#include <math.h>
#include "image.h"
#include "image_io.h"

void InfinitTexture2D::export_as_image(const char *file, int imageWidth, int imageHeight, float x, float y, float width, float height) const{
    float stepX = width/imageWidth;
    float stepY = height/imageHeight;
    Image image(imageWidth, imageHeight, Color(0));

    float min_value = value(x, y);
    float max_value = min_value;
    float x_ = x;
    float y_ = y;

    for(int i = 0; i < imageWidth; i++){
        for(int j = 0; j < imageHeight; j++){
            float v = value(x_, y_);
            image(i, j) = Color(v);
            
            if(v > max_value)
                max_value = v;
            if(v < min_value)
                min_value = v;

            y_ += stepY;
        }

        x_ += stepX;
        y_ = y;
    }
    for(int i = 0; i < imageWidth; i++)
        for(int j = 0; j < imageHeight; j++)
            image(i, j) = Color((image(i, j).r-min_value)/(max_value-min_value));

    write_image(image, file);
}


float InfinitTexture2DFromNoise::value(float x, float y) const{
    return noise->value(x, y);
}

float InfinitTexture2DTranslated::value(float x, float y) const{
    return texture->value(x+t.x, y+t.y);
}

float InfinitTexture2DZoom::value(float x, float y) const{
    return texture->value(x*zoom.x, y*zoom.y);
}

float InfinitTexture2DOffseted::value(float x, float y) const{
    return texture->value(x, y)+offset;
}

float InfinitTexture2DScaled::value(float x, float y) const{
    return scale*texture->value(x, y);
}

float InfinitTexture2DMin::value(float x, float y) const{
    return std::min(texture1->value(x, y), texture2->value(x, y));
}

float InfinitTexture2DMax::value(float x, float y) const{
    return std::max(texture1->value(x, y), texture2->value(x, y));
}

float InfinitTexture2DRotated::value(float x, float y) const{
    return texture->value(x*cos(alpha)+y*sin(alpha), -x*sin(alpha)+y*cos(alpha));
}

float InfinitTexture2DSum::value(float x, float y) const{
    return texture1->value(x, y) + texture2->value(x, y);
}

float InfinitTexture2DSub::value(float x, float y) const{
    return texture1->value(x, y) - texture2->value(x, y);
}

float InfinitTexture2DProduct::value(float x, float y) const{
    return texture1->value(x, y) * texture2->value(x, y);
}

InfinitTexture2D* sum(const InfinitTexture2D* texture1, const InfinitTexture2D* texture2){
    return new InfinitTexture2DSum(texture1, texture2);
}

InfinitTexture2D* sub(const InfinitTexture2D* texture1, const InfinitTexture2D* texture2){
    return new InfinitTexture2DSub(texture1, texture2);
}

InfinitTexture2D* product(const InfinitTexture2D* texture1, const InfinitTexture2D* texture2){
    return new InfinitTexture2DProduct(texture1, texture2);
}

InfinitTexture2D* min(const InfinitTexture2D* texture1, const InfinitTexture2D* texture2){
    return new InfinitTexture2DMin(texture1, texture2);
}

InfinitTexture2D* max(const InfinitTexture2D* texture1, const InfinitTexture2D* texture2){
    return new InfinitTexture2DMax(texture1, texture2);
}

InfinitTexture2D* translation(const InfinitTexture2D* texture, const vec2& t){
    return new InfinitTexture2DTranslated(texture, t);
}

InfinitTexture2D* zoom(const InfinitTexture2D* texture, const vec2& zoom){
    return new InfinitTexture2DZoom(texture, zoom);
}

InfinitTexture2D* offset(const InfinitTexture2D* texture, float offset){
    return new InfinitTexture2DOffseted(texture, offset);
}

InfinitTexture2D* scale(const InfinitTexture2D* texture, float scale){
    return new InfinitTexture2DScaled(texture, scale);
}

InfinitTexture2D* rotation(const InfinitTexture2D* texture, float alpha){
    return new InfinitTexture2DRotated(texture, alpha);
}

