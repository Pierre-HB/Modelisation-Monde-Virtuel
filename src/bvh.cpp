#include "bvh.hpp"

BVH::BVH(std::vector<vec2> points, float radius, size_t max_element_per_leaf): radius(radius){


    min_p = points[0] - vec2(radius, radius);
    max_p = points[0] + vec2(radius, radius);
    for(vec2 point : points){
        if(point.x - radius < min_p.x)
            min_p.x = point.x - radius;
        if(point.y - radius < min_p.y)
            min_p.y = point.y - radius;
        if(point.x + radius > max_p.x)
            max_p.x = point.x + radius;
        if(point.y + radius > max_p.y)
            max_p.y = point.y + radius;
    }

    if(points.size() < max_element_per_leaf){
        type = NODE::leaf;
        left = nullptr;
        right = nullptr;        
        this->points = points;
    }else{

        type = NODE::node;
        std::vector<vec2> left_points = std::vector<vec2>();
        std::vector<vec2> right_points = std::vector<vec2>();

        vec2 mid = (max_p + min_p)/2;
        vec2 dir = max_p.x - min_p.x > max_p.y - min_p.y ? vec2(1, 0) : vec2(0, 1);

        for(vec2 point : points){
            if(dot(point-mid, dir) < 0)
                left_points.push_back(point);
            else   
                right_points.push_back(point);
        }


       

        left = new BVH(left_points, radius, max_element_per_leaf);
        right = new BVH(right_points, radius, max_element_per_leaf);
    }
}

bool BVH::intersection(const vec2& p, float r) const{
    if(type == NODE::leaf){
        for(vec2 point : points)
            if(length(point-p) < r + radius)
                return true;
        return false;
    }else{

        if(p.x + radius > left->min_p.x && p.x - radius < left->max_p.x && p.y + radius > left->min_p.y && p.y - radius < left->max_p.y)
            if(left->intersection(p, r))
                return true;
        
        if(p.x + radius > right->min_p.x && p.x - radius < right->max_p.x && p.y + radius > right->min_p.y && p.y - radius < right->max_p.y)
            if(right->intersection(p, r))
                return true;

         return false;
    }
}

