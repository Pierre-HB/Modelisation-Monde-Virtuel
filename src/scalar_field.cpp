#include "scalar_field.h"
#include <math.h>
#include <random>
#include <string>
#include "mat.h"
#include "image.h"
#include "image_io.h"

ScalarField2D::ScalarField2D():values(){}

ScalarField2D::ScalarField2D(const ScalarField2D& cpy):nx(cpy.nx), ny(cpy.ny), values(cpy.values){}

ScalarField2D::ScalarField2D(const std::vector<float>& values, int nx, int ny):nx(nx), ny(ny), values(values){}

void ScalarField2D::export_as_image(const char *file) const{
    Image image(nx, ny, Color(0));
    float max_value = values[0];
    float min_value = values[0];
    for(float v : values){
        if(max_value < v)
            max_value = v;
        if(min_value > v)
            min_value = v;
    }

    for(int i = 0; i < nx; i++)
        for(int j = 0; j < ny; j++)
            image(i, j) = Color((get_value(i, j)-min_value)/(max_value-min_value));
    
    write_image(image, file);
}

float ScalarField2D::get_value(int i, int j) const{
    return values[j*nx + i];
}

void ScalarField2D::set_value(int i, int j, float v){
    values[j*nx + i] = v;
}
