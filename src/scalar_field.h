#ifndef _SCALAR_FIELD_H
#define _SCALAR_FIELD_H

#include <vector>
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
    //! get the value at node (i, j)
    float get_value(int i, int j) const;
    //! set the value at node (i, j)
    void set_value(int i, int j, float v);

public:
//! TODO
    // float laplacian(int i, int j);

};

class Terrain2D: public ScalarField2D
{
private:
    float slope_max;
protected:
    Point min_p; //! min angle of the bouding box of the terrain
    Point max_p; //! min angle of the bouding box of the terrain
    InfinitTexture2D *texture; //! texture to compute heights

public:
    Terrain2D(InfinitTexture2D *texture, vec2 min_p, vec2 max_p, int nx, int ny);
    ~Terrain2D(){};
    //find a way to free the texture

    //! height at the node (i, j)
    float height(int i, int j) const;
    //! height at the point (x, y)
    float height(float x, float y) const;

    //! return the point at the node (i, j)
    Point point(int i, int j) const;

    //! The 2D gradiant of the height at the node (i, j)
    vec2 gradiant(int i, int j) const;
    //! normal at the node (i, j)
    Vector normal(int i, int j) const;
    //! length of the gradiant at the node (i, j)
    float slope(int i, int j) const;
    //! max slope of the terrain
    float max_slope() const;
    //! ambiant occlusion at the node (i, j)
    float ambiant_occlusion(int i, int j) const;

    /**
     * @brief return the intersection point with a ray
     * 
     * @param o : origine of the ray
     * @param d : direction of the ray
     * @param intersection : optional, the ref of a vec3 that will be set to the inetersection if there is an intersection
     * @return bool : true if there is an intersection
     */
    bool ray_intersection(Point o, Vector d, Point* intersection) const;
    bool ray_intersection(Point o, Vector d) const;

    //! return the list of Points representing the mesh
    std::vector<vec3> get_positions() const;
    //! return the list of Normals representing the mesh
    std::vector<vec3> get_normals() const;
    //! return the list of indexes representing the mesh
    std::vector<int> get_indexes() const;
    //! return the list of Colors representing the slope at each node
    std::vector<Color> get_slope_color() const;
    //! return the list of Colors representing the slope at each node
    std::vector<Color> get_occlusion_color() const;
};

#endif