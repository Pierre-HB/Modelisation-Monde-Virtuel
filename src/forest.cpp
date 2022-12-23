#include "terrain.h"
#include "forest.hpp"
#include "vec.h"
#include <random>
#include <algorithm>

ForestTile::ForestTile(float tree_radius, vec2 size, int seed, int nb_max_tree): tree_radius(tree_radius), size(size) {
    populate(nb_max_tree, seed);
}

ForestTile::ForestTile(float tree_radius, vec2 size, const std::vector<vec2> border, int seed, int nb_max_tree): tree_radius(tree_radius), size(size) {
    for(vec2 position : border)
        trees.push_back(position);
    populate(nb_max_tree, seed);
}

void ForestTile::populate(int nb_max_tree, int seed){
    std::uniform_real_distribution<float> randX(0.0, size.x);
    std::uniform_real_distribution<float> randY(0.0, size.y);
    std::default_random_engine rng(seed); 
    std::vector<vec2> border_extern = std::vector<vec2>();     

    for(int _ = 0; _ < nb_max_tree; _++){
        vec2 new_position = vec2(randX(rng), randY(rng));
        bool intersect = false;
        for(size_t i = 0; i < trees.size(); i++)
            if(length(trees[i] - new_position) < 2*tree_radius){
                intersect = true;
                break;
            }
        for(size_t i = 0; i < border_extern.size(); i++)
            if(length(border_extern[i] - new_position) < 2*tree_radius){
                intersect = true;
                break;
            }
        if(!intersect){
            trees.push_back(new_position); 
            if(new_position.x - tree_radius < 0)
                border_extern.push_back(new_position + vec2(size.x, 0));
            if(new_position.x + tree_radius > size.x)
                border_extern.push_back(new_position - vec2(size.x, 0));
            if(new_position.y - tree_radius < 0)
                border_extern.push_back(new_position + vec2(0, size.y));
            if(new_position.y + tree_radius > size.y)
                border_extern.push_back(new_position - vec2(0, size.y));
        }
                   
    }
}

std::vector<vec2> ForestTile::get_border() const{
    std::vector<vec2> border;
    for(vec2 tree : trees)
        if(tree.x - tree_radius <= 0 || tree.y - tree_radius <= 0 || tree.x + tree_radius >= size.x || tree.x + tree_radius >= size.y)
            border.push_back(tree);
    return border;
}

void ForestTile::shuffle(){
    // std::shuffle(trees.begin(), trees.end(), std::random_device());
    std::shuffle(trees.begin(), trees.end(), std::default_random_engine(42));
}

Forest::Forest(float tree_radius, vec2 size, int nb_tree_max_per_tile, int nb_tile): tileSize(size){
    tiles = std::vector<ForestTile>();
    tiles.push_back(ForestTile(tree_radius, size, 42, nb_tree_max_per_tile));
    for(int i = 1; i < nb_tile; i++)
        tiles.push_back(ForestTile(tree_radius, size, tiles[0].get_border(), 42+i*1234567, nb_tree_max_per_tile));
    shuffle = std::vector<int>();
    for(int i = 0; i < nb_tile; i++){
        tiles[i].shuffle();
        shuffle.push_back(i);
    }
    std::shuffle(shuffle.begin(), shuffle.end(), std::default_random_engine(42));
}

ForestTile Forest::get_tile(vec2 p) const{
    int x = fmod(p.x, tileSize.x);
    int y = fmod(p.y, tileSize.y);
    return tiles[shuffle[(shuffle[x%shuffle.size()]+y)%shuffle.size()]];
}

std::vector<Tree> Forest::get_trees(const Terrain2D *terrain) const{
    TreeType TreeTypes[2] = {oak, fir};
    Vector terrainSize = terrain->get_max_p() - terrain->get_min_p();
    vec2 min_p = vec2(terrain->get_min_p().x, terrain->get_min_p().y);
    vec2 max_p = vec2(terrain->get_max_p().x, terrain->get_max_p().y);
    std::vector<Tree> trees = std::vector<Tree>();
    std::uniform_real_distribution<float> u01(0.0, 1.0);
    std::default_random_engine rng(42); 

    ScalarField2D slopes = terrain->get_slopes();

    for(int i = 0; i*tileSize.x < terrainSize.x; i++){
        for(int j = 0; j*tileSize.y < terrainSize.y; j++){
            vec2 offset = vec2(i*tileSize.x, j*tileSize.y) + min_p;
            ForestTile tile = get_tile(offset);
            // ForestTile tile = tiles[0];
            // ForestTile tile = tiles[shuffle[(shuffle[i%shuffle.size()]+j)%shuffle.size()]];
            //TODO populate trees
             
            for(vec2 p : tile.trees){
                vec2 point = p + offset;
                if(point.x < min_p.x || point.x > max_p.x || point.y < min_p.y || point.y > max_p.y)
                    continue;
                if(terrain->is_water(point.x, point.y)){
                    continue;
                }
                if(terrain->intsersection_with_bvh(point, tile.tree_radius)){
                    continue;
                }
                //add or not the tree in location p+offset
                std::vector<float> proba(2);

                int oak = 0;
                int fir = 1;
                

                //compute proba:

                //...
                proba[fir] = (terrain->height(point.x, point.y)-150)/150;

                float threshold = 1;
                float coef = 2;
                if(slopes.get_value(point) > threshold)
                    proba[fir]/= 1+coef*slopes.get_value(point);
                else{
                    proba[fir]/= 1+coef*(slopes.get_value(point)/threshold)*slopes.get_value(point);
                }

                if(proba[fir] < 0)
                    proba[fir] = 0;
                    
                if(proba[fir] > 1)
                    proba[fir] = 1;

                                
                // proba[fir]/= 1+10*slopes.get_value(point);
                // proba[fir]/=10;
                // proba[fir]=1;
                
                proba[oak] = 1-terrain->height(point.x, point.y)/500;
                if(slopes.get_value(point) > 0.25)
                    proba[oak]/= 1+10*slopes.get_value(point);
                else
                    proba[oak]/= 1+10*(4*slopes.get_value(point))*slopes.get_value(point);

                if(proba[oak] < 0)
                    proba[oak] = 0;
                if(proba[oak] > 1)
                    proba[oak] = 1;



                // proba[oak]/= 1+20*slopes.get_value(point);

                
                
                // proba[oak]/=10;
                // proba[oak]=1;

                //randomly select tree that can grow
                float total_proba = 0;
                for(size_t i = 0; i < proba.size(); i++)
                    if(u01(rng) > proba[i])
                        proba[i] = 0;
                    else
                        total_proba+=proba[i];
                

                //randomly select on tree that will grow among any tree that can grow
                if(total_proba != 0){
                    float selection = u01(rng);
                    for(size_t i = 0; i < proba.size(); i++){
                        if(selection < proba[i]/total_proba){
                            trees.push_back(Tree(p+offset, tile.tree_radius, proba[i], TreeTypes[i]));
                            break;
                        }   
                        else
                            selection-=proba[i]/total_proba;
                    }
                }  
            }  
        }
    }
    return trees;
}
