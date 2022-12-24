#include "path.hpp"

Path::Path(const std::vector<vec2> &points, float path_size): points(points), path_size(path_size), start(0), end(points.size()) {}

void Path::refine(float precision){
    points = get_points(precision);
    start = 0;
    end = points.size();
}

std::vector<vec2> Path::get_points(float precision) const{
    std::vector<vec2> new_points;
    for(size_t i = start; i < end-1; i++){
        vec2 u = points[i+1] - points[i];
        float l = length(u);
        int n = l/precision;
        for(float a = 0; a < 1; a+=1.0/n)
            new_points.push_back(points[i+1]*a + points[i]*(1-a));
    }
    new_points.push_back(vec2(points[end-1].x, points[end-1].y));
    return new_points;
}

std::vector<vec2> Path::get_directions(float precision) const{
    std::vector<vec2> new_points = get_points(precision);
    std::vector<vec2> directions = std::vector<vec2>();

    directions.push_back(new_points[1] - new_points[0]);
    directions[0] = directions[0]/length(directions[0]);

    for(size_t i = 1; i < new_points.size()-1; i++){
        directions.push_back(new_points[i+1] - new_points[i-1]);
        directions[i] = directions[i]/length(directions[i]);
    }
    size_t i = new_points.size()-1;
    directions.push_back(new_points[i] - new_points[i-1]);
    directions[i] = directions[i]/length(directions[i]);

    return directions;
}

void Path::cut_before(size_t k){
    if(k > start)
        start = k;
}

void Path::cut_after(size_t k){
    if(k < end-1)
        end = k+1;
}

void Path::cut_before(size_t k, vec2 extremity){
    if(k > start)
        start = k;
    points[start] = extremity;
}
void Path::cut_after(size_t k, vec2 extremity){
    if(k < end-1)
        end = k+1;
    points[end-1] = extremity;
}