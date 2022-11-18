
#include "noise.h"
#include <math.h>

float ValueGrid1D::value(float x) const{
    float whole = floor(x);
    float dec = x - whole;
    float s = smoothstep1(dec);

    return (1+cos(123456789*whole)*(1-s) + cos(123456789*(whole+1))*s)/2;
}



float ValueGrid2D::value(float x, float y) const{
    float whole_x = floor(x);
    float dec_x = x - whole_x;
    float s_x = smoothstep2(dec_x);

    float whole_y = floor(y);
    float dec_y = y - whole_y;
    float s_y = smoothstep2(dec_y);

    float tmp1 = (1+cos(1357+123456789*whole_x+123456788*whole_y)*(1-s_x) + cos(1357+123456789*(whole_x+1)+123456788*whole_y)*s_x)/2;
    float tmp2 = (1+cos(1357+123456789*whole_x+123456788*(whole_y+1))*(1-s_x) + cos(1357+123456789*(whole_x+1)+123456788*(whole_y+1))*s_x)/2;
    return tmp1*(1-s_y) + tmp2*s_y;
}
