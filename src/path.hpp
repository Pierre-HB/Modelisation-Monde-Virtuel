#ifndef _PATH_H
#define _PATH_H

#include "vec.h"
#include <vector>
#include <limits>


//! define a path in the terrain
class Path
{
    private:
    std::vector<vec2> points;
    float path_size;

    public:
    //! Constructor from a vector of vec2
    Path(const std::vector<vec2> &points, float path_size=1.f);
    //! return the list of point defining the path (with a max distance between point of 'precision')
    std::vector<vec2> get_points(float precision=std::numeric_limits<float>::max()) const;
    //! return the size of the path (in meter)
    float get_path_size() const{return path_size;}
};

#endif