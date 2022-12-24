#ifndef _TERRAIN_H
#define _TERRAIN_H

#include <vector>
#include <string>
#include "vec.h"
#include "color.h"
#include "infinit_texture.h"
#include "scalar_field.h"
#include "dijkstra.hpp"
#include "path.hpp"
#include "forest.hpp"
#include "bvh.hpp"

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

class City;
#include "city.hpp"

class Terrain2D: public ScalarField2D
{
private:
    float slope_max;
    std::vector<Path> paths;
    std::vector<City> cities;
    std::vector<Tree> trees;
    std::vector<BVH> bvhs;
public:
    Terrain2D(InfinitTexture2D *texture, vec2 min_p, vec2 max_p, int nx, int ny, float tree_radius=5);
    Terrain2D(const char *filename, vec2 min_p, vec2 max_p, float tree_radius=5);
    Terrain2D(const ScalarField2D& sf, float tree_radius=5);
    ~Terrain2D(){};

    //! compute the neighborhood of a node(cell adjacent) with the distance between the two cell and the coeficent of down-flow with the metric L^p
    neighborhood find_neighborhood(int i, int j, float p = 4) const;
    //! return the resulotion in x and y
    vec2 get_resolution() const {return vec2((max_p.x - min_p.x)/(nx-1), (max_p.y - min_p.y)/(ny-1));}
    //! height at the node (i, j)
    float height(int i, int j) const;
    //! height at the point (x, y)
    float height(float x, float y) const;
    //! return if there is water
    bool is_water(float x, float y) const;

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
    //! return the edge graph using the mask of size 'n' and a map scale of 'scale' (divide nx and ny by 'scale')
    adjacency_list_t get_adjacency_list(int n, int scale, int min_x, int min_y, int max_x, int max_y) const;
    //! return the edge graph using the mask of size 'n' and a map scale of 'scale' (divide nx and ny by 'scale')
    adjacency_list_t get_adjacency_list(int n, int scale) const;
    //! draw the path between start and end on the terrain
    void draw_path(vec2 start, vec2 end, float road_size = 1, int scale=1, int n=3);
    //! Draw a network connecting points. With a hight tolerence road might be longer (there will be lless road in the network)
    //! min_x and max_x are the bound of the x index to consider for drawing the graph
    //! min_y and max_y are the bound of the y index to consider for drawing the graph
    std::vector<Path> get_network_path(std::vector<vec2> points, int min_x, int min_y, int max_x, int max_y, float road_size=1, float tolerence=2, int scale=1, int n=3);
    //! Draw a network connecting points. With a hight tolerence road might be longer (there will be lless road in the network)
    std::vector<Path> get_network_path(std::vector<vec2> points, float road_size=1, float tolerence=2, int scale=1, int n=3);
    //! refine the path of the terrain to have a certain precision
    void refine(float precision);
    //! add a set of path on the terrain
    void add_paths(const std::vector<Path>& m_paths);
    //! set the water level at -10m
    void apply_water(float water_level = 0.f);
    //! add a city in the location 'position'
    void add_city(vec2 position, int nb_crossroad, float crossroad_radius=100.f, float streat_size=10.f, int nb_direction=32);
    //! compute paths in every city
    void compute_city_paths(float tolerence=2.f);
    //! conect cities with the paths linking them
    void connect_cities();

    //! compute the list of Tree on the terrain
    void comput_trees(const Forest &forest);
    //! return the list of Transform of the TreeType
    std::vector<Transform> get_tree_transform(TreeType type) const;
    //! compute the BVH of the city, the road and the streets
    void compute_bvhs();
    //! compute the intersection between a circle adn every bvh
    bool intsersection_with_bvh(const vec2& p, float r) const;
    //! compute the list of Transform for the houses of the terrain
    std::vector<Transform> get_houses_transform() const;


    //! return the scalarfield of the slope
    ScalarField2D get_slopes() const;
    //! return the scalarfield of the occlusion
    ScalarField2D get_occlusions(int nb_ray=64);
    //! return the scalarfield of the drain (p an optional parameter to define the norme use to compute the down-flow)
    ScalarField2D get_drains(float p=4) const;
    //! export a colored texture for the terrain. This texture show paths, water, grass, etc
    void export_colored_terrain(const char *file, int scale=1) const;

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