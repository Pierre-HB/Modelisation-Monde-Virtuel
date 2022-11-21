#include "scalar_field.h"
#include <math.h>
#include <random>
#include <string>
#include "mat.h"
#include "image.h"
#include "image_io.h"

ScalarField2D::ScalarField2D():values(){}

ScalarField2D::ScalarField2D(const ScalarField2D& cpy):nx(cpy.nx), ny(cpy.ny), values(cpy.values), min_p(cpy.min_p), max_p(cpy.max_p){}

ScalarField2D::ScalarField2D(const std::vector<float>& values, int nx, int ny, Point min_p, Point max_p):nx(nx), ny(ny), values(values), min_p(min_p), max_p(max_p){}

ScalarField2D ScalarField2D::update_min_max(){
    float min_v = values[0];
    float max_v = values[0];
    for(size_t i = 1; i < values.size(); i++){
        if(values[i] > max_v)
            max_v = values[i];
        if(values[i] < min_v)
            min_v = values[i];
    }
    min_p.z = min_v;
    max_p.z = max_v;
    return *this;
}
void ScalarField2D::export_as_image(const char *file, bool normalisation) const{
    Image image(nx, ny, Color(0));
    float max_value = values[0];
    float min_value = values[0];
    for(float v : values){
        if(max_value < v)
            max_value = v;
        if(min_value > v)
            min_value = v;
    }
    // float max_value = min_p.z;
    // float min_value = max_p.z;

    for(int i = 0; i < nx; i++)
        for(int j = 0; j < ny; j++)
            if(normalisation)
                image(i, j) = Color((get_value(i, j)-min_value)/(max_value-min_value));
            else
                image(i, j) = Color(get_value(i, j));
    
    write_image(image, file);
}

std::vector<float> ScalarField2D::get_values() const{
    return values;
}
std::vector<Color> ScalarField2D::get_values_as_color() const{
    std::vector<Color> colors = std::vector<Color>(values.size());
    for(size_t i = 0; i < values.size(); i++)
        colors[i] = Color(values[i]);
    return colors;
} 

ScalarField2D ScalarField2D::map(float (*function)(float)) const{
    std::vector<float> tmp = std::vector<float>(values.size());
    float max_v = function(values[0]);
    float min_v = max_v;
    for(size_t i = 0; i < values.size(); i++)
        tmp[i] = function(values[i]);
    return ScalarField2D(tmp, nx, ny, min_p, max_p).update_min_max();
}

ScalarField2D ScalarField2D::convolution(const std::vector<std::vector<float>>& kernel) const{
    int offset_x = kernel[0].size()/2;//swap between x and y because of transpose
    int offset_y = kernel.size()/2;
    std::cout << "kernel size : (" << kernel.size() << ", " << kernel[0].size() << ")" << std::endl;
    std::cout << "offset x : " << offset_x << ", offset y : " << offset_y << std::endl;

    int new_nx = nx - 2*offset_x;
    int new_ny = ny - 2*offset_y;

        std::cout << "new nx : " << new_nx << ", new ny : " << new_ny << std::endl;


    Vector e = Vector(offset_x*(max_p.x - min_p.x)/(nx-1),
                      offset_y*(max_p.y - min_p.y)/(ny-1),
                      0);

    Point new_min_p = min_p + e;
    Point new_max_p = max_p - e;

    // values[j*nx + i];

    std::vector<float> new_value = std::vector<float>(new_nx*new_ny, 0);
    for(int i = 0; i < new_nx; i++)
        for(int j = 0; j < new_ny; j++)
            for(size_t k = 0; k < kernel.size(); k++)
                for(size_t l = 0; l < kernel[k].size(); l++)
                    new_value[j*new_nx+i]+=get_value(i+l, j+k)*kernel[k][l];

    return ScalarField2D(new_value, new_nx, new_ny, new_min_p, new_max_p).update_min_max();
}


ScalarField2D& ScalarField2D::operator+=(const ScalarField2D& other){
    assert(nx==other.nx);
    assert(ny==other.ny);
    for(size_t i = 0; i < values.size(); i++)
        values[i]+=other.values[i];
    update_min_max();
    return *this;
}
ScalarField2D& ScalarField2D::operator*=(const ScalarField2D& other){
    assert(nx==other.nx);
    assert(ny==other.ny);
    for(size_t i = 0; i < values.size(); i++)
        values[i]*=other.values[i];
    update_min_max();
    return *this;
}
ScalarField2D& ScalarField2D::operator+=(const float& other){
    for(size_t i = 0; i < values.size(); i++)
        values[i]+=other;
    max_p.z+=other;
    min_p.z+=other;
    return *this;
}
ScalarField2D& ScalarField2D::operator*=(const float& other){
    for(size_t i = 0; i < values.size(); i++)
        values[i]*=other;
    max_p.z*=other;
    min_p.z*=other;
    return *this;
}

ScalarField2D ScalarField2D::derivate_x() const{
    float ex = (max_p.x - min_p.x)/(nx-1);
    std::vector<float> derivate = std::vector<float>(values.size());
    for(int i = 1; i < nx-1; i++)
        for(int j = 0; j < ny; j++)
            derivate[get_index(i, j)] = (get_value(i+1, j) - get_value(i-1, j))/(2*ex);
    
    int i = 0;
    for(int j = 0; j < ny; j++)
        derivate[get_index(i, j)] = (get_value(i+1, j) - get_value(i, j))/ex;
    
    i = nx-1;
    for(int j = 0; j < ny; j++)
        derivate[get_index(i, j)] = (get_value(i, j) - get_value(i-1, j))/ex;
    
    return ScalarField2D(derivate, nx, ny, min_p, max_p).update_min_max();
}
ScalarField2D ScalarField2D::derivate_y() const{
    float ey = (max_p.y - min_p.y)/(ny-1);
    std::vector<float> derivate = std::vector<float>(values.size());
    for(int j = 1; j < ny-1; j++)
        for(int i = 0; i < nx; i++)
            derivate[get_index(i, j)] = (get_value(i, j+1) - get_value(i, j-1))/(2*ey);
    
    int j = 0;
    for(int i = 0; i < nx; i++)
        derivate[get_index(i, j)] = (get_value(i, j+1) - get_value(i, j))/ey;
    
    j = ny-1;
    for(int i = 0; i < nx; i++)
        derivate[get_index(i, j)] = (get_value(i, j) - get_value(i, j-1))/ey;
    
    return ScalarField2D(derivate, nx, ny, min_p, max_p).update_min_max();
}

float ScalarField2D::laplacian(int i, int j) const{
    float laplacian_x = 0;
    float laplacian_y = 0;
    float ex = (max_p.x - min_p.x)/(nx-1);
    float ey = (max_p.y - min_p.y)/(ny-1);
    if(i == 0)
        // laplacian_x = 4*(get_value(i+1, j) - get_value(i, j))/(ex*ex);
        laplacian_x = (get_value(i+2, j) + get_value(i, j) - 2*get_value(i+1, j))/(ex*ex);
    else if(i == nx-1)
        // laplacian_x = 4*(get_value(i, j) - get_value(i-1, j))/(ex*ex);
        laplacian_x = (get_value(i, j) + get_value(i-2, j) - 2*get_value(i-1, j))/(ex*ex);
    else
        laplacian_x = (get_value(i-1, j) + get_value(i+1, j) - 2*get_value(i, j))/(ex*ex);

    if(j == 0)
        // laplacian_y = 4*(get_value(i, j+1) - get_value(i, j))/(ey*ey);
        laplacian_y = (get_value(i, j+2) + get_value(i, j) - 2*get_value(i, j+1))/(ey*ey);
    else if(j == ny-1)
        // laplacian_y = 4*(get_value(i, j) - get_value(i, j-1))/(ey*ey);
        laplacian_y = (get_value(i, j) + get_value(i, j-2) - 2*get_value(i, j-1))/(ey*ey);
    else
        laplacian_y = (get_value(i, j-1) + get_value(i, j+1) - 2*get_value(i, j))/(ey*ey);
    
    return laplacian_x + laplacian_y;
}
ScalarField2D ScalarField2D::laplacian() const{
    std::vector<float> laplacians = std::vector<float>(values.size());
    for(int i = 0; i < nx; i++)
        for(int j = 0; j < ny; j++)
            laplacians[get_index(i, j)] = laplacian(i, j);
    return ScalarField2D(laplacians, nx, ny, min_p, max_p).update_min_max();
}


