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
    //! get the value at node (i, j)
    float get_value(int i, int j) const;
    //! set the value at node (i, j)
    void set_value(int i, int j, float v);
    //! return the index in the values vector of the node (i, j)
    int get_index(int i, int j) const;
    //! export as a grayscale image
    void export_as_image(const char *file, bool normalisation = true) const;
    //! return the values vector
    std::vector<float> get_values() const; 
    //! return the value vector as color
    std::vector<Color> get_values_as_color() const; 

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

#endif