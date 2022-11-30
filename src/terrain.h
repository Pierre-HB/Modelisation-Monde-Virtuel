#ifndef _TERRAIN_H
#define _TERRAIN_H

#include <vector>
#include <string>
#include "vec.h"
#include "color.h"
#include "infinit_texture.h"
#include "scalar_field.h"
#include "dijkstra.hpp"

struct neighborhood{
    int size;
    int x[8];
    int y[8];
    float length[8];
    float coef[8];
};

struct heighCell{
    float height;
    int i;
    int j;
};

class Terrain2D: public ScalarField2D
{
private:
    float slope_max;
    std::vector<bool> path; //grid of the same size as value telling if the node is on a path or not
public:
    Terrain2D(InfinitTexture2D *texture, vec2 min_p, vec2 max_p, int nx, int ny);
    Terrain2D(const char *filename, vec2 min_p, vec2 max_p);
    Terrain2D(const ScalarField2D& sf);
    ~Terrain2D(){};

    //! compute the neighborhood of a node(cell adjacent) with the distance between the two cell and the coeficent of down-flow with the metric L^p
    neighborhood find_neighborhood(int i, int j, float p = 4) const;
    //! height at the node (i, j)
    float height(int i, int j) const;
    //! height at the point (x, y)
    float height(float x, float y) const;

    //! return the point at the node (i, j)
    Point point(int i, int j) const;

    //! The 2D gradient of the height at the node (i, j)
    vec2 gradient(int i, int j) const;
    //! normal at the node (i, j)
    Vector normal(int i, int j) const;
    //! length of the gradient at the node (i, j)
    float slope(int i, int j) const;
    //! max slope of the terrain
    float max_slope() const;
    //! ambiant occlusion at the node (i, j)
    float ambiant_occlusion(int i, int j, int nb_ray=64) const;

    /**
     * @brief return the intersection point with a ray
     * 
     * @param o : origine of the ray
     * @param d : direction of the ray
     * @param intersection : optional, the ref of a vec3 that will be set to the inetersection if there is an intersection
     * @return bool : true if there is an intersection
     */
    bool ray_intersection(Point o, Vector d, Point* intersection = new Point()) const;
    //! return the cost of traveling between two point
    float path_cost(vec2 start, vec2 end) const;
    //! return the edjey graph using the mask of size 'n' and a map scale of 'scale' (divide nx and ny by 'scale')
    adjacency_list_t get_adjacency_list(int n, int scale) const;
    //! draw the path between start and end on the terrain
    void draw_path(vec2 start, vec2 end, int n=3, int scale=1);
    

    //! return the scalarfield of the slope
    ScalarField2D get_slopes() const;
    //! return the scalarfield of the occlusion
    ScalarField2D get_occlusions(int nb_ray=64);
    //! return the scalarfield of the drain (p an optional parameter to define the norme use to compute the down-flow)
    ScalarField2D get_drains(float p=4) const;
    //! export a colored texture for the terrain. This texture show paths, water, grass, etc
    void export_colored_terrain(const char *file) const;

    //! return the list of Points representing the mesh
    std::vector<vec3> get_positions() const;
    //! return the list of Normals representing the mesh
    std::vector<vec3> get_normals() const;
    //! return the list of uv coordinate representing the mesh
    std::vector<vec2> get_texcoords() const;
    //! return the list of indexes representing the mesh
    std::vector<int> get_indexes() const;
    
};

#endif