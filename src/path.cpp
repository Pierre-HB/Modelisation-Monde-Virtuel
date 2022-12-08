#include "path.hpp"

Path::Path(const std::vector<vec2> &points, float path_size): points(points), path_size(path_size) {}


std::vector<vec2> Path::get_points(float precision) const{
    std::vector<vec2> new_points;
    for(size_t i = 0; i < points.size()-1; i++){
        vec2 u = points[i+1] - points[i];
        float l = length(u);
        int n = l/precision;
        for(float a = 0; a < 1; a+=1.0/n)
            new_points.push_back(points[i]*a + points[i+1]*(1-a));
    }
    new_points.push_back(vec2(points[points.size()-1].x, points[points.size()-1].y));
    return new_points;
}
