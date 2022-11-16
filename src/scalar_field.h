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
    

public:
    //! empty constructor
    ScalarField2D();
    //! copy
    ScalarField2D(const ScalarField2D& cpy);
    //! constructor from a list of value
    ScalarField2D(const std::vector<float>& values, int nx, int ny);
    //! get the value at node (i, j)
    float get_value(int i, int j) const;
    //! set the value at node (i, j)
    void set_value(int i, int j, float v);
    //! export as a grayscale image
    void export_as_image(const char *file) const;

//! TODO
    // float laplacian(int i, int j);

};

#endif