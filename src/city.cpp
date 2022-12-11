
#include "city.hpp"
#include <math.h>
#include <random>


vec2 crossroad::get_neighbor_center(int direction){
    vec2 dir = get_direction(direction, nb_direction);
    return center + dir*(2+epsilon)*radius;
}

bool crossroad::compute_free_direction(){
    int nb_free_direction = 0;
    for(int i = 0; i < nb_direction; i++){
        free_direction[i] = city->crossroad_space(get_neighbor_center(i));
        if(free_direction[i])
            nb_free_direction++;
    }
    return nb_free_direction != 0;
}

bool crossroad::update_free_direction(crossroad new_crossroad){
    int nb_free_direction = 0;
    for(int i = 0; i < nb_direction; i++){
        free_direction[i] = free_direction[i] && length(get_neighbor_center(i)-new_crossroad.center) >= 2*radius;
        if(free_direction[i])
            nb_free_direction++;
    }
    return nb_free_direction != 0;
}

crossroad crossroad::get_random_neighbor(){
    int nb_free_direction = 0;
    for(int i = 0; i < nb_direction; i++)
        if(free_direction[i])
            nb_free_direction++;
    if(nb_free_direction == 0){
        std::cout << "NO FREE DIRECTION TO GENERATE A NEW CROSSROAD" << std::endl;
        return crossroad();
    }
    int rand_dir = rand() % nb_free_direction;

    for(int i = 0; i < nb_direction; i++){
        if(free_direction[i]){
            if(rand_dir == 0)
                return crossroad(get_neighbor_center(i), nb_direction, radius, city);
            rand_dir--;
        }   
    }

    return crossroad();
}

bool crossroad::check_integrity(){
    for(int i = 0; i < nb_direction; i++)
        if(free_direction[i] && !city->crossroad_space(get_neighbor_center(i)))
            return false;
    return true;
}


City::City(Terrain2D* terrain, vec2 center, float crossroad_radius, float streat_size, int nb_direction): terrain(terrain), crossroad_radius(crossroad_radius), streat_size(streat_size){
    crossroads = std::vector<crossroad>();
    can_grow_crossroads = std::vector<bool>();

    
    //no space to create the city
    if(!crossroad_space(center)){
        std::cout << "The city cannot be created, no space for a crossroad at center" << std::endl;
        return;
    }
    crossroad node = crossroad(center, nb_direction, crossroad_radius, this);
    
    can_grow_crossroads.push_back(node.compute_free_direction());
    crossroads.push_back(node);
}

bool City::add_random_crossroad(){
    int nb_can_grow_crossroad = 0;
    for(size_t i = 0; i < can_grow_crossroads.size(); i++)
        if(can_grow_crossroads[i])
            nb_can_grow_crossroad++;
    
    if(nb_can_grow_crossroad == 0){
        std::cout << "The city can no longer grow" << std::endl;
        return false;
    }
    int rand_crossroad = rand() % nb_can_grow_crossroad;
    for(size_t i = 0; i < can_grow_crossroads.size(); i++){
        if(can_grow_crossroads[i]){
            if(rand_crossroad == 0){
                crossroad new_crossroad = crossroads[i].get_random_neighbor();
                can_grow_crossroads.push_back(new_crossroad.compute_free_direction());
                crossroads.push_back(new_crossroad);
                
                for(size_t j = 0; j < can_grow_crossroads.size()-1; j++)
                    can_grow_crossroads[j] = crossroads[j].update_free_direction(new_crossroad);                
                break;
            }
            rand_crossroad--;
        }
    }    


    nb_can_grow_crossroad = 0;
    for(size_t i = 0; i < can_grow_crossroads.size(); i++){
        if(can_grow_crossroads[i])
            nb_can_grow_crossroad++;
    }        

    return nb_can_grow_crossroad != 0;
}

std::vector<vec2> City::get_crossroad_centers() const {
    std::vector<vec2> centers;
    for(crossroad node : crossroads)
        centers.push_back(node.center);
    return centers;
}

bool City::crossroad_space(vec2 p){
    std::pair<int, int> coord = terrain->get_coordinate(p);
    if(terrain->slope(coord.first, coord.second) > 0.6){
        return false;
    }
        
    if(terrain->is_water(p.x, p.y)){
        return false;
    }
        
    for(crossroad node : crossroads)
        if(length(p - node.center) < 2*crossroad_radius)
            return false;
    return true;
    
    
}

bool City::check_integrity(){
    bool integrity = true;
    for(crossroad node : crossroads)
        integrity = integrity && node.check_integrity();
    return integrity;
}
