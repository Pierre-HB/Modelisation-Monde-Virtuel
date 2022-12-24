
#include "city.hpp"
#include <math.h>
#include <random>
#include "bvh.hpp"

vec2 crossroad::get_neighbor_center(int direction) const{
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

crossroad crossroad::get_random_neighbor() const{
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

bool crossroad::check_integrity() const{
    for(int i = 0; i < nb_direction; i++)
        if(free_direction[i] && !city->crossroad_space(get_neighbor_center(i)))
            return false;
    return true;
}


City::City(Terrain2D* terrain, vec2 center, float crossroad_radius, float streat_size, int nb_direction): terrain(terrain), crossroad_radius(crossroad_radius), streat_size(streat_size){
    crossroads = std::vector<crossroad>();
    can_grow_crossroads = std::vector<bool>();
    paths = std::vector<Path>();

    
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

void City::add_entrence(vec2 v){
    entrences.push_back(v);
}

std::vector<vec2> City::get_crossroad_centers() const {
    std::vector<vec2> centers;
    for(crossroad node : crossroads)
        centers.push_back(node.center);
    return centers;
}

bool City::crossroad_space(vec2 p) const{
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

std::pair<vec2, vec2> City::get_bouding_box() const{
    std::pair<vec2, vec2> bounds = std::pair<vec2, vec2>(crossroads[0].center, crossroads[0].center);
    for(crossroad node : crossroads){
        if(bounds.first.x > node.center.x-crossroad_radius)
            bounds.first.x = node.center.x-crossroad_radius;
        if(bounds.first.y > node.center.y-crossroad_radius)
            bounds.first.y = node.center.y-crossroad_radius;
        if(bounds.second.x < node.center.x+crossroad_radius)
            bounds.second.x = node.center.x+crossroad_radius;
        if(bounds.second.y < node.center.y+crossroad_radius)
            bounds.second.y = node.center.y+crossroad_radius;
    }
    for(vec2 entrence : entrences){
        if(bounds.first.x > entrence.x)
            bounds.first.x = entrence.x;
        if(bounds.first.y > entrence.y)
            bounds.first.y = entrence.y;
        if(bounds.second.x < entrence.x)
            bounds.second.x = entrence.x;
        if(bounds.second.y < entrence.y)
            bounds.second.y = entrence.y;
    }
    return bounds;
}

void City::compute_path(float tolerence){
    std::pair<vec2, vec2> bounds = get_bouding_box();
    std::pair<int, int> min_angle = terrain->get_coordinate(bounds.first);
    std::pair<int, int> max_angle = terrain->get_coordinate(bounds.second+terrain->get_resolution());
    std::vector<vec2> points = get_crossroad_centers();
    for(vec2 v : entrences)
        points.push_back(v);
    std::vector<Path> m_path = terrain->get_network_path(points, min_angle.first, min_angle.second, max_angle.first+1, max_angle.second+1, streat_size, tolerence);
    
    for(Path p : m_path)
        paths.push_back(p);
}
    
bool City::in_city(const vec2& v) const{
    for(crossroad node : crossroads)
        if(length(v-node.center) < node.radius)
            return true;
    return false;
}

void City::compute_houses(const BVH& bvh){
    houses = std::vector<Transform>();
    street_and_houses_bvh = create_bvh_from_paths(paths);

    float house_longueur = 10+1;
    float house_largeur = 8+1;

    for(size_t i = 0; i < paths.size(); i++){
        
        std::vector<vec2> points = paths[i].get_points(house_longueur);
        std::vector<vec2> directions = paths[i].get_directions(house_longueur);
        
        for(size_t i = 0; i < points.size(); i++){
            vec2 direction = directions[i];
            vec2 tangeante = vec2(direction.y, -direction.x);
            float sens[2] = {1, -1};
            for(float s : sens){
                vec2 center = points[i] + tangeante*s*(house_largeur+paths[i].get_path_size()+1);
                Object2D *rec = new Rectangle(center, direction, house_longueur, house_largeur);

                if(!street_and_houses_bvh.intersection(rec) && !(bvh.intersection(rec))){
                    street_and_houses_bvh.add(rec);
                    float angle = direction.y > 0 ? acos(direction.x) : 2*M_PI - acos(direction.x);
                    
                    houses.push_back(
                        Translation(Vector(center.x, center.y, terrain->height(center.x, center.y)))
                        *RotationZ(degrees(angle)));
                }

            }
        }
    }
}




bool City::check_integrity(){
    bool integrity = true;
    for(crossroad node : crossroads)
        integrity = integrity && node.check_integrity();
    return integrity;
}
