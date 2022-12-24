#ifndef _SCALAR_FIELD_H
#define _SCALAR_FIELD_H

#include <vector>
#include <string>
#include "vec.h"
#include "color.h"
#include "infinit_texture.h"

class ScalarField2D
{
protected:
    int nx; //! Number of point on the x axis (first axis) of the gris
    int ny; //! Number of point on the y axis (second axis) of the gris
    std::vector<float> values; //! vector of size nx, ny. Value (i, j) is located at the position j*nx+i
    const float epsilon = 1.0e-3;
    Point min_p; //! min angle of the bouding box of the terrain
    Point max_p; //! min angle of the bouding box of the terrain
    

public:
    //! empty constructor
    ScalarField2D();
    //! copy
    ScalarField2D(const ScalarField2D& cpy);
    //! constructor from a list of value
    ScalarField2D(const std::vector<float>& values, int nx, int ny, Point min_p, Point max_p);

    ScalarField2D& operator+=(const ScalarField2D& other);
    //! operator += wothtout affecting the border values
    ScalarField2D& add_border_freezed(const ScalarField2D& other);
    ScalarField2D& operator*=(const ScalarField2D& other);
    ScalarField2D& operator+=(const float& other);
    ScalarField2D& operator*=(const float& other);

    friend ScalarField2D operator+(ScalarField2D a, const ScalarField2D& b);
    friend ScalarField2D operator*(ScalarField2D a, const ScalarField2D& b);
    friend ScalarField2D operator+(ScalarField2D a, const float& b);
    friend ScalarField2D operator*(ScalarField2D a, const float& b);
    friend ScalarField2D operator+(const float& b, ScalarField2D a);
    friend ScalarField2D operator*(const float& b, ScalarField2D a);

    //! compute min and max value of field
    ScalarField2D update_min_max();
    //! get the value at node (i, j)
    float get_value(int i, int j) const;
    //! get the value at postion p
    float get_value(vec2 p) const;
    //! set the value at node (i, j)
    void set_value(int i, int j, float v);
    //! return the index in the values vector of the node (i, j)
    int get_index(int i, int j) const;
    //! return the pair (i, j) assosciated to an index in the values array
    std::pair<int, int> get_coordinate(int index) const;
    //! return the pair (i, j) assosciated to the point v
    std::pair<int, int> get_coordinate(vec2 v) const;
    //! return the pair (i, j) assosciated to the point v
    std::pair<int, int> get_coordinate(vec2 v, int nx_, int ny_) const;
    //! return the index in the values vector of the node (i, j)
    int get_index(int i, int j, int nx_, int ny_) const;
    //! return the vec2 corrsponding to the node (i, j)
    vec2 get_vec(int i, int j) const;
    //! return the vec2 corrsponding to the node (i, j)
    vec2 get_vec(int i, int j, int nx_, int ny_) const;
    //! return the pair (i, j) assosciated to an index in the values array
    std::pair<int, int> get_coordinate(int index, int nx_, int ny_) const;
    //! export as a grayscale image
    void export_as_image(const char *file, bool normalisation = true) const;
    //! return the values vector
    std::vector<float> get_values() const; 
    //! return nx
    int get_nx() const{return nx;}
    //! return ny
    int get_ny() const{return ny;}
    //! return min_p
    Point get_min_p() const{return min_p;}
    //! return max_p
    Point get_max_p() const{return max_p;}
    //! return the value vector as color
    std::vector<Color> get_values_as_color() const;
    //! return the scalarfield maped by the function f
    ScalarField2D map(float (*function)(float)) const;
    //! return the convolution with a kernel
    //! kernel[i][j] is the kernel value at line i and collumn j
    //! Be carefull during the convolution, the kernel is transpose
    ScalarField2D convolution(const std::vector<std::vector<float>>& kernel) const;

    //! return the scalarfield of the derivation with respect to x
    ScalarField2D derivate_x() const;
    //! return the scalarfield of the derivation with respect to y
    ScalarField2D derivate_y() const;
    //! return the laplacian at node (i, j)
    float laplacian(int i, int j) const;
    //! return the scalarfield of the laplacian
    ScalarField2D laplacian() const;

};

inline float ScalarField2D::get_value(int i, int j) const{
    return values[j*nx + i];
}

inline void ScalarField2D::set_value(int i, int j, float v){
    values[j*nx + i] = v;
}

inline int ScalarField2D::get_index(int i, int j) const{
    return j*nx+i;
}

inline std::pair<int, int> ScalarField2D::get_coordinate(int index) const{
    return std::pair<int, int>(index % nx, index / nx);
}

inline int ScalarField2D::get_index(int i, int j, int nx_, int ny_) const{
    return j*nx_ + i;
}

inline std::pair<int, int> ScalarField2D::get_coordinate(int index, int nx_, int ny_) const{
    return std::pair<int, int>(index % nx_, index / nx_);
}

inline vec2 ScalarField2D::get_vec(int i, int j) const{
    return vec2(float(i)/(nx-1) * (max_p.x-min_p.x) + min_p.x, float(j)/(ny-1) * (max_p.y-min_p.y) + min_p.y);
}

inline vec2 ScalarField2D::get_vec(int i, int j, int nx_, int ny_) const{
    return vec2(float(i)/(nx_-1) * (max_p.x-min_p.x) + min_p.x, float(j)/(ny_-1) * (max_p.y-min_p.y) + min_p.y);
}

inline ScalarField2D operator+(ScalarField2D a, const ScalarField2D& b){return a+=b;}
inline ScalarField2D operator*(ScalarField2D a, const ScalarField2D& b){return a*=b;}
inline ScalarField2D operator+(ScalarField2D a, const float& b){return a+=b;}
inline ScalarField2D operator*(ScalarField2D a, const float& b){return a*=b;}
inline ScalarField2D operator+(const float& b, ScalarField2D a){return a+=b;}
inline ScalarField2D operator*(const float& b, ScalarField2D a){return a*=b;}
#endif