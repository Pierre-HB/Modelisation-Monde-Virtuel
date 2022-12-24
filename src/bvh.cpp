#include "bvh.hpp"
#include <math.h>

bool Circle::intersection(Object2D *obj) const
{
    Circle* circle = dynamic_cast<Circle*>(obj);
    if(circle)
    {
        return length(center - circle->get_center()) < r + circle->get_r();
    }

    Rectangle* rec = dynamic_cast<Rectangle*>(obj);
    if(rec)
    {
        vec2 c2c = center - rec->get_center();
        float d1 = abs(dot(c2c, rec->get_axis1())) - rec->get_d1();
        float d2 = abs(dot(c2c, rec->get_axis2())) - rec->get_d2();
        d1 = d1 < 0 ? 0 : d1;
        d2 = d2 < 0 ? 0 : d2;

        float d = sqrt(d1*d1 + d2*d2);
        return d <= r;   
    }
    std::cout << "[???] Intersection circle and unknow form" << std::endl;
    return false;
}

//! project the 'points' on the 'axis' and check if the projection overap with the 
//! segment center around the projection of 'center' with a radius of 'd' 
bool overlap(vec2 center, vec2 axis, float d, std::vector<vec2> points){
    float min_proj = 2*d;
    float max_proj = -2*d;
    for(vec2 p : points){
        float proj = dot(p-center, axis);
        if(proj < min_proj)
            min_proj = proj;
        if(proj > max_proj)
            max_proj = proj;
    }
    if(max_proj >= -d && max_proj <= d)
        return true;
    if(min_proj >= -d && min_proj <= d)
        return true;
    if(min_proj < -d && max_proj > d)
        return true;
    
    return false;
}

bool Rectangle::intersection(Object2D *obj) const
{
    Circle* circle = dynamic_cast<Circle*>(obj);
    if(circle)
    {
        vec2 c2c = circle->get_center() - center;
        float d1_ = abs(dot(c2c, axis1)) - d1;
        float d2_ = abs(dot(c2c, axis2)) - d2;
        d1_ = d1_ < 0 ? 0 : d1_;
        d2_ = d2_ < 0 ? 0 : d2_;

        float d = sqrt(d1_*d1_ + d2_*d2_);
        return d <= circle->get_r();     
    }

    Rectangle* rec = dynamic_cast<Rectangle*>(obj);
    if(rec)
    {
        std::vector<vec2> angles = get_angles();
        std::vector<vec2> angles_ = rec->get_angles();

        //project angles_ on axis1 and check for overlap
        if(!overlap(center, axis1, d1, angles_))
            return false;
            //! no overlap => no intersection
        
        if(!overlap(center, axis2, d2, angles_))
            return false;
        if(!overlap(rec->get_center(), rec->get_axis1(), rec->get_d1(), angles))
            return false;
        if(!overlap(rec->get_center(), rec->get_axis2(), rec->get_d2(), angles))
            return false;
        return true;
    }
    std::cout << "[???] Intersection rectangle and unknow form" << std::endl;
    return false;
}

std::vector<vec2> Rectangle::get_angles() const{
    std::vector<vec2> angles = std::vector<vec2>();
    angles.push_back(center + axis1*d1 + axis2*d2);
    angles.push_back(center + axis1*d1 - axis2*d2);
    angles.push_back(center - axis1*d1 - axis2*d2);
    angles.push_back(center - axis1*d1 + axis2*d2);

    return angles;
}

vec2 Rectangle::get_min_p() const{
    std::vector<vec2> angles = get_angles();

    vec2 min_p = angles[0];
    for(vec2 p : angles){
        if(p.x < min_p.x)
            min_p.x = p.x;
        if(p.y < min_p.y)
            min_p.y = p.y;
    }
    return min_p;
}

vec2 Rectangle::get_max_p() const{
    std::vector<vec2> angles = get_angles();

    vec2 max_p = angles[0];
    for(vec2 p : angles){
        if(p.x > max_p.x)
            max_p.x = p.x;
        if(p.y > max_p.y)
            max_p.y = p.y;
    }
    return max_p;
}


BVH::BVH(std::vector<Object2D*> objs, size_t max_element_per_leaf){


    min_p = objs[0]->get_min_p();
    max_p = objs[0]->get_max_p();
    for(Object2D *obj : objs){
        vec2 min_p_ = obj->get_min_p();
        vec2 max_p_ = obj->get_max_p();
        if(min_p_.x < min_p.x)
            min_p.x = min_p_.x;
        if(min_p_.y < min_p.y)
            min_p.y = min_p_.y;
        if(max_p_.x > max_p.x)
            max_p.x = max_p_.x;
        if(max_p_.y > max_p.y)
            max_p.y = max_p_.y;
    }

    if(objs.size() < max_element_per_leaf){
        type = NODE::leaf;
        left = nullptr;
        right = nullptr;        
        this->objs = objs;
    }else{

        type = NODE::node;
        std::vector<Object2D*> left_points = std::vector<Object2D*>();
        std::vector<Object2D*> right_points = std::vector<Object2D*>();

        vec2 mid = (max_p + min_p)/2;
        vec2 dir = max_p.x - min_p.x > max_p.y - min_p.y ? vec2(1, 0) : vec2(0, 1);

        for(Object2D *obj : objs){
            if(dot(obj->get_center()-mid, dir) < 0)
                left_points.push_back(obj);
            else   
                right_points.push_back(obj);
        }


       

        left = new BVH(left_points, max_element_per_leaf);
        right = new BVH(right_points, max_element_per_leaf);
    }
}

bool aabb_intersection(vec2 min_p1, vec2 max_p1, vec2 min_p2, vec2 max_p2){
    if(max_p1.x <= min_p2.x)
        return false;
    if(max_p2.x <= min_p1.x)
        return false;
    if(max_p1.y <= min_p2.y)
        return false;
    if(max_p2.y <= min_p1.y)
        return false;
    return true;
}

bool BVH::intersection(Object2D* obj) const{
    if(type == NODE::leaf){
        for(Object2D* obj_ : objs)
            if(obj_->intersection(obj))
                return true;
        return false;
    }else{
        vec2 min_p_ = obj->get_min_p();
        vec2 max_p_ = obj->get_max_p();
        if(aabb_intersection(left->min_p, left->max_p, min_p_, max_p_))
            if(left->intersection(obj))
                return true;
        
        if(aabb_intersection(right->min_p, right->max_p, min_p_, max_p_))
            if(right->intersection(obj))
                return true;

         return false;
    }
}

