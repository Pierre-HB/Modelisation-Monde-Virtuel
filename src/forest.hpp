#ifndef _FOREST_H
#define _FOREST_H

#include "terrain.h"
#include "mat.h"

class Terrain2D;
class City;

struct ForestTile{
    std::vector<vec2> trees; //! list of trees
    float tree_radius; //! radius of trees
    vec2 size; //! size in meter of the tile (in x and in y)

    //! constructor for a forest tile
    ForestTile(float tree_radius, vec2 size, int seed = 42, int nb_max_tree = 1000);
    //! constructor for a forest tile starting from a border
    ForestTile(float tree_radius, vec2 size, const std::vector<vec2> border, int seed=42, int nb_max_tree = 1000);
    //! populate the tile
    void populate(int nb_max_tree, int seed=42);
    //! return the list of tree that are next to the border of the tile
    std::vector<vec2> get_border() const;
    //! shuffle the tree list (specialy after beeing created using a border)
    void shuffle();
};

enum TreeType { oak, fir };


struct Tree{
    vec2 position;
    float radius;
    float proba;
    TreeType type;
    Transform model;

    Tree(vec2 position, float radius, float proba, TreeType type): position(position), radius(radius), proba(proba), type(type) {}
};

//! representation of a forest using a list of forest tile
class Forest{
    private:
    vec2 tileSize;
    std::vector<ForestTile> tiles;
    std::vector<int> shuffle;


    public:
    //! constructor for a forest
    Forest(float tree_radius, vec2 size, int nb_tree_max_per_tile = 10000, int nb_tile = 1);
    //! empty constructor
    Forest(): tileSize(vec2(0, 0)), tiles(), shuffle() {};
    //! return the tile use for the point p
    ForestTile get_tile(vec2 p) const;
    //! populate a terrain with trees
    std::vector<Tree> get_trees(const Terrain2D *terrain) const;
};

#endif