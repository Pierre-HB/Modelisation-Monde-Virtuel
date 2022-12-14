#ifndef _CITY_H
#define _CITY_H

#include "terrain.h"
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "path.hpp"

class Terrain2D;
class City;

const float epsilon = 1.0e-3;

//! subdivide the unit circle into 'nb_direction' equaly spaced points and return the i-th point
inline vec2 get_direction(int i, int nb_direction){
    float angle_precision = 2*M_PI/nb_direction;
    return vec2(cos(i*angle_precision), sin(i*angle_precision));
}

struct crossroad{
    vec2 center;
    std::vector<bool> free_direction;
    int nb_direction;
    float radius;
    City *city;

    //! default constructor
    crossroad(): center(vec2(0, 0)), free_direction(std::vector<bool>()), nb_direction(0), radius(0), city(NULL){}
    //! constructor
    crossroad(vec2 center, int nb_direction, float radius, City *city): center(center), free_direction(std::vector<bool>(nb_direction, true)), nb_direction(nb_direction), radius(radius), city(city){}
    //!return the center of a crossroad next to this one in the direction 'get_direction(direction, nb_direction)'
    vec2 get_neighbor_center(int direction) const;
    //! initialize the free direction (direction in wich the city can grow)
    bool compute_free_direction();
    //! update free_direction : desable direction that lead to a collision with the new crossroad
    //! return True if there is still some free_direction
    bool update_free_direction(crossroad new_crossroad);

    bool check_integrity() const;
    
    crossroad get_random_neighbor() const;
};

class City{
private:
    Terrain2D* terrain;
    std::vector<crossroad> crossroads;
    std::vector<bool> can_grow_crossroads;
    float crossroad_radius;
    float streat_size;
    std::vector<Path> paths;
    std::vector<vec2> entrences;
    
public:
    //! create a city using Elden growth algorithm, starting at center and growing with cells of radius 'crossroas_radius'
    //! For revery cells, only 'nb_direction' are checked. 
    //! streat_size is the road_size of the network of crossroead
    City(Terrain2D* terrain, vec2 center, float crossroad_radius, float streat_size, int nb_direction=32);
    //! add a random crossroad, return True if the crossroad is created
    bool add_random_crossroad();
    //! add an entrence to the city
    void add_entrence(vec2 v);
    //! return if a crossroad can be created here
    bool crossroad_space(vec2 p) const;
    //! getter for crossroads
    std::vector<vec2> get_crossroad_centers() const;
    //! getter for crossroad_radius
    float get_crossroad_radius() const {return crossroad_radius;}
    //! getter for streat_size
    float get_streat_size() const {return streat_size;}
    //! return the two angle of a AABB the city
    std::pair<vec2, vec2> get_bouding_box() const;
    //! Compute paths of the city
    void compute_path(float tolerence=2.f);
    //! return the paths
    std::vector<Path> get_paths() const{return paths;}
    //! return if a point is in any crossroad of the city
    bool in_city(const vec2& v) const;

    bool check_integrity();


};



#endif