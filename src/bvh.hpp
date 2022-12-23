#ifndef _BVH_H
#define _BVH_H

#include <vector>
#include "vec.h"

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
    std::vector<vec2> points;
    float radius;

    BVH(std::vector<vec2> points, float radius, size_t max_element_per_leaf = 100);

    //! return if there is an intersection between the BVH and the circle in input
    bool intersection(const vec2& p, float r) const;
};


#endif