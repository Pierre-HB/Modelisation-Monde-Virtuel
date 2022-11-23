
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



double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
double lerp(double t, double a, double b) { return a + t * (b - a); }
double grad(int hash, double x, double y) {
      int h = hash & 7;                      
      double u = h<4 ? x : y,                 
             v = h<2 ? y : h==5||h==6 ? x : y;
      return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
   }
   
float Perlin2D::value(float x, float y) const{
      int X = (int)floor(x) & 255,                  
          Y = (int)floor(y) & 255;
      x -= floor(x);                                
      y -= floor(y);                                
      double u = fade(x),                                
             v = fade(y);
      int A = (p[X]+Y) & 255         , AA = p[A], AB = p[(A+1) & 255],      
          B = (p[(X+1) & 255]+Y)& 255, BA = p[B], BB = p[(B+1) & 255];   
 
      return lerp(v, lerp(u, grad(p[AA  ], x  , y  ), 
                             grad(p[BA  ], x-1, y  )),
                     lerp(u, grad(p[AB  ], x  , y-1), 
                             grad(p[BB  ], x-1, y-1)));
   }