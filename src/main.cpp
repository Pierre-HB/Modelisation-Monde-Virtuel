
#include "simple_app.h"
#include "noise.h"
#include "scalar_field.h"
#include "infinit_texture.h"
#include "terrain.h"
#include "dijkstra.hpp"

std::vector<std::vector<float>> gaussian_kernel(int size, float sigma = 1.0f){
    float coef = 1.0/(2*M_PI*sigma*sigma);
    std::vector<std::vector<float>> kernel = std::vector<std::vector<float>>();
    for(int i =-size/2; i <= size/2; i++){
        kernel.push_back(std::vector<float>());
        for(int j = -size/2; j <= size/2; j++){
            kernel[i+size/2].push_back(coef*exp(-(i*i + j*j)/(2*sigma*sigma)));
        }
    }
    return kernel;
}

void erosion(Terrain2D& t){
    //calibrated for a terrain of res 256
    int borne = 1000;
    for(int _ = 0; _ < borne; _++){
        std::cout << "\33[2K\r[" << _ << "/" << borne << "]" << std::flush;
        t+=0.0001 + (-0.01)*t.get_drains(0.1).map([](float x){return float(pow(x, 0.5));})*t.get_slopes().map([](float x){return float(pow(x, 0.8));}) + (0.5)*t.laplacian();
    }
}

void hill(Terrain2D& t){
    for(int _ = 0; _ < 1500; _++)
        t+= (0.0001)*t.laplacian();
}

int main( int argc, char **argv )
{

    // std::vector<std::pair<int, int>> m = create_Mask_neighborhood(4);
    // for(int i = 0; i < m.size(); i++){
    //     std::cout << "(" << m[i].first << ", " << m[i].second << ")" << std::endl;
    // }
    InfinitTexture2D* noise = new InfinitTexture2DFromNoise(new Perlin2D());
    noise = translation(noise, vec2(23, -34));
    noise = zoom(noise, vec2(0.25, 0.25));
    noise = scale(noise, 2);
    noise = rotation(noise, M_PI/7);
    noise = sum(noise, scale(new InfinitTexture2DFromNoise(new Perlin2D()), 0.25));
    
    noise = scale(noise, 500);
    noise = zoom(noise, vec2(1/500.0, 1/500.0));
    // int res = 10;
    int res = 64*4*4;
    
    // Terrain2D t = Terrain2D(perlin_noise, vec2(-5, -5), vec2(5, 5), res, res);
    // Terrain2D t = Terrain2D(noise, vec2(-5, -5.03), vec2(5, 5.03), res, res);
    Terrain2D t = Terrain2D(noise, vec2(-2500, -2500), vec2(2500, 2500), res, res);
    std::vector<std::vector<float>> kernel = gaussian_kernel(11, 3);

    Terrain2D t_ = Terrain2D(t.convolution(kernel));
    // Terrain2D t_ = Terrain2D("height.png", vec2(-5, -5), vec2(5, 5));
    // Terrain2D t_ = t;
    
    // t_.export_as_image("mean.png");
    

    //THIS IS WORKING, it smooth out really well the terrain
    // hill(t_);

    //Geologic equation:
    // erosion(t_);

    // adjacency_list_t adj = t.get_adjacency_list(2, 2);
    // for(int i = 0; i < adj.size(); i++){
    //     for(int j = 0; j < adj[i].size(); j++){
    //         std::cout << i << " -> " << adj[i][j].target << " (" << adj[i][j].weight << ")" << std::endl;
    //     }
    // }

    vec2 city1 = vec2(-1950, -1400);
    vec2 city2 = vec2(2250, -1000);
    vec2 city3 = vec2(2000, 1800);
    vec2 city4 = vec2(500, -1000);
    vec2 city5 = vec2(-1500, 2100);
    std::vector<vec2> cities = std::vector<vec2>({city1, city2, city3, city4, city5});
    float road_size = 1;

    // std::cout << "path length (0->1): ";
    // t_.draw_path(city1, city2, 10*road_size, 3, 4);
    // std::cout << "path length (1->0): ";
    // t_.draw_path(city2, city3, 10*road_size, 3, 4);
    // std::cout << "path length (2->0): ";
    // t_.draw_path(city3, city1, 10*road_size, 3, 4);

    // std::cout << "path length (0->3): ";
    // t_.draw_path(city1, city4, 10*road_size, 3, 4);
    // std::cout << "path length (1->3): ";
    // t_.draw_path(city2, city4, 10*road_size, 3, 4);
    // std::cout << "path length (2->3): ";
    // t_.draw_path(city3, city4, 10*road_size, 3, 4);

    // std::cout << "path length (0->4): ";
    // t_.draw_path(city1, city5, 10*road_size, 3, 4);
    // std::cout << "path length (1->4): ";
    // t_.draw_path(city2, city5, 10*road_size, 3, 4);
    // std::cout << "path length (2->4): ";
    // t_.draw_path(city3, city5, 10*road_size, 3, 4);
    // std::cout << "path length (3->4): ";
    // t_.draw_path(city4, city5, 10*road_size, 3, 4);
    // std::cout << "NETWORK" << std::endl;
    t_.add_paths(t_.get_network_path(cities, 5, 2, 4));
    t_.refine(1);
    t_.add_city(city1, 16, 25, 3);
    t_.add_city(city2, 16, 25, 3);
    t_.add_city(city3, 16, 25, 3);
    t_.add_city(city4, 16, 25, 3);
    t_.add_city(city5, 16, 25, 3);

    t_.connect_cities();

    t_.compute_city_paths(10);

    t_.compute_bvhs();

    float tree_radius = 5;
    Forest forest = Forest(tree_radius, vec2(5000, 5000)/10, 100000, 8);
    t_.comput_trees(forest);


    t_.export_colored_terrain("texture.png", 4);
    t_.export_colored_terrain("texture_.png", 4, false, false, false, false);
    t_.export_colored_terrain("texture_p.png", 4, true, false, false, false);
    t_.export_colored_terrain("texture_ps.png", 4, true, true, false, false);
    t_.export_colored_terrain("texture_psc.png", 4, true, true, true, false);
    t_.export_colored_terrain("texture_psct.png", 4, true, true, true, true);
    std::vector<const char *> texture_files = std::vector<const char*>({"texture_.png", "texture_p.png", "texture_ps.png", "texture_psc.png", "texture_psct.png"});
    t_.apply_water();


    

    std::cout << "p_min : " << t_.get_min_p() << ", p_max : " << t_.get_max_p() << std::endl;
    (t_.derivate_x() + t_.derivate_y()).export_as_image("derivate.png");
    t_.get_slopes().export_as_image("slope.png");
    t_.get_drains().map([](float x){return sqrt(x);}).export_as_image("drain.png");
    (t_.get_drains()*t_.get_slopes()).map([](float x){return sqrt(x);}).export_as_image("drain and slope.png");
    t_.laplacian().export_as_image("laplacian.png");
    t_.export_as_image("height.png");
    if(false)t_.get_occlusions(256).export_as_image("occlusion.png", false);

    
    std::vector<Transform> oaks = t_.get_tree_transform(oak);
    std::vector<Transform> firs = t_.get_tree_transform(fir);
    std::vector<Transform> houses = t_.get_houses_transform();

    for(size_t i = 0; i < oaks.size(); i++)
        oaks[i] = oaks[i]*RotationX(90);
    for(size_t i = 0; i < firs.size(); i++)
        firs[i] = firs[i]*RotationX(90);
    for(size_t i = 0; i < houses.size(); i++)
        houses[i] = houses[i]*RotationX(90);
    
    std::cout << "oaks : " << oaks.size() << std::endl;
    std::cout << "firs : " << firs.size() << std::endl;
    std::cout << "houses : " << houses.size() << std::endl;

    const char* texture_file = "texture.png";
    

    SimpleApp simple_app = SimpleApp(1024, 640, t_.get_positions(), t_.get_normals(), t_.get_texcoords(), texture_files, t_.get_indexes(), oaks, firs, houses);
    // SimpleApp simple_app = SimpleApp(1024, 640, t_.get_positions(), t_.get_normals(), t_.get_texcoords(), texture_file, t_.get_indexes(), oaks, firs, houses);
    // SimpleApp simple_app = SimpleApp(1024, 640, t_.get_positions(), t_.get_normals(), t_.get_indexes());
    
    // SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(),t.get_slopes().get_values_as_color(), t.get_indexes());
    
    // SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(),t.get_slope_color(), t.get_indexes());
    // SimpleApp simple_app = SimpleApp(1024, 640, t.get_positions(), t.get_normals(),t.get_occlusion_color(), t.get_indexes());
    simple_app.run();
    
    return 0;
}
