#ifndef _BVH_H
#define _BVH_H

#include <vector>
#include "vec.h"
#include "path.hpp"

//! an abstract class to compute the intersetion between 2D object
//! such as circle or rectangles
class Object2D
{
    public:
    virtual bool intersection(Object2D *obj) const = 0;
    virtual vec2 get_center() const = 0;
    //! getter for bouding box
    virtual vec2 get_min_p() const = 0;
    //! getter for bouding box
    virtual vec2 get_max_p() const = 0;
};

// class Circle;
// class Rectangle;

class Circle : public Object2D
{
    private:
        vec2 center;
        float r;

    public:
    //! standart constructor
    Circle(vec2 center, float r): center(center), r(r) {};
    //! intersection with Object2D
    bool intersection(Object2D *obj) const;
    //! getter
    vec2 get_center() const {return center;}
    //! getter
    float get_r() const {return r;}
    //! getter for bouding box
    vec2 get_min_p() const {return center - vec2(r, r);}
    //! getter for bouding box
    vec2 get_max_p() const {return center + vec2(r, r);}
};

class Rectangle : public Object2D
{
    private:
    vec2 center; //! center of the rectangle
    vec2 axis1; //! perpendicular direction of one edge
    vec2 axis2; //! perpendicular direction of the second edge
    float d1; //! distance between the center and the edge along axis1
    float d2; //! distance between the center and the edge along axis2

    public:
    //! rectangle definition
    Rectangle(vec2 center, vec2 axis, float d1, float d2): center(center), axis1(axis/length(axis)), axis2(vec2(axis.y, -axis.x)/length(axis)), d1(d1), d2(d2) {}
    //! intersection with Object2D
    bool intersection(Object2D *obj) const;
    //! getter
    vec2 get_center() const {return center;}
    //! getter
    vec2 get_axis1() const {return axis1;}
    //! getter
    vec2 get_axis2() const {return axis2;}
    //! getter
    float get_d1() const {return d1;}
    //! getter
    float get_d2() const {return d2;}
    //! getter for the fourth angles
    std::vector<vec2> get_angles() const;

    //! getter for bouding box
    vec2 get_min_p() const;
    //! getter for bouding box
    vec2 get_max_p() const;


};



enum NODE{leaf, node};

//! node of a BVH. either a node with a left and right child
//! dividing the list of point along the longest axis direction
//! a leaf is composed of at most max_element_per_leaf element
struct BVH{
    NODE type;
    BVH* left;
    BVH* right;
    vec2 min_p;
    vec2 max_p;
    size_t max_element_per_leaf; 
    std::vector<Object2D*> objs;

    BVH(): type(NODE::leaf) {};
    BVH(std::vector<Object2D*> objs, size_t max_element_per_leaf = 100);

    //! return if there is an intersection between the BVH and the circle in input
    bool intersection(Object2D* obj) const;
    //! add an object in the BVH
    void add(Object2D* obj);
};

BVH create_bvh_from_paths(std::vector<Path> paths, float precision=std::numeric_limits<float>::max());



#endif