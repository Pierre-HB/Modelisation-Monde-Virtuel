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
    size_t start;
    size_t end;

    public:
    //! Constructor from a vector of vec2
    Path(const std::vector<vec2> &points, float path_size=1.f);
    //! refine the path to have a certain precision
    void refine(float precision);
    //! return the list of point defining the path (with a max distance between point of 'precision')
    std::vector<vec2> get_points(float precision=std::numeric_limits<float>::max()) const;
    //! return the size of the path (in meter)
    float get_path_size() const{return path_size;}
    //! erase the path before the point k
    void cut_before(size_t k);
    //! erase the path after the point k
    void cut_after(size_t k);
    //! erase the path before the point k and link the path to extremity
    void cut_before(size_t k, vec2 extremity);
    //! erase the path after the point k and link the path to extremity
    void cut_after(size_t k, vec2 extremity);
};

#endif